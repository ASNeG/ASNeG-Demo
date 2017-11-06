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
#include "ASNeG-Demo/Library/Alarm.h"

namespace OpcUaServerApplicationDemo
{

	Alarm::Alarm(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
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
		if (!getNamespaceInfo()) {
			return false;
		}

		// get node ids from opc ua information model
		if (!getNodeIds()) {
			return false;
		}

		return true;
	}

	bool
	Alarm::shutdown(void)
	{
		Log(Debug, "Alarm::shutdown");

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
		Log(Debug, "get namespace info");

		ServiceTransactionBrowsePathToNodeId::SPtr trx = constructSPtr<ServiceTransactionBrowsePathToNodeId>();
		BrowsePathToNodeIdRequest::SPtr req = trx->request();
		BrowsePathToNodeIdResponse::SPtr res = trx->response();

		BrowseName::SPtr browseName;
		OpcUaQualifiedName::SPtr pathElement;
		rootNodeId_.set("AlarmObject", namespaceIndex_);

		req->browseNameArray()->resize(13);
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("AckedState"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("AckedState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("ActiveState"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("ActiveState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("EnabledState"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("EnabledState"), OpcUaQualifiedName("Id"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("Comment"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("Comment"), OpcUaQualifiedName("SourceTimestamp"));

		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("Acknowledge"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("Confirm"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("AddComment"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("Enable"));
		req->addBrowsePath(rootNodeId_, OpcUaQualifiedName("Disable"));

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
	Alarm::getNodeIdFromResponse(BrowsePathToNodeIdResponse::SPtr& res, uint32_t idx, OpcUaNodeId& nodeId)
	{
		NodeIdResult::SPtr nodeIdResult;
		if (!res->nodeIdResults()->get(idx, nodeIdResult)) {
			Log(Error, "node id result not exist in response")
				.parameter("Idx", idx);
			return false;
		}

		if (nodeIdResult->statusCode() != Success) {
			Log(Error, "node id result not error in response")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(nodeIdResult->statusCode()))
				.parameter("Idx", idx);
			return false;
		}

		nodeId = nodeIdResult->nodeId();
		return true;
	}

}
