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
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/HistoricalAccess.h"

namespace OpcUaServerApplicationDemo
{

	HistoricalAccess::HistoricalAccess(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	{
	}

	HistoricalAccess::~HistoricalAccess(void)
	{
	}

	bool
	HistoricalAccess::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "HistoricalAccess::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua information model
		// we will find the right namespace index
		if (!getNamespaceInfo()) {
			return false;
		}

		// register historical data access
		if (!registerHADataCallback()) {
			return false;
		}

		// register historical event access
		if (!registerHAEventCallback()) {
			return false;
		}

		return true;
	}

	bool
	HistoricalAccess::shutdown(void)
	{
		Log(Debug, "HistoricalAccess::shutdown");

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
	HistoricalAccess::getNamespaceInfo(void)
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
			if (it->second == "http://ASNeG-Demo.de/HistoricalAccess/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in opc ua information model")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/HistoricalAccess/");

		return false;
	}

	bool
	HistoricalAccess::registerHADataCallback(void)
	{
		Log(Debug, "registerHADataCallback");

	  	ServiceTransactionRegisterForwardNode::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardNode>();
	  	RegisterForwardNodeRequest::SPtr req = trx->request();
	  	RegisterForwardNodeResponse::SPtr res = trx->response();

	  	OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>("DoubleValue", namespaceIndex_);
	  	req->forwardNodeSync()->readHService().setCallback(boost::bind(&HistoricalAccess::readHValue, this, _1));
	  	req->nodesToRegister()->resize(1);
	  	req->nodesToRegister()->set(nodeId);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "registerHADataCallback - transaction error");
	  		return false;
	  	}

	  	for (uint32_t pos = 0; pos < res->statusCodeArray()->size(); pos++) {
	  		OpcUaStatusCode statusCode;
	  		res->statusCodeArray()->get(pos, statusCode);
	  		if (statusCode != Success) {
	  			Log(Error, "registerHADataCallback - response error");
	  			return false;
	  		}
	  	}

	    return true;
	}

	bool
	HistoricalAccess::registerHAEventCallback(void)
	{
		Log(Debug, "registerHAEventCallback");

	  	ServiceTransactionRegisterForwardNode::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardNode>();
	  	RegisterForwardNodeRequest::SPtr req = trx->request();
	  	RegisterForwardNodeResponse::SPtr res = trx->response();

	  	OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>("EventObject", namespaceIndex_);
	  	req->forwardNodeSync()->readHEService().setCallback(boost::bind(&HistoricalAccess::readHEvent, this, _1));
	  	req->nodesToRegister()->resize(1);
	  	req->nodesToRegister()->set(nodeId);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		Log(Error, "registerHAEventCallback - transaction error");
	  		return false;
	  	}

	  	for (uint32_t pos = 0; pos < res->statusCodeArray()->size(); pos++) {
	  		OpcUaStatusCode statusCode;
	  		res->statusCodeArray()->get(pos, statusCode);
	  		if (statusCode != Success) {
	  			Log(Error, "registerHAEventCallback - response error");
	  			return false;
	  		}
	  	}

	    return true;
	}

	void
	HistoricalAccess::readHValue(ApplicationHReadContext* applicationHReadContext)
	{
	}

	void
	HistoricalAccess::readHEvent(ApplicationHReadEventContext* applicationHReadEventContext)
	{
	}

}
