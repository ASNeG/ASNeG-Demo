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

#include <boost/lexical_cast.hpp>
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

		// create start index
		uint32_t startIdx = 0;
		if (!applicationHReadContext->continousPoint_.empty()) {
			try {
				startIdx = boost::lexical_cast<uint32_t>(applicationHReadContext->continousPoint_);
			} catch (boost::bad_lexical_cast& e) {
				startIdx = 0;
			}
		}

		// create end index
		uint32_t endIdx = numberEntries_;
		if (applicationHReadContext->numValuesPerNode_ != 0) {
			if ((startIdx + applicationHReadContext->numValuesPerNode_) < endIdx) {
				endIdx = startIdx + applicationHReadContext->numValuesPerNode_;
			}
		}

		Log(Debug, "read historical data")
			.parameter("NodeId", applicationHReadContext->nodeId_)
			.parameter("NumValuesPerNode", applicationHReadContext->numValuesPerNode_)
			.parameter("ContinousPoint", applicationHReadContext->continousPoint_)
			.parameter("ReleaseContinousPoint", applicationHReadContext->releaseContinuationPoints_);

		// check if user wants to remove order
		if (applicationHReadContext->continousPoint_.empty() && applicationHReadContext->releaseContinuationPoints_) {
			applicationHReadContext->statusCode_ = Success;
			return;
		}

		// set continuation point
		applicationHReadContext->continousPoint_ = "";
		if (endIdx != numberEntries_ && !applicationHReadContext->releaseContinuationPoints_) {
			std::stringstream ss;
			ss << endIdx;
			applicationHReadContext->continousPoint_ = ss.str();
		}

		// set example data
		applicationHReadContext->dataValueArray_ = constructSPtr<OpcUaDataValueArray>();
		applicationHReadContext->dataValueArray_->resize(endIdx - startIdx);
		boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
		for (uint32_t idx=startIdx; idx<endIdx; idx++) {
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

		uint32_t numberEvents = 5;

		// set example data
		applicationHReadEventContext->eventFieldArray_ = constructSPtr<HistoryEventFieldListArray>();
		applicationHReadEventContext->eventFieldArray_->resize(numberEvents);
		for (uint32_t idx=0; idx<numberEvents; idx++) {

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

			// generate event id
			boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();

			OpcUaByteString byteString;
			byteString.value((char*)&time, sizeof(boost::posix_time::ptime));

			variant = constructSPtr<OpcUaVariant>();
			variant->setValue(byteString);

			baseEventType->eventId(variant);


			// set source node id if necessary
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue(OpcUaNodeId("EventObject", namespaceIndex_));
			baseEventType->sourceNode(variant);

			// set source name if necessary
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue("EventObject");
			baseEventType->sourceName(variant);

			// set time if necessary
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue(OpcUaDateTime(boost::posix_time::microsec_clock::local_time()));
			baseEventType->time(variant);

			// set receive time if necessary
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue(OpcUaDateTime(boost::posix_time::microsec_clock::local_time()));
			baseEventType->receiveTime(variant);

			// set message if necessary
			variant = constructSPtr<OpcUaVariant>();
			variant->setValue(OpcUaLocalizedText("", "EventObject"));
			baseEventType->message(variant);

			// set severity if necessary
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
