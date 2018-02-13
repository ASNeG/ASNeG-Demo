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
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/Authentication.h"

namespace OpcUaServerApplicationDemo
{

	Authentication::Authentication(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, authenticationCallback_(boost::bind(&Authentication::authenticationCallback, this, _1))
	, autorizationCallback_(boost::bind(&Authentication::autorizationCallback, this, _1))
	, namespaceIndex_(0)
	{
	}

	Authentication::~Authentication(void)
	{
	}

	bool
	Authentication::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Authentication::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		// register authentication callback
		if (!registerAuthenticationCallback()) {
			return false;
		}

		// creatre node references and set variables to default
		if (!setValuesToDefault()) {
			return false;
		}

		return true;
	}

	bool
	Authentication::shutdown(void)
	{
		Log(Debug, "Authentication::shutdown");

		return true;
	}

	bool
	Authentication::getNamespaceInfo(void)
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
			if (it->second == "http://ASNeG-Demo.de/Auth/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in configuration")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/Auth/");

		return false;
	}

	bool
	Authentication::registerAuthenticationCallback(void)
	{
		Log(Debug, "registern authenitcation callbacks");

		ServiceTransactionRegisterForwardGlobal::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardGlobal>();
		RegisterForwardGlobalRequest::SPtr req = trx->request();
		RegisterForwardGlobalResponse::SPtr res = trx->response();

		req->forwardGlobalSync()->authenticationService().setCallback(authenticationCallback_);
		req->forwardGlobalSync()->autorizationService().setCallback(autorizationCallback_);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "send authentication request error");
	  		return false;
	  	}

	  	if (res->statusCode() != Success) {
	  		Log(Debug, "authentication response error");
  			return false;
	  	}

	  	return true;
	}

	void
	Authentication::authenticationCallback(ApplicationAuthenticationContext* applicationAuthenitcationContext)
	{
		Log(Debug, "Event::authenticationCallback");

		applicationAuthenitcationContext->statusCode_ = Success;
	}

	void
	Authentication::autorizationCallback(ApplicationAutorizationContext* applicationAutorizationContext)
	{
		Log(Debug, "Event::autorizationCallback");

		applicationAutorizationContext->statusCode_ = Success;
	}

	bool
	Authentication::setValuesToDefault(void)
	{
		Log(Debug, "get references");

		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;
		OpcUaDataValue dataValue;
		OpcUaNodeId::SPtr nodeId;

		ServiceTransactionGetNodeReference::SPtr trx = constructSPtr<ServiceTransactionGetNodeReference>();
		GetNodeReferenceRequest::SPtr req = trx->request();
		GetNodeReferenceResponse::SPtr res = trx->response();

	  	req->nodes()->resize(5);
	  	nodeId = constructSPtr<OpcUaNodeId>();
	  	nodeId->set("Auth.Value01", namespaceIndex_);
	  	req->nodes()->push_back(nodeId);
	  	nodeId = constructSPtr<OpcUaNodeId>();
	  	nodeId->set("Auth.Value02", namespaceIndex_);
	  	req->nodes()->push_back(nodeId);
	  	nodeId = constructSPtr<OpcUaNodeId>();
	  	nodeId->set("Auth.Value03", namespaceIndex_);
	  	req->nodes()->push_back(nodeId);
	  	nodeId = constructSPtr<OpcUaNodeId>();
	  	nodeId->set("Auth.Value04", namespaceIndex_);
	  	req->nodes()->push_back(nodeId);
	  	nodeId = constructSPtr<OpcUaNodeId>();
	  	nodeId->set("Auth.Value05", namespaceIndex_);
	  	req->nodes()->push_back(nodeId);

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

		if (!getRefFromResponse(res, 0, value01_)) return false;
		if (!getRefFromResponse(res, 1, value02_)) return false;
		if (!getRefFromResponse(res, 2, value03_)) return false;
		if (!getRefFromResponse(res, 3, value04_)) return false;
		if (!getRefFromResponse(res, 4, value05_)) return false;

		baseNodeClass = value01_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue((double)1);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = value02_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue((double)2);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = value03_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue((double)3);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = value04_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue((double)4);
		baseNodeClass->setValueSync(dataValue);

		baseNodeClass = value05_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		dataValue.serverTimestamp(dateTime);
		dataValue.sourceTimestamp(dateTime);
		dataValue.statusCode(Success);
		dataValue.variant()->setValue((double)5);
		baseNodeClass->setValueSync(dataValue);

		return true;
	}

	bool
	Authentication::getRefFromResponse(GetNodeReferenceResponse::SPtr& res, uint32_t idx, BaseNodeClass::WPtr& ref)
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

}
