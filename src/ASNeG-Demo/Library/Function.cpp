/*
   Copyright 2016-2017 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/Function.h"

namespace OpcUaServerApplicationDemo
{

	Function::Function(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	, baseNodeClassWMap_()
	, methodCallback_(boost::bind(&Function::method, this, _1))
	{
	}

	Function::~Function(void)
	{
	}

	bool
	Function::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Function::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		// register function callbacks
		OpcUaNodeId object("Function", namespaceIndex_);
		OpcUaNodeId method;

		method.set("func1", namespaceIndex_);
		if (!registerCallbacks(object, method)) return false;
		method.set("func2", namespaceIndex_);
		if (!registerCallbacks(object, method)) return false;
		method.set("func3", namespaceIndex_);
		if (!registerCallbacks(object, method)) return false;
		method.set("funcMult", namespaceIndex_);
		if (!registerCallbacks(object, method)) return false;

		return true;
	}

	bool
	Function::shutdown(void)
	{
		Log(Debug, "Function::shutdown");

		return true;
	}

	bool
	Function::getNamespaceInfo(void)
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
			if (it->second == "http://ASNeG-Demo.de/Function/") {
				namespaceIndex_ = it->first;

				func1_.set(std::string("func1"), namespaceIndex_);
				func2_.set(std::string("func2"), namespaceIndex_);
				func3_.set(std::string("func3"), namespaceIndex_);
				funcMult_.set(std::string("funcMult"), namespaceIndex_);
				return true;
			}
 		}

		Log(Error, "namespace not found in configuration")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/Function/");

		return false;
	}

	bool
	Function::registerCallbacks(OpcUaNodeId& objectNodeId, OpcUaNodeId& methodNodeId)
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
	Function::method(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "method call")
			.parameter("ObjectNodeId", applicationMethodContext->objectNodeId_)
			.parameter("MethodNodeId", applicationMethodContext->methodNodeId_);

		// method func1
		if (applicationMethodContext->methodNodeId_ == func1_) {
			applicationMethodContext->statusCode_ = Success;
		}

		// method func2
		else if (applicationMethodContext->methodNodeId_ == func2_) {
			if (applicationMethodContext->inputArguments_->size() != 0) {
				Log(Debug, "input arguments")
				    .parameter("Arguments", applicationMethodContext->inputArguments_);
			}
			applicationMethodContext->statusCode_ = Success;
		}

		// method func3
		else if (applicationMethodContext->methodNodeId_ == func3_) {
			if (applicationMethodContext->inputArguments_->size() != 0) {
				Log(Debug, "input arguments")
				    .parameter("Arguments", applicationMethodContext->inputArguments_);
			}

			OpcUaVariant::SPtr variant;
			applicationMethodContext->outputArguments_->resize(2);
			variant = constructSPtr<OpcUaVariant>();
			variant->variant((OpcUaUInt32)1);
			applicationMethodContext->outputArguments_->push_back(variant);
			variant = constructSPtr<OpcUaVariant>();
			variant->variant((OpcUaUInt32)2);
			applicationMethodContext->outputArguments_->push_back(variant);

			applicationMethodContext->statusCode_ = Success;
		}

		// method funcMult
		else if (applicationMethodContext->methodNodeId_ == funcMult_) {
			if (applicationMethodContext->inputArguments_->size() != 0) {
				Log(Debug, "input arguments")
				    .parameter("Arguments", applicationMethodContext->inputArguments_);
			}
			if (applicationMethodContext->inputArguments_->size() != 2) {
				applicationMethodContext->statusCode_ = BadInvalidArgument;
				return;
			}

			OpcUaUInt32 value1;
			OpcUaUInt32 value2;
			OpcUaUInt32 value3;
			OpcUaVariant::SPtr variant;

			// read input values
			applicationMethodContext->inputArguments_->get(0,variant);
			if (variant->variantType() != OpcUaBuildInType_OpcUaUInt32) {
				applicationMethodContext->statusCode_ = BadInvalidArgument;
				return;
			}
			value1 = variant->get<OpcUaUInt32>();

			applicationMethodContext->inputArguments_->get(1,variant);
			if (variant->variantType() != OpcUaBuildInType_OpcUaUInt32) {
				applicationMethodContext->statusCode_ = BadInvalidArgument;
				return;
			}
			value2 = variant->get<OpcUaUInt32>();

			// calculate result value
			value3 = value1 * value2;

			// set result value
			applicationMethodContext->outputArguments_->resize(1);
			variant = constructSPtr<OpcUaVariant>();
			variant->variant(value3);
			applicationMethodContext->outputArguments_->push_back(variant);
			applicationMethodContext->statusCode_ = Success;
		}

		else {
			applicationMethodContext->statusCode_ = BadNotSupported;
		}

	}

}
