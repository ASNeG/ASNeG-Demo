/*
   Copyright 2016-2019 Kai Huebl (kai@huebl-sgh.de)

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

#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/Discovery.h"

namespace OpcUaServerApplicationDemo
{

	Discovery::Discovery(void)
	: findServerCallback_(boost::bind(&Discovery::findServer, this, _1))
	{
	}

	Discovery::~Discovery(void)
	{
	}

	bool
	Discovery::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Discovery::startup");

    	// register discovery callbacks
	  	ServiceTransactionRegisterForwardGlobal::SPtr trx = boost::make_shared<ServiceTransactionRegisterForwardGlobal>();
	  	RegisterForwardGlobalRequest::SPtr req = trx->request();
	  	RegisterForwardGlobalResponse::SPtr res = trx->response();

	  	req->forwardGlobalSync()->findServersService().setCallback(findServerCallback_);

	  	applicationServiceIf.sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "register forward response error")
	  		    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
	  		return false;
	  	}

		return true;
	}

	bool
	Discovery::shutdown(void)
	{
		Log(Debug, "Discovery::shutdown");

		return true;
	}

    void
    Discovery::findServer(ApplicationFindServerContext* applicationFindServerContext)
    {
       	Log(Debug, "find server request")
    			.parameter("EndpointUrl", applicationFindServerContext->endpointUrl_)
    			.parameter("LocaleIds", *applicationFindServerContext->localeIdArraySPtr_)
    			.parameter("ServerUris", *applicationFindServerContext->serverUriArraySPtr_);

       	auto ad = boost::make_shared<ApplicationDescription>();
       	ad->applicationUri().value("opc.tcp://127.0.0.1:8889");
       	ad->productUri().value("urn:ASNeG:ASNeG-Demo");

       	applicationFindServerContext->servers_ = boost::make_shared<ApplicationDescriptionArray>();
       	applicationFindServerContext->servers_->resize(1);
       	applicationFindServerContext->servers_->push_back(ad);

       	applicationFindServerContext->statusCode_ = Success;

#if 0
        	// select server entries
        	ApplicationDescription::Vec adVec;
        	ServerEntry::Map::iterator it;
        	for (it = serverEntryMap_.begin(); it != serverEntryMap_.end(); it++) {
        		ServerEntry::SPtr serverEntry = it->second;

        		// process filter
        		bool filterResult = processFilter(
        			serverEntry,
        			applicationFindServerContext->endpointUrl_,
        			applicationFindServerContext->localeIdArraySPtr_,
        			applicationFindServerContext->serverUriArraySPtr_
        		);
        		if (!filterResult) {
        			continue;
        		}

        		// create application description
        		ApplicationDescription::SPtr ad = boost::make_shared<ApplicationDescription>();
        		ad->applicationUri(serverEntry->registeredServer().serverUri().value());
        		ad->productUri(serverEntry->registeredServer().productUri().value());

        		if (serverEntry->registeredServer().serverNames()->size() > 0) {
        			OpcUaLocalizedText::SPtr serverName;
        			serverEntry->registeredServer().serverNames()->get(0, serverName);
        			ad->applicationName(*serverName);
        		}

        		ad->applicationType(serverEntry->registeredServer().serverType());
        		ad->gatewayServerUri(serverEntry->registeredServer().gatewayServerUri().value());

        		// discoveryProfileUri - todo

        		if (serverEntry->registeredServer().discoveryUrls()->size() > 0) {
        			serverEntry->registeredServer().discoveryUrls()->copyTo(*ad->discoveryUrls());
        		}

        		adVec.push_back(ad);
        	}

        	applicationFindServerContext->servers_ = boost::make_shared<ApplicationDescriptionArray>();
        	if (adVec.size() > 0) {
        	    applicationFindServerContext->servers_->resize(adVec.size());

        	    ApplicationDescription::Vec::iterator it;
        	    for (it = adVec.begin(); it != adVec.end(); it++) {
        	    	ApplicationDescription::SPtr ad = *it;
        	    	applicationFindServerContext->servers_->push_back(ad);
        	    }
        	}

        	applicationFindServerContext->statusCode_ = Success;
#endif
    }

}
