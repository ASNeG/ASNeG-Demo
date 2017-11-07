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
	, enableCallback_(boost::bind(&Alarm::enable, this, _1))
	, disableCallback_(boost::bind(&Alarm::disable, this, _1))
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
		ackedState(true);
		activeState(false);
		enableState(true);

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
	Alarm::ackedState(bool ackedState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = ackedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->set(ackedState);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = ackedState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		if (ackedState) dataValue.variant()->set(constructSPtr<OpcUaLocalizedText>("en","Acknowledged"));
		else dataValue.variant()->set(constructSPtr<OpcUaLocalizedText>("en","NotAcknowledged"));
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::ackedState(void)
	{
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = ackedStateId_.lock();
		if (baseNodeClass.get() == nullptr) return false;

		baseNodeClass->getValueSync(dataValue);
		return dataValue.variant()->get<OpcUaBoolean>();
	}

	void
	Alarm::activeState(bool activeState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = activeStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->set(activeState);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = activeState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		if (activeState) dataValue.variant()->set(constructSPtr<OpcUaLocalizedText>("en","Active"));
		else dataValue.variant()->set(constructSPtr<OpcUaLocalizedText>("en","Inactive"));
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::activeState(void)
	{
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = activeStateId_.lock();
		if (baseNodeClass.get() == nullptr) return false;

		baseNodeClass->getValueSync(dataValue);
		return dataValue.variant()->get<OpcUaBoolean>();
	}

	void
	Alarm::enableState(bool enableState)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = enableStateId_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->set(enableState);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = enableState_.lock();
		if (baseNodeClass.get() == nullptr) return;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		if (enableState) dataValue.variant()->set(constructSPtr<OpcUaLocalizedText>("en","Enabled"));
		else dataValue.variant()->set(constructSPtr<OpcUaLocalizedText>("en","Disabled"));
		baseNodeClass->setValueSync(dataValue);
	}

	bool
	Alarm::enableState(void)
	{
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;

		baseNodeClass = enableStateId_.lock();
		if (baseNodeClass.get() == nullptr) return false;

		baseNodeClass->getValueSync(dataValue);
		return dataValue.variant()->get<OpcUaBoolean>();
	}

	void
	Alarm::comment(const std::string& comment)
	{
		// FIXME: todo
	}

	std::string
	Alarm::comment(void)
	{
		// FIXME: todo
		return "";
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

		req->browseNameArray()->resize(13);
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("AckedState"));
		req->addBrowsePath(*rootNodeId_, OpcUaQualifiedName("AckedState"), OpcUaQualifiedName("Id"));
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
		if (!getNodeIdFromResponse(res, 2, activeStateNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 3, activeStateIdNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 4, enableStateNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 5, enableStateIdNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 6, commentNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 7, commentSourceTimestampNodeId_)) return false;

		if (!getNodeIdFromResponse(res, 8, acknowlegeNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 9, confirmNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 10, addCommentNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 11, enableNodeId_)) return false;
		if (!getNodeIdFromResponse(res, 12, disableNodeId_)) return false;

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

	  	req->nodes()->resize(8);
	  	req->nodes()->push_back(ackedStateNodeId_);
	  	req->nodes()->push_back(ackedStateIdNodeId_);
	  	req->nodes()->push_back(activeStateNodeId_);
	  	req->nodes()->push_back(activeStateIdNodeId_);
	  	req->nodes()->push_back(enableStateNodeId_);
	  	req->nodes()->push_back(enableStateIdNodeId_);
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
		if (!getRefFromResponse(res, 2, activeState_)) return false;
		if (!getRefFromResponse(res, 3, activeStateId_)) return false;
		if (!getRefFromResponse(res, 4, enableState_)) return false;
		if (!getRefFromResponse(res, 5, enableStateId_)) return false;
		if (!getRefFromResponse(res, 6, comment_)) return false;
		if (!getRefFromResponse(res, 7, commentSourceTimestamp_)) return false;

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
		if (!registerCallback(*rootNodeId_, *acknowlegeNodeId_, &acknowledgeCallback_)) return false;
		if (!registerCallback(*rootNodeId_, *confirmNodeId_, &confirmCallback_)) return false;
		if (!registerCallback(*rootNodeId_, *enableNodeId_, &enableCallback_)) return false;
		if (!registerCallback(*rootNodeId_, *disableNodeId_, &disableCallback_)) return false;
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
		// FIXME: todo
	}

	void
	Alarm::confirm(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "confirm callback");
		// FIXME: todo
	}

	void
	Alarm::enable(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "enable callback");
		// FIXME: todo
	}

	void
	Alarm::disable(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "disable callback");
		// FIXME: todo
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
		fireEvent("alarm activated...");
	}

	void
	Alarm::fireEvent(const std::string& eventMessage)
	{
		if (!enableState()) {
			return;
		}

		ackedState(false);
		activeState(true);

		OffNormalAlarmType::SPtr event = constructSPtr<OffNormalAlarmType>();
		EventBase::SPtr eventBase;
		OpcUaVariant::SPtr variant;

		ServiceTransactionFireEvent::SPtr trx = constructSPtr<ServiceTransactionFireEvent>();
		FireEventRequest::SPtr req = trx->request();
		FireEventResponse::SPtr res = trx->response();

		// set active state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaBoolean)activeState());
		event->activeState(variant);

		// set acked state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaBoolean)ackedState());
		event->ackedState(variant);

		// set enable state
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaBoolean)enableState());
		event->enabledState(variant);

		// set message value
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue(OpcUaLocalizedText("en", eventMessage.c_str()));
		event->message(variant);

		// set severity message
		variant = constructSPtr<OpcUaVariant>();
		variant->setValue((OpcUaUInt16)100);
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
