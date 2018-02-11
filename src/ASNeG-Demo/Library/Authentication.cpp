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
			//return false;
		}

		// register authentication callback
		if (!registerAuthenticationCallback()) {
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
			if (it->second == "http://ASNeG-Demo.de/Authentication/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in configuration")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/Authentication/");

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

}
