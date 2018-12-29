/*
   Copyright 2016-2018 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNamespaceInfo.h"
#include "ASNeG-Demo/Library/ServiceFunction.h"

namespace OpcUaServerApplicationDemo
{

	ServiceFunction::ServiceFunction(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	, baseNodeClassWMap_()
	, methodCallback_(boost::bind(&ServiceFunction::method, this, _1))
	{
	}

	ServiceFunction::~ServiceFunction(void)
	{
	}

	bool
	ServiceFunction::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "ServiceFunction::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		// register function callbacks
		OpcUaNodeId object("Service", namespaceIndex_);
		OpcUaNodeId method;

		method.set("Service.Reload", namespaceIndex_);
		if (!registerCallbacks(object, method)) return false;

		return true;
	}

	bool
	ServiceFunction::shutdown(void)
	{
		Log(Debug, "ServiceFunction::shutdown");

		return true;
	}

	bool
	ServiceFunction::getNamespaceInfo(void)
	{
		GetNamespaceInfo getNamespaceInfo;

		if (!getNamespaceInfo.query(applicationServiceIf_)) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		namespaceIndex_ = getNamespaceInfo.getNamespaceIndex("http://ASNeG-Demo/Service/");
		if (namespaceIndex_ == -1) {
			std::cout << "namespace index not found: http://ASNeG-Demo/Service/" << std::endl;
			return false;
		}

		return true;
	}

	bool
	ServiceFunction::registerCallbacks(OpcUaNodeId& objectNodeId, OpcUaNodeId& methodNodeId)
	{
		Log(Debug, "register method callbacks");

	  	ServiceTransactionRegisterForwardMethod::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardMethod>();
	  	RegisterForwardMethodRequest::SPtr req = trx->request();
	  	RegisterForwardMethodResponse::SPtr res = trx->response();

	  	req->forwardMethodSync()->methodService().setCallback(methodCallback_);
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
	ServiceFunction::method(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "service method call")
			.parameter("ObjectNodeId", applicationMethodContext->objectNodeId_)
			.parameter("MethodNodeId", applicationMethodContext->methodNodeId_);

		// reload function
		if (applicationMethodContext->methodNodeId_ == reloadFunc_) {
			applicationServiceIf_->reload();
			applicationMethodContext->statusCode_ = Success;
		}

		else {
			applicationMethodContext->statusCode_ = BadNotSupported;
		}

	}

}
