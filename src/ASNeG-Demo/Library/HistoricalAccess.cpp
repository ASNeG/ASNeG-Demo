/*
   Copyright 2018 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/ServiceSet/EventField.h"
#include "OpcUaStackCore/StandardEventType/BaseEventType.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/HistoricalAccess.h"

namespace OpcUaServerApplicationDemo
{

	HistoricalAccess::HistoricalAccess(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	{
	}

	HistoricalAccess::~HistoricalAccess(void)
	{
	}

	bool
	HistoricalAccess::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "HistoricalAccess::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua information model
		// we will find the right namespace index
		if (!getNamespaceInfo()) {
			return false;
		}

		// register historical data access
		if (!registerHADataCallback()) {
			return false;
		}

		// register historical event access
		if (!registerHAEventCallback()) {
			return false;
		}

		return true;
	}

	bool
	HistoricalAccess::shutdown(void)
	{
		Log(Debug, "HistoricalAccess::shutdown");

		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// private functions
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	HistoricalAccess::getNamespaceInfo(void)
	{
		Log(Debug, "get namespace info");

		ServiceTransactionNamespaceInfo::SPtr trx = constructSPtr<ServiceTransactionNamespaceInfo>();
		NamespaceInfoRequest::SPtr req = trx->request();
		NamespaceInfoResponse::SPtr res = trx->response();

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			Log(Error, "NamespaceInfoResponse error")
			    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
			return false;
		}

		NamespaceInfoResponse::Index2NamespaceMap::iterator it;
		for (
		    it = res->index2NamespaceMap().begin();
			it != res->index2NamespaceMap().end();
			it++
		)
		{
			if (it->second == "http://ASNeG-Demo.de/HistoricalAccess/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in opc ua information model")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/HistoricalAccess/");

		return false;
	}

	bool
	HistoricalAccess::registerHADataCallback(void)
	{
		Log(Debug, "registerHADataCallback");

	  	ServiceTransactionRegisterForwardNode::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardNode>();
	  	RegisterForwardNodeRequest::SPtr req = trx->request();
	  	RegisterForwardNodeResponse::SPtr res = trx->response();

	  	OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>("DoubleValue", namespaceIndex_);
	  	req->forwardNodeSync()->readHService().setCallback(boost::bind(&HistoricalAccess::readHValue, this, _1));
	  	req->nodesToRegister()->resize(1);
	  	req->nodesToRegister()->set(nodeId);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "registerHADataCallback - transaction error");
	  		return false;
	  	}

	  	for (uint32_t pos = 0; pos < res->statusCodeArray()->size(); pos++) {
	  		OpcUaStatusCode statusCode;
	  		res->statusCodeArray()->get(pos, statusCode);
	  		if (statusCode != Success) {
	  			Log(Error, "registerHADataCallback - response error");
	  			return false;
	  		}
	  	}

	    return true;
	}

	bool
	HistoricalAccess::registerHAEventCallback(void)
	{
		Log(Debug, "registerHAEventCallback");

	  	ServiceTransactionRegisterForwardNode::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardNode>();
	  	RegisterForwardNodeRequest::SPtr req = trx->request();
	  	RegisterForwardNodeResponse::SPtr res = trx->response();

	  	OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>("EventObject", namespaceIndex_);
	  	req->forwardNodeSync()->readHEService().setCallback(boost::bind(&HistoricalAccess::readHEvent, this, _1));
	  	req->nodesToRegister()->resize(1);
	  	req->nodesToRegister()->set(nodeId);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "registerHAEventCallback - transaction error");
	  		return false;
	  	}

	  	for (uint32_t pos = 0; pos < res->statusCodeArray()->size(); pos++) {
	  		OpcUaStatusCode statusCode;
	  		res->statusCodeArray()->get(pos, statusCode);
	  		if (statusCode != Success) {
	  			Log(Error, "registerHAEventCallback - response error");
	  			return false;
	  		}
	  	}

	    return true;
	}

	void
	HistoricalAccess::readHValue(ApplicationHReadContext* applicationHReadContext)
	{
		Log(Debug, "readHValue");

		// set example data
		applicationHReadContext->dataValueArray_ = constructSPtr<OpcUaDataValueArray>();
		applicationHReadContext->dataValueArray_->resize(10);
		boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
		for (uint32_t idx=0; idx<10; idx++) {
			OpcUaDateTime dateTime(time - boost::posix_time::seconds(10+idx));

			OpcUaDataValue::SPtr dataValue = constructSPtr<OpcUaDataValue>();
			dataValue->variant()->variant((double)idx);
			dataValue->statusCode(Success);
			dataValue->sourceTimestamp(dateTime);
			dataValue->serverTimestamp(dateTime);
			applicationHReadContext->dataValueArray_->push_back(dataValue);
		}

		applicationHReadContext->statusCode_ = Success;
	}

	void
	HistoricalAccess::readHEvent(ApplicationHReadEventContext* applicationHReadEventContext)
	{
		Log(Debug, "readHEvent");

		// set example data
		applicationHReadEventContext->eventFieldArray_ = constructSPtr<HistoryEventFieldListArray>();
		applicationHReadEventContext->eventFieldArray_->resize(5);
		for (uint32_t idx=0; idx<5; idx++) {

			//
			// create event
			//
			OpcUaVariant::SPtr variant;
			BaseEventType::SPtr baseEventType = constructSPtr<BaseEventType>();
			EventBase::SPtr eventBase = baseEventType;

			// set message value
			std::stringstream ss;

			ss << "BaseEventType: Event message " << idx;
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue(OpcUaLocalizedText("de", ss.str()));
			baseEventType->message(variant);

			// set severity message
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue((OpcUaUInt16)100);
			baseEventType->severity(variant);

			//
			// filter event
			//
			EventFilter::SPtr eventFilter = applicationHReadEventContext->filter_;
			HistoryEventFieldList::SPtr eventFieldList = constructSPtr<HistoryEventFieldList>();
			eventFieldList->eventFields()->resize(eventFilter->selectClauses()->size());
			applicationHReadEventContext->eventFieldArray_->push_back(eventFieldList);

			for (uint32_t j=0; j<eventFilter->selectClauses()->size(); j++) {

				// get simple attribute operand
				SimpleAttributeOperand::SPtr simpleAttributeOperand;
				eventFilter->selectClauses()->get(j, simpleAttributeOperand);

				std::list<OpcUaQualifiedName::SPtr> browseNameList;
				for (uint32_t j=0; j<simpleAttributeOperand->browsePath()->size(); j++) {
					OpcUaQualifiedName::SPtr browseName;
					simpleAttributeOperand->browsePath()->get(j, browseName);
					browseNameList.push_back(browseName);
				}

				// get variant value from event
				OpcUaVariant::SPtr value;
				EventResult::Code resultCode = eventBase->get(
					simpleAttributeOperand->typeId(),
					browseNameList,
					value
				);

				// insert variant into event field list
				EventField::SPtr eventField;
				eventField = constructSPtr<EventField>();
				if (resultCode != EventResult::Success) {
					value = constructSPtr<OpcUaVariant>();

				}
				else {
				}
				eventField->variant(value);
				eventFieldList->eventFields()->push_back(eventField);
			}
		}

		applicationHReadEventContext->statusCode_ = Success;
	}

}
