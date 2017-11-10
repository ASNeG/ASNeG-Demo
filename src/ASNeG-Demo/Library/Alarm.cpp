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

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaStackCore/BuildInTypes/BuildInTypes.h"
#include "OpcUaStackCore/StandardEventType/OffNormalAlarmType.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
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
			if (it->second == "http://ASNeG-Demo.de/Alarm/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in opc ua information model")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/Alarm/");

		return false;
	}

	bool
	Alarm::getNodeIds(void)
	{
		Log(Debug, "get node ids");

		ServiceTransactionBrowsePathToNodeId::SPtr trx = constructSPtr<ServiceTransactionBrowsePathToNodeId>();
		BrowsePathToNodeIdRequest::SPtr req = trx->request();
		BrowsePathToNodeIdResponse::SPtr res = trx->response();

		BrowseName::SPtr browseName;
		OpcUaQualifiedName::SPtr pathElement;
		rootNodeId_ = constructSPtr<OpcUaNodeId>();
		rootNodeId_->set("AlarmObject", namespaceIndex_);

		req->browseNameArray()->resize(15);
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("AckedState"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("AckedState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("ConfirmedState"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("ConfirmedState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("ActiveState"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("ActiveState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("EnabledState"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("EnabledState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("Comment"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("Comment"), OpcUaQualifiedName("SourceTimestamp"));

		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("Acknowledge"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("Confirm"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("AddComment"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("Enable"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("Disable"));

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			Log(Error, "BrowsePathToNodeIdResponse error")
			    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
			return false;
		}
		if (res->nodeIdResults().get() == nullptr) {
			Log(Error, "BrowsePathToNodeIdResponse error");
			return false;
		}
		if (res->nodeIdResults()->size() != req->browseNameArray()->size()) {
			Log(Error, "BrowsePathToNodeIdResponse size error");
			return false;
		}

		if (!getNodeIdFromResponse(res, 0, ackedStateNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 1, ackedStateIdNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 2, confirmedStateNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 3, confirmedStateIdNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 4, activeStateNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 5, activeStateIdNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 6, enabledStateNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 7, enabledStateIdNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 8, commentNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 9, commentSourceTimestampNodeId_)) return false;

		if (!getNodeIdFromResponse(res, 10, acknowlegeNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 11, confirmNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 12, addCommentNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 13, enabledNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 14, disableNodeId_)) return false;

		return true;
	}

	bool
	Alarm::getNodeIdFromResponse(BrowsePathToNodeIdResponse::SPtr& res, uint32_t idx, OpcUaNodeId::SPtr& nodeId)
	{
		NodeIdResult::SPtr nodeIdResult;
		if (!res->nodeIdResults()->get(idx, nodeIdResult)) {
			Log(Error, "node id result not exist in response")
				.parameter("Idx", idx);
			return false;
		}

		if (nodeIdResult->statusCode() != Success) {
			Log(Error, "node id result error in response")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(nodeIdResult->statusCode()))
				.parameter("Idx", idx);
			return false;
		}

		nodeId = constructSPtr<OpcUaNodeId>();
		*nodeId = nodeIdResult->nodeId();
		return true;
	}

	bool
	Alarm::createNodeReferences(void)
	{
		Log(Debug, "get references");

		ServiceTransactionGetNodeReference::SPtr trx = constructSPtr<ServiceTransactionGetNodeReference>();
		GetNodeReferenceRequest::SPtr req = trx->request();
		GetNodeReferenceResponse::SPtr res = trx->response();

	  	req->nodes()->resize(10);
	  	req->nodes()->push_back(ackedStateNodeId_);
	  	req->nodes()->push_back(ackedStateIdNodeId_);
	  	req->nodes()->push_back(confirmedStateNodeId_);
	  	req->nodes()->push_back(confirmedStateIdNodeId_);
	  	req->nodes()->push_back(activeStateNodeId_);
	  	req->nodes()->push_back(activeStateIdNodeId_);
	  	req->nodes()->push_back(enabledStateNodeId_);
	  	req->nodes()->push_back(enabledStateIdNodeId_);
	  	req->nodes()->push_back(commentNodeId_);
	  	req->nodes()->push_back(commentSourceTimestampNodeId_);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "GetNodeReference error")
	  		    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
	  		return false;
	  	}
		if (res->nodeReferenceArray().get() == nullptr) {
			Log(Error, "GetNodeReference error");
			return false;
		}
		if (res->nodeReferenceArray()->size() != req->nodes()->size()) {
			Log(Error, "GetNodeReference size error");
			return false;
		}

		if (!getRefFromResponse(res, 0, ackedState_)) return false;
		if (!getRefFromResponse(res, 1, ackedStateId_)) return false;
		if (!getRefFromResponse(res, 2, confirmedState_)) return false;
		if (!getRefFromResponse(res, 3, confirmedStateId_)) return false;
		if (!getRefFromResponse(res, 4, activeState_)) return false;
		if (!getRefFromResponse(res, 5, activeStateId_)) return false;
		if (!getRefFromResponse(res, 6, enabledState_)) return false;
		if (!getRefFromResponse(res, 7, enabledStateId_)) return false;
		if (!getRefFromResponse(res, 8, comment_)) return false;
		if (!getRefFromResponse(res, 9, commentSourceTimestamp_)) return false;

		return true;
	}

	bool
	Alarm::getRefFromResponse(GetNodeReferenceResponse::SPtr& res, uint32_t idx, BaseNodeClass::WPtr& ref)
	{
		NodeReference::SPtr nodeReference;
		if (!res->nodeReferenceArray()->get(idx, nodeReference)) {
			Log(Error, "reference result not exist in response")
				.parameter("Idx", idx);
			return false;
		}

		if (nodeReference->statusCode() != Success) {
			Log(Error, "reference error in response")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(nodeReference->statusCode()))
				.parameter("Idx", idx);
			return false;
		}

  		NodeReferenceApplication::SPtr nodeReferenceApplication;
  		nodeReferenceApplication = boost::static_pointer_cast<NodeReferenceApplication>(nodeReference);
  		ref = nodeReferenceApplication->baseNodeClass();

		return true;
	}

	bool
	Alarm::registerCallbacks(void)
	{
		OpcUaNodeId acknowlegeNodeId(9111);
		OpcUaNodeId confirmNodeId(9113);
		OpcUaNodeId contionRefresh(3875);
		OpcUaNodeId offNormalAlarmType(2782);

		if (!registerCallback(*rootNodeId_, *acknowlegeNodeId_, &acknowledgeCallback_)) return false;
		if (!registerCallback(*rootNodeId_, acknowlegeNodeId, &acknowledgeCallback_)) return false;
		if (!registerCallback(*rootNodeId_, *confirmNodeId_, &confirmCallback_)) return false;
		if (!registerCallback(*rootNodeId_, confirmNodeId, &confirmCallback_)) return false;
		if (!registerCallback(*rootNodeId_, *enabledNodeId_, &enabledCallback_)) return false;
		if (!registerCallback(*rootNodeId_, *disableNodeId_, &disableCallback_)) return false;
		if (!registerCallback(offNormalAlarmType, contionRefresh, &conditionRefreshCallback_)) return false;
		return true;
	}

	bool
	Alarm::registerCallback(OpcUaNodeId& objectNodeId, OpcUaNodeId& methodNodeId, Callback* callback)
	{
	  	ServiceTransactionRegisterForwardMethod::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardMethod>();
	  	RegisterForwardMethodRequest::SPtr req = trx->request();
	  	RegisterForwardMethodResponse::SPtr res = trx->response();

	  	req->forwardMethodSync()->methodService().setCallback(*callback);
	  	req->objectNodeId(objectNodeId);
	  	req->methodNodeId(methodNodeId);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		std::cout << "response error" << std::endl;
	  		return false;
	  	}

	  	if (res->statusCode() != Success) {
  			std::cout << "register value error" << std::endl;
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
		variant->setValue(*rootNodeId_);
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
		req->nodeId(*rootNodeId_);
		eventBase = event;
		req->eventBase(eventBase);

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			Log(Debug, "event response error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
		}
	}

}
