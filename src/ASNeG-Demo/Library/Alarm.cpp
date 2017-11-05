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
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
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
		OpcUaNodeId nodeId;
		nodeId.set(5003, namespaceIndex_);

		req->browseNameArray()->resize(2);

		browseName = constructSPtr<BrowseName>();
		browseName->set(nodeId, OpcUaQualifiedName("AckedState"));
		req->browseNameArray()->push_back(browseName);

		browseName = constructSPtr<BrowseName>();
		browseName->set(nodeId, OpcUaQualifiedName("AckedState"), OpcUaQualifiedName("Id"));
		req->browseNameArray()->push_back(browseName);

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			Log(Error, "BrowsePathToNodeIdResponse error")
			    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
			return false;
		}

		for (uint32_t idx = 0; idx < res->nodeIdResults()->size(); idx++) {
			NodeIdResult::SPtr nodeIdResult;
			res->nodeIdResults()->get(idx, nodeIdResult);

			std::cout << "StatusCode=" << (uint32_t)nodeIdResult->statusCode() << std::endl;
			std::cout << "NodeId=" << nodeIdResult->nodeId() << std::endl;
		}

		return true;
	}

}
