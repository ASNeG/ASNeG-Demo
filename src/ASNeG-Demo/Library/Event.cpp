/*
   Copyright 2017-2020 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackServer/ServiceSetApplication/GetNamespaceInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/RegisterForwardGlobal.h"
#include "OpcUaStackServer/ServiceSetApplication/FireEvent.h"
#include "ASNeG-Demo/Library/Event.h"
#include "ASNeG-Demo/CustomerEventType/CustomerEventType.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	Event::Event(void)
	: applicationServiceIf_(nullptr)
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
		const IOThread::SPtr& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Event::startup");

		ioThread_ = ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		// register event callbacks
		if (!registerEventCallbacks()) {
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


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// private functions
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	Event::eventItemStartCallback(ApplicationEventItemStartContext* applicationEventItemStartContext)
	{
		Log(Debug, "Event::eventItemStartCallback");
	}

	void
	Event::eventItemStopCallback(ApplicationEventItemStopContext* applicationEventItemStopContext)
	{
		Log(Debug, "Event::eventItemStopCallback");
	}

	bool
	Event::registerEventCallbacks(void)
	{
		RegisterForwardGlobal registerForwardGlobal;
		registerForwardGlobal.setEventItemStartCallback(boost::bind(&Event::eventItemStartCallback, this, _1));
		registerForwardGlobal.setEventItemStopCallback(boost::bind(&Event::eventItemStopCallback, this, _1));
		if (!registerForwardGlobal.query(applicationServiceIf_)) {
			std::cout << "registerForwardGlobal response error" << std::endl;
			return false;
		}
	    return true;
	}

	bool
	Event::getNamespaceInfo(void)
	{
		GetNamespaceInfo getNamespaceInfo;

		if (!getNamespaceInfo.query(applicationServiceIf_)) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		namespaceIndex_ = getNamespaceInfo.getNamespaceIndex("http://ASNeG-Demo.de/Event/");
		if (namespaceIndex_ == -1) {
			std::cout << "namespace index not found: http://ASNeG-Demo.de/Event/" << std::endl;
			return false;
		}

		return true;
	}

	void
	Event::startTimerLoop(void)
	{
		Log(Debug, "start Event loop");
		slotTimerElement_ = boost::make_shared<SlotTimerElement>();
		slotTimerElement_->timeoutCallback(boost::bind(&Event::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 5000);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	Event::timerLoop(void)
	{
		Log(Debug, "send Event 11");
		sendEvent11();
		Log(Debug, "send Event 12");
		sendEvent12();
		Log(Debug, "send Event 21");
		sendEvent21();
		Log(Debug, "send Event done");
	}

	void
	Event::sendEvent11(void)
	{
		BaseEventType::SPtr baseEventType = boost::make_shared<BaseEventType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		// set message value
		std::stringstream ss;
		counter_++;
		ss << "BaseEventType: Event message " << counter_;
		variant = boost::make_shared<OpcUaVariant>(OpcUaLocalizedText("de", ss.str()));
		baseEventType->message(variant);

		// set severity message
		variant = boost::make_shared<OpcUaVariant>((OpcUaUInt16)100);
		baseEventType->severity(variant);

		// send event on node Event11
		FireEvent fireEvent(OpcUaNodeId("Event11", namespaceIndex_), baseEventType);
		if (!fireEvent.fireEvent(applicationServiceIf_)) {
			std::cout << "event response error" << std::endl;
		}
	}

	void
	Event::sendEvent12(void)
	{
		CustomerEventType::SPtr customerEventType = boost::make_shared<CustomerEventType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		// set message value
		std::stringstream ss;
		counter_++;
		ss << "CustomerEventType: Event message " << counter_;
		variant = boost::make_shared<OpcUaVariant>(OpcUaLocalizedText("de", ss.str()));
		customerEventType->message(variant);

		// set severity message
		variant = boost::make_shared<OpcUaVariant>((OpcUaUInt16)100);
		customerEventType->severity(variant);

		// set variable1
		variant = boost::make_shared<OpcUaVariant>((OpcUaDouble)1234);
		customerEventType->variable1(variant);

		// set variable2
		variant = boost::make_shared<OpcUaVariant>((OpcUaDouble)5678);
		customerEventType->variable2(variant);

		// send event on node Event12
		FireEvent fireEvent(OpcUaNodeId("Event12", namespaceIndex_), customerEventType);
		if (!fireEvent.fireEvent(applicationServiceIf_)) {
			std::cout << "event response error" << std::endl;
		}
	}

	void
	Event::sendEvent21(void)
	{
		BaseEventType::SPtr eventType = boost::make_shared<AlarmConditionType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		// set message value
		std::stringstream ss;
		counter_++;
		ss << "BaseEventType: Event message " << counter_;
		variant = boost::make_shared<OpcUaVariant>(OpcUaLocalizedText("de", ss.str()));
		eventType->message(variant);

		// set severity message
		variant = boost::make_shared<OpcUaVariant>((OpcUaUInt16)100);
		eventType->severity(variant);

		// send event on node Event21
		FireEvent fireEvent(OpcUaNodeId("Event21", namespaceIndex_), eventType);
		if (!fireEvent.fireEvent(applicationServiceIf_)) {
			std::cout << "event response error" << std::endl;
		}
	}

}
