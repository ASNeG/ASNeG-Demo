/*
   Copyright 2017-2018 Kai Huebl (kai@huebl-sgh.de)

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
#include "ASNeG-Demo/Library/Alarm.h"

using namespace OpcUaStackCore;

namespace OpcUaServerApplicationDemo
{

	Alarm::Alarm(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	, acknowledgeCallback_(boost::bind(&Alarm::acknowledge, this, _1))
	, confirmCallback_(boost::bind(&Alarm::confirm, this, _1))
	, enabledCallback_(boost::bind(&Alarm::enabled, this, _1))
	, disableCallback_(boost::bind(&Alarm::disable, this, _1))
	, conditionRefreshCallback_(boost::bind(&Alarm::conditionRefresh, this, _1))
	{
	}

	Alarm::~Alarm(void)
	{
	}

	bool
	Alarm::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Alarm::startup");

		ioThread_ = &ioThread;
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

		return true;
	}

	void
	Alarm::ackedState(const OpcUaLocalizedText& ackedState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = ackedState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(ackedState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::ackedState(void)
	{
		OpcUaLocalizedText ackedState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = ackedState_.lock();
		if (baseNodeClass.get() == nullptr) return ackedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(ackedState);

		return ackedState;
	}

	void
	Alarm::ackedState_Id(bool ackedState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = ackedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(ackedState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::ackedState_Id(void)
	{
		OpcUaBoolean ackedState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = ackedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return ackedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(ackedState);

		return ackedState;
	}

	void
	Alarm::confirmedState(const OpcUaLocalizedText& confirmedState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = confirmedState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(confirmedState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::confirmedState(void)
	{
		OpcUaLocalizedText confirmedState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = confirmedState_.lock();
		if (baseNodeClass.get() == nullptr) return confirmedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(confirmedState);

		return confirmedState;
	}

	void
	Alarm::confirmedState_Id(bool confirmedState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = confirmedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(confirmedState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::confirmedState_Id(void)
	{
		OpcUaBoolean confirmedState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = confirmedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return confirmedState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(confirmedState);

		return confirmedState;
	}

	void
	Alarm::activeState(const OpcUaLocalizedText& activeState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = activeState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(activeState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::activeState(void)
	{
		OpcUaLocalizedText activeState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = activeState_.lock();
		if (baseNodeClass.get() == nullptr) return activeState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(activeState);

		return activeState;
	}

	void
	Alarm::activeState_Id(bool activeState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = activeStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(activeState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::activeState_Id(void)
	{
		OpcUaBoolean activeState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = activeStateId_.lock();
		if (baseNodeClass.get() == nullptr) return activeState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(activeState);

		return activeState;
	}

	void
	Alarm::enabledState(const OpcUaLocalizedText& enabledState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = enabledState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(enabledState);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::enabledState(void)
	{
		OpcUaLocalizedText enabledState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = enabledState_.lock();
		if (baseNodeClass.get() == nullptr) return enabledState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(enabledState);

		return enabledState;
	}

	void
	Alarm::enabledState_Id(bool enabledState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = enabledStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(enabledState);
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::enabledState_Id(void)
	{
		OpcUaBoolean enabledState;
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = enabledStateId_.lock();
		if (baseNodeClass.get() == nullptr) return enabledState;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(enabledState);

		return enabledState;
	}

	void
	Alarm::comment(const OpcUaLocalizedText& comment)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = comment_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue(comment);
		baseNodeClass->setValueSync(dataValue);
	}

	OpcUaLocalizedText
	Alarm::comment(void)
	{
		OpcUaLocalizedText comment("", "");
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = comment_.lock();
		if (baseNodeClass.get() == nullptr) return comment;

		baseNodeClass->getValueSync(dataValue);
		dataValue.variant()->getValue(comment);

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
		OpcUaNodeId acknowlegeNodeId(9111);
		OpcUaNodeId confirmNodeId(9113);
		OpcUaNodeId contionRefresh(3875);
		OpcUaNodeId offNormalAlarmType(2782);

		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), acknowlegeNodeId_, &acknowledgeCallback_)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), acknowlegeNodeId, &acknowledgeCallback_)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), confirmNodeId_, &confirmCallback_)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), confirmNodeId, &confirmCallback_)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), enabledNodeId_, &enabledCallback_)) return false;
		if (!registerCallback(OpcUaNodeId("AlarmObject", namespaceIndex_), disableNodeId_, &disableCallback_)) return false;
		if (!registerCallback(offNormalAlarmType, contionRefresh, &conditionRefreshCallback_)) return false;
		return true;
	}

	bool
	Alarm::registerCallback(const OpcUaNodeId& objectNodeId, OpcUaNodeId& methodNodeId, Callback* callback)
	{
		RegisterForwardMethod registerForwardMethod(objectNodeId, methodNodeId);
		registerForwardMethod.setMethodCallback(*callback);
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
		slotTimerElement_ = constructSPtr<SlotTimerElement>();
		slotTimerElement_->callback().reset(boost::bind(&Alarm::timerLoop, this));
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
		OffNormalAlarmType::SPtr event = constructSPtr<OffNormalAlarmType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		ServiceTransactionFireEvent::SPtr trx = constructSPtr<ServiceTransactionFireEvent>();
		FireEventRequest::SPtr req = trx->request();
		FireEventResponse::SPtr res = trx->response();

		// set condition identifier
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaNodeId("AlarmObject", namespaceIndex_));
		event->setAlarmConditionType(variant);

		// set event id
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaByteString("0123456789012345")); // FIXME:
		event->eventId(variant);

		// set condition name
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaString("OffNormalAlarm")); // FIXME:
		event->conditionName(variant);

		// set branch id
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaNodeId()); // FIXME:
		event->branchId(variant);

		// set active state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(activeState());
		event->activeState(variant);

		// set active state id
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(activeState_Id());
		event->activeState_Id(variant);

		// set acked state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(ackedState());
		event->ackedState(variant);

		// set acked state id
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(ackedState_Id());
		event->ackedState_Id(variant);

		// set confirm state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(confirmedState());
		event->confirmedState(variant);

		// set confirm state id
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(confirmedState_Id());
		event->confirmedState_Id(variant);

		// set enabled state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(enabledState());
		event->enabledState(variant);

		// set enabled state id
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(enabledState_Id());
		event->enabledState_Id(variant);

		// set retain
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaBoolean)true); // FIXME: todo
		event->retain(variant);

		// set message value
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaLocalizedText("en", eventMessage.c_str()));
		event->message(variant);

		// set severity message
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaUInt16)500);
		event->severity(variant);

		// send event on alarm node
		OpcUaNodeId x("AlarmObject", namespaceIndex_);
		req->nodeId(x);
		eventBase = event;
		req->eventBase(eventBase);

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			Log(Debug, "event response error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
		}
	}

}
