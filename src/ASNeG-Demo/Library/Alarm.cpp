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

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaStackCore/BuildInTypes/BuildInTypes.h"
#include "OpcUaStackCore/StandardEventType/OffNormalAlarmType.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNamespaceInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/BrowsePathToNodeId.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNodeReference.h"
#include "OpcUaStackServer/ServiceSetApplication/RegisterForwardMethod.h"
#include "OpcUaStackServer/ServiceSetApplication/FireEvent.h"
#include "ASNeG-Demo/Library/Alarm.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	Alarm::Alarm(void)
	: applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	{
	}

	Alarm::~Alarm(void)
	{
	}

	bool
	Alarm::startup(
		const IOThread::SPtr& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Alarm::startup");

		ioThread_ = ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua information model
		// we will find the right namespace index
		if (!getNamespaceInfo()) {
			return false;
		}

		// get alarm node ids from opc ua information model
		// we can use the browse name to find the node id
		if (!getNodeIds()) {
			return false;
		}

		// get references to alarm nodes from opc ua information model
		if (!createNodeReferences()) {
			return false;
		}

		// register callback functions
		if (!registerCallbacks()) {
			return false;
		}

		// set default values
		ackedState(OpcUaLocalizedText("en", "Acknowledged"));
		ackedState_Id(true);
		confirmedState(OpcUaLocalizedText("en", "Confirmed"));
		confirmedState_Id(true);
		activeState(OpcUaLocalizedText("en", "Inactive"));
		activeState_Id(false);
		enabledState(OpcUaLocalizedText("en", "Enabled"));
		enabledState_Id(true);
		comment(OpcUaLocalizedText("", ""));

		// start timer loop - fire event each 60 seconds
		startTimerLoop();

		return true;
	}

	bool
	Alarm::shutdown(void)
	{
		Log(Debug, "Alarm::shutdown");

		ioThread_->slotTimer()->stop(slotTimerElement_);
		slotTimerElement_.reset();
		return true;
	}

	void
	Alarm::ackedState(const OpcUaLocalizedText& ackedState)
	{
		BaseNodeClass::SPtr baseNodeClass = ackedState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue(ackedState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::ackedState(void)
	{
		OpcUaLocalizedText ackedState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = ackedState_.lock();
		if (baseNodeClass.get() == nullptr) return ackedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(ackedState);

		return ackedState;
	}

	void
	Alarm::ackedState_Id(bool ackedState)
	{
		BaseNodeClass::SPtr baseNodeClass = ackedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue((OpcUaBoolean)ackedState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::ackedState_Id(void)
	{
		OpcUaBoolean ackedState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = ackedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return ackedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(ackedState);

		return ackedState;
	}

	void
	Alarm::confirmedState(const OpcUaLocalizedText& confirmedState)
	{
		BaseNodeClass::SPtr baseNodeClass = confirmedState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue(confirmedState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::confirmedState(void)
	{
		OpcUaLocalizedText confirmedState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = confirmedState_.lock();
		if (baseNodeClass.get() == nullptr) return confirmedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(confirmedState);

		return confirmedState;
	}

	void
	Alarm::confirmedState_Id(bool confirmedState)
	{
		BaseNodeClass::SPtr baseNodeClass = confirmedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue((OpcUaBoolean)confirmedState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::confirmedState_Id(void)
	{
		OpcUaBoolean confirmedState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = confirmedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return confirmedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(confirmedState);

		return confirmedState;
	}

	void
	Alarm::activeState(const OpcUaLocalizedText& activeState)
	{
		BaseNodeClass::SPtr baseNodeClass = activeState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue(activeState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::activeState(void)
	{
		OpcUaLocalizedText activeState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = activeState_.lock();
		if (baseNodeClass.get() == nullptr) return activeState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(activeState);

		return activeState;
	}

	void
	Alarm::activeState_Id(bool activeState)
	{
		BaseNodeClass::SPtr baseNodeClass = activeStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue((OpcUaBoolean)activeState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::activeState_Id(void)
	{
		OpcUaBoolean activeState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = activeStateId_.lock();
		if (baseNodeClass.get() == nullptr) return activeState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(activeState);

		return activeState;
	}

	void
	Alarm::enabledState(const OpcUaLocalizedText& enabledState)
	{
		BaseNodeClass::SPtr baseNodeClass = enabledState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue(enabledState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::enabledState(void)
	{
		OpcUaLocalizedText enabledState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = enabledState_.lock();
		if (baseNodeClass.get() == nullptr) return enabledState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(enabledState);

		return enabledState;
	}

	void
	Alarm::enabledState_Id(bool enabledState)
	{
		BaseNodeClass::SPtr baseNodeClass = enabledStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue((OpcUaBoolean)enabledState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::enabledState_Id(void)
	{
		OpcUaBoolean enabledState;
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = enabledStateId_.lock();
		if (baseNodeClass.get() == nullptr) return enabledState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(enabledState);

		return enabledState;
	}

	void
	Alarm::comment(const OpcUaLocalizedText& comment)
	{
		BaseNodeClass::SPtr baseNodeClass = comment_.lock();
		if (baseNodeClass.get() == nullptr) return;
		OpcUaDataValue dataValue(comment);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::comment(void)
	{
		OpcUaLocalizedText comment("", "");
		OpcUaDataValue dataValue;

		BaseNodeClass::SPtr baseNodeClass = comment_.lock();
		if (baseNodeClass.get() == nullptr) return comment;

		baseNodeClass->getValueSync(dataValue);
		dataValue.getValue(comment);

		return comment;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// private functions
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Alarm::getNamespaceInfo(void)
	{
		GetNamespaceInfo getNamespaceInfo;

		if (!getNamespaceInfo.query(applicationServiceIf_)) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		namespaceIndex_ = getNamespaceInfo.getNamespaceIndex("http://ASNeG-Demo.de/Alarm/");
		if (namespaceIndex_ == -1) {
			std::cout << "namespace index not found: http://ASNeG-Demo.de/Alarm/" << std::endl;
			return false;
		}

		return true;
	}

	bool
	Alarm::getNodeIds(void)
	{
		BrowsePathToNodeId browsePathToNodeId({
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("AckedState")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("AckedState"), OpcUaQualifiedName("Id")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("ConfirmedState")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("ConfirmedState"), OpcUaQualifiedName("Id")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("ActiveState")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("ActiveState"), OpcUaQualifiedName("Id")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("EnabledState")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("EnabledState"), OpcUaQualifiedName("Id")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("Comment")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("Comment"), OpcUaQualifiedName("SourceTimestamp")),

			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("Acknowledge")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("Confirm")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("AddComment")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("Enable")),
			BrowseName(OpcUaNodeId("AlarmObject", namespaceIndex_), OpcUaQualifiedName("Disable")),
		});

		if (!browsePathToNodeId.query(applicationServiceIf_, true)) {
			std::cout << "browse path to node id error" << std::endl;
			return false;
		}

		ackedStateNodeId_ = browsePathToNodeId.nodes()[0];
		ackedStateIdNodeId_ = browsePathToNodeId.nodes()[1];
		confirmedStateNodeId_ = browsePathToNodeId.nodes()[2];
		confirmedStateIdNodeId_ = browsePathToNodeId.nodes()[3];
		activeStateNodeId_ = browsePathToNodeId.nodes()[4];
		activeStateIdNodeId_ = browsePathToNodeId.nodes()[5];
		enabledStateNodeId_ = browsePathToNodeId.nodes()[6];
		enabledStateIdNodeId_ = browsePathToNodeId.nodes()[7];
		commentNodeId_ = browsePathToNodeId.nodes()[8];
		commentSourceTimestampNodeId_ = browsePathToNodeId.nodes()[9];

		acknowlegeNodeId_ = browsePathToNodeId.nodes()[10];
		confirmNodeId_ = browsePathToNodeId.nodes()[11];
		addCommentNodeId_ = browsePathToNodeId.nodes()[12];
		enabledNodeId_ = browsePathToNodeId.nodes()[13];
		disableNodeId_ = browsePathToNodeId.nodes()[14];

		return true;
	}

	bool
	Alarm::createNodeReferences(void)
	{
		GetNodeReference getNodeReference({
			ackedStateNodeId_, ackedStateIdNodeId_,
			confirmedStateNodeId_, confirmedStateIdNodeId_,
			activeStateNodeId_, activeStateIdNodeId_,
			enabledStateNodeId_, enabledStateIdNodeId_,
			commentNodeId_, commentSourceTimestampNodeId_
		});

		if (!getNodeReference.query(applicationServiceIf_, true)) {
	  		std::cout << "response error" << std::endl;
	  		return false;
		}

		ackedState_ = getNodeReference.nodeReferences()[0];
		ackedStateId_ = getNodeReference.nodeReferences()[1];
		confirmedState_ = getNodeReference.nodeReferences()[2];
		confirmedStateId_ = getNodeReference.nodeReferences()[3];
		activeState_ = getNodeReference.nodeReferences()[4];
		activeStateId_ = getNodeReference.nodeReferences()[5];
		enabledState_ = getNodeReference.nodeReferences()[6];
		enabledStateId_ = getNodeReference.nodeReferences()[7];
		comment_ = getNodeReference.nodeReferences()[8];
		commentSourceTimestamp_ = getNodeReference.nodeReferences()[9];

		return true;
	}

	bool
	Alarm::registerCallbacks(void)
	{
		bool rc;
		OpcUaNodeId acknowlegeNodeId(9111);
		OpcUaNodeId confirmNodeId(9113);
		OpcUaNodeId contionRefresh(3875);
		OpcUaNodeId offNormalAlarmType(2782);

		auto acknowledgeCallback = [this](ApplicationMethodContext* applicationMethodContext) {
			acknowledge(applicationMethodContext);
		};
		auto confirmCallback = [this](ApplicationMethodContext* applicationMethodContext) {
			confirm(applicationMethodContext);
		};
		auto enabledCallback = [this](ApplicationMethodContext* applicationMethodContext) {
			enabled(applicationMethodContext);
		};
		auto disableCallback = [this](ApplicationMethodContext* applicationMethodContext) {
			disable(applicationMethodContext);
		};
		auto conditionRefreshCallback = [this](ApplicationMethodContext* applicationMethodContext) {
			conditionRefresh(applicationMethodContext);
		};

		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), acknowlegeNodeId_, acknowledgeCallback)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), acknowlegeNodeId, acknowledgeCallback)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), confirmNodeId_, confirmCallback)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), confirmNodeId, confirmCallback)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), enabledNodeId_, enabledCallback)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), disableNodeId_, disableCallback)) return false;
		if (!registerCallback(offNormalAlarmType, contionRefresh, conditionRefreshCallback)) return false;
		return true;
	}

	bool
	Alarm::registerCallback(
		const OpcUaNodeId& objectNodeId,
		OpcUaNodeId& methodNodeId,
		ApplicationCallback::Method callback
	)
	{
		RegisterForwardMethod registerForwardMethod(objectNodeId, methodNodeId);
		registerForwardMethod.setMethodCallback(callback);
		if (!registerForwardMethod.query(applicationServiceIf_)) {
			std::cout << "registerForwardNode response error" << std::endl;
			return false;
		}
	    return true;
	}

	void
	Alarm::acknowledge(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "acknowledge callback");
		ackedState(OpcUaLocalizedText("en", "Acknowledged"));
		ackedState_Id(true);

		sendAlarmEvent("alarm acknowledged...");
	}

	void
	Alarm::confirm(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "confirm callback");
		ackedState(OpcUaLocalizedText("en", "Acknowledged"));
		ackedState_Id(true);
		confirmedState(OpcUaLocalizedText("en", "Confirmed"));
		confirmedState_Id(true);

		sendAlarmEvent("alarm confirmed...");
	}

	void
	Alarm::enabled(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "enabled callback");
		enabledState(OpcUaLocalizedText("en", "Enabled"));
		enabledState_Id(true);

		sendAlarmEvent("alarm enable...");
	}

	void
	Alarm::disable(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "disable callback");
		enabledState(OpcUaLocalizedText("en", "Disabled"));
		enabledState_Id(false);

		sendAlarmEvent("alarm disable...");
	}

	void
	Alarm::conditionRefresh(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "condition refresh callback");

		sendAlarmEvent("alarm condition refresh...");
	}

	void
	Alarm::startTimerLoop(void)
	{
		Log(Debug, "start Event loop");
		slotTimerElement_ = boost::make_shared<SlotTimerElement>();
		slotTimerElement_->timeoutCallback(boost::bind(&Alarm::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 60000);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	Alarm::timerLoop(void)
	{
		Log(Debug, "activate alarm");

		if (!enabledState_Id()) {
			return;
		}

		ackedState(OpcUaLocalizedText("en", "Unacknowledged"));
		ackedState_Id(false);
		confirmedState(OpcUaLocalizedText("en", "Unconfirmed"));
		confirmedState_Id(false);
		activeState(OpcUaLocalizedText("en", "Active"));
		activeState_Id(true);

		sendAlarmEvent("alarm activated...");
	}

	void
	Alarm::sendAlarmEvent(const std::string& eventMessage)
	{
		OffNormalAlarmType::SPtr event = boost::make_shared<OffNormalAlarmType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		// set condition identifier
		variant = boost::make_shared<OpcUaVariant>(OpcUaNodeId("AlarmObject", namespaceIndex_));
		event->setAlarmConditionType(variant);

		// set event id
		variant = boost::make_shared<OpcUaVariant>(OpcUaByteString("0123456789012345"));
		event->eventId(variant);

		// set condition name
		variant = boost::make_shared<OpcUaVariant>(OpcUaString("OffNormalAlarm"));
		event->conditionName(variant);

		// set branch id
		variant = boost::make_shared<OpcUaVariant>(OpcUaNodeId()); // FIXME:
		event->branchId(variant);

		// set active state
		variant = boost::make_shared<OpcUaVariant>(activeState());
		event->activeState(variant);

		// set active state id
		variant = boost::make_shared<OpcUaVariant>(activeState_Id());
		event->activeState_Id(variant);

		// set acked state
		variant = boost::make_shared<OpcUaVariant>(ackedState());
		event->ackedState(variant);

		// set acked state id
		variant = boost::make_shared<OpcUaVariant>(ackedState_Id());
		event->ackedState_Id(variant);

		// set confirm state
		variant = boost::make_shared<OpcUaVariant>();
		variant->setValue(confirmedState());
		event->confirmedState(variant);

		// set confirm state id
		variant = boost::make_shared<OpcUaVariant>(confirmedState_Id());
		event->confirmedState_Id(variant);

		// set enabled state
		variant = boost::make_shared<OpcUaVariant>(enabledState());
		event->enabledState(variant);

		// set enabled state id
		variant = boost::make_shared<OpcUaVariant>(enabledState_Id());
		event->enabledState_Id(variant);

		// set retain
		variant = boost::make_shared<OpcUaVariant>();
		variant->setValue((OpcUaBoolean)true); // FIXME: todo
		event->retain(variant);

		// set message value
		variant = boost::make_shared<OpcUaVariant>(OpcUaLocalizedText("en", eventMessage.c_str()));
		event->message(variant);

		// set severity message
		variant = boost::make_shared<OpcUaVariant>((OpcUaUInt16)500);
		event->severity(variant);

		// send event on alarm node
		FireEvent fireEvent(OpcUaNodeId("AlarmObject", namespaceIndex_), event);
		if (!fireEvent.fireEvent(applicationServiceIf_)) {
			std::cout << "event response error" << std::endl;
		}
	}

}
