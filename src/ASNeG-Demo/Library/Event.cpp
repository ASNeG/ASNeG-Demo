/*
   Copyright 2017 Kai Huebl (kai@huebl-sgh.de)

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

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaStackCore/StandardEventType/BaseEventType.h"
#include "OpcUaStackCore/StandardEventType/AlarmConditionType.h"
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/Event.h"
#include "ASNeG-Demo/CustomerEventType/CustomerEventType.h"

namespace OpcUaServerApplicationDemo
{

	Event::Event(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	, counter_(0)
	{
	}

	Event::~Event(void)
	{
	}

	bool
	Event::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Event::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		startTimerLoop();

		return true;
	}

	bool
	Event::shutdown(void)
	{
		Log(Debug, "Event::shutdown");

		return true;
	}

	bool
	Event::getNamespaceInfo(void)
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
			if (it->second == "http://ASNeG-Demo.de/Event/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in configuration")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/Event/");

		return false;
	}

	void
	Event::startTimerLoop(void)
	{
		slotTimerElement_ = constructSPtr<SlotTimerElement>();
		slotTimerElement_->callback().reset(boost::bind(&Event::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 5000);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	Event::timerLoop(void)
	{
		sendEvent11();
		sendEvent12();
		//sendEvent21();
	}

	void
	Event::sendEvent11(void)
	{
		BaseEventType::SPtr baseEventType = constructSPtr<BaseEventType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		ServiceTransactionFireEvent::SPtr trx = constructSPtr<ServiceTransactionFireEvent>();
		FireEventRequest::SPtr req = trx->request();
		FireEventResponse::SPtr res = trx->response();

		// set message value
		std::stringstream ss;
		counter_++;
		ss << "BaseEventType: Event message " << counter_;
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaLocalizedText("de", ss.str()));
		baseEventType->message(variant);

		// set severity message
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaUInt16)100);
		baseEventType->severity(variant);

		// send event on node Event11
		req->nodeId().set("Event11", namespaceIndex_);
		eventBase = baseEventType;
		req->eventBase(eventBase);

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			  std::cout << "event response error" << std::endl;
		}
	}

	void
	Event::sendEvent12(void)
	{
		CustomerEventType::SPtr customerEventType = constructSPtr<CustomerEventType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		ServiceTransactionFireEvent::SPtr trx = constructSPtr<ServiceTransactionFireEvent>();
		FireEventRequest::SPtr req = trx->request();
		FireEventResponse::SPtr res = trx->response();

		// set message value
		std::stringstream ss;
		counter_++;
		ss << "CustomerEventType: Event message " << counter_;
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaLocalizedText("de", ss.str()));
		customerEventType->message(variant);

		// set severity message
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaUInt16)100);
		customerEventType->severity(variant);

		// set variable1
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaDouble)1234);
		customerEventType->variable1(variant);

		// set variable2
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaDouble)5678);
		customerEventType->variable2(variant);

		// send event on node Event12
		req->nodeId().set("Event12", namespaceIndex_);
		eventBase = customerEventType;
		req->eventBase(eventBase);

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			  std::cout << "event response error" << std::endl;
		}
	}

	void
	Event::sendEvent21(void)
	{
		BaseEventType::SPtr eventType = constructSPtr<AlarmConditionType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		ServiceTransactionFireEvent::SPtr trx = constructSPtr<ServiceTransactionFireEvent>();
		FireEventRequest::SPtr req = trx->request();
		FireEventResponse::SPtr res = trx->response();

		// set message value
		std::stringstream ss;
		counter_++;
		ss << "BaseEventType: Event message " << counter_;
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaLocalizedText("de", ss.str()));
		eventType->message(variant);

		// set severity message
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaUInt16)100);
		eventType->severity(variant);

		// send event on node Event11
		req->nodeId().set("Event21", namespaceIndex_);
		eventBase = eventType;
		req->eventBase(eventBase);

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			  std::cout << "event response error" << std::endl;
		}
	}

}
