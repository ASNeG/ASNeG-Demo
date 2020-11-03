/*
   Copyright 2020 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackServer/ServiceSetApplication/GetNamespaceInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/RegisterForwardMethodAsync.h"
#include "OpcUaStackServer/Forward/ForwardServiceTransaction.h"
#include "ASNeG-Demo/Library/ForwardContext.h"
#include "ASNeG-Demo/Library/FunctionForward.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	FunctionForward::FunctionForward(void)
	: applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	, baseNodeClassWMap_()
	{
	}

	FunctionForward::~FunctionForward(void)
	{
	}

	bool
	FunctionForward::startup(
		const IOThread::SPtr& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "FunctionForward::startup");

		ioThread_ = ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		// register FunctionForward callbacks
		if (!registerCallbacks(OpcUaNodeId("FunctionForward", namespaceIndex_), OpcUaNodeId("func1", namespaceIndex_))) return false;
		if (!registerCallbacks(OpcUaNodeId("FunctionForward", namespaceIndex_), OpcUaNodeId("func2", namespaceIndex_))) return false;
		if (!registerCallbacks(OpcUaNodeId("FunctionForward", namespaceIndex_), OpcUaNodeId("func3", namespaceIndex_))) return false;
		if (!registerCallbacks(OpcUaNodeId("FunctionForward", namespaceIndex_), OpcUaNodeId("funcMult", namespaceIndex_))) return false;

		return true;
	}

	bool
	FunctionForward::shutdown(void)
	{
		Log(Debug, "FunctionForward::shutdown");

		return true;
	}

	bool
	FunctionForward::getNamespaceInfo(void)
	{
		GetNamespaceInfo getNamespaceInfo;

		if (!getNamespaceInfo.query(applicationServiceIf_)) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		namespaceIndex_ = getNamespaceInfo.getNamespaceIndex("http://ASNeG-Demo.de/FunctionForward/");
		if (namespaceIndex_ == -1) {
			std::cout << "namespace index not found: http:http://ASNeG-Demo.de/FunctionForward/" << std::endl;
			return false;
		}

		return true;
	}

	bool
	FunctionForward::registerCallbacks(const OpcUaNodeId& objectNodeId, const OpcUaNodeId& methodNodeId)
	{
		// create application context
		auto applicationContext = boost::make_shared<ForwardContext>();
		applicationContext->forwardCallback(
			[this](ForwardTransaction::SPtr& forwardTransaction) {
				receiveForwardTrx(forwardTransaction);
			}
		);

		// register
		BaseClass::SPtr baseClass = applicationContext;
		RegisterForwardMethodAsync registerForwardMethodAsync(objectNodeId, methodNodeId);
		registerForwardMethodAsync.activate();
		registerForwardMethodAsync.applicationContext(baseClass);
		if (!registerForwardMethodAsync.query(applicationServiceIf_)) {
			std::cout << "registerForwardMethod response error" << std::endl;
			return false;
		}
	    return true;
	}

	void
	FunctionForward::receiveForwardTrx(
		ForwardTransaction::SPtr& forwardTransaction
	)
	{
		// check transaction type
		if (forwardTransaction->nodeTypeRequest() == OpcUaNodeId(OpcUaId_ForwardTransactionMethodRequest_Encoding_DefaultBinary)) {
			receiveMethod(forwardTransaction);
			return;
		}

		forwardTransaction->statusCode(BadServiceUnsupported);
		applicationServiceIf_->sendForwardTrx(forwardTransaction);
	}

	void
	FunctionForward::receiveMethod(
		OpcUaStackServer::ForwardTransaction::SPtr& forwardTransaction
	)
	{
		// get request and response from transaction
		auto forwardTrx = boost::static_pointer_cast<ForwardTransactionCall>(forwardTransaction);
		auto forwardReq = forwardTrx->request();
		auto forwardRes = forwardTrx->response();

		// create result
		forwardRes->result(boost::make_shared<CallMethodResult>());

		// handle method func1
		if (*forwardReq->callMethodRequest()->methodId() == OpcUaNodeId("func1", namespaceIndex_)) {
			// set result
			forwardRes->result()->statusCode(Success);
		}

		// handle method func2
		else if (*forwardReq->callMethodRequest()->methodId() == OpcUaNodeId("func2", namespaceIndex_)) {
			if (forwardReq->callMethodRequest()->inputArguments()->size() != 0) {
				Log(Debug, "input arguments")
					.parameter("Arguments", forwardReq->callMethodRequest()->inputArguments());
			}

			// set result
			forwardRes->result()->statusCode(Success);
		}

		// handle method func3
		else if (*forwardReq->callMethodRequest()->methodId() == OpcUaNodeId("func3", namespaceIndex_)) {
			if (forwardReq->callMethodRequest()->inputArguments()->size() != 0) {
				Log(Debug, "input arguments")
					.parameter("Arguments", forwardReq->callMethodRequest()->inputArguments());
			}

			OpcUaVariant::SPtr variant;
			forwardRes->result()->outputArguments()->resize(2);
			variant = boost::make_shared<OpcUaVariant>();
			variant->variant((OpcUaUInt32)1);
			forwardRes->result()->outputArguments()->push_back(variant);
			variant = boost::make_shared<OpcUaVariant>();
			variant->variant((OpcUaUInt32)2);
			forwardRes->result()->outputArguments()->push_back(variant);

			// set result
			forwardRes->result()->statusCode(Success);
		}

		// handle method func4
		else if (*forwardReq->callMethodRequest()->methodId() == OpcUaNodeId("funcMult", namespaceIndex_)) {
			bool rc = true;

			if (forwardReq->callMethodRequest()->inputArguments()->size() != 0) {
				Log(Debug, "input arguments")
					.parameter("Arguments", forwardReq->callMethodRequest()->inputArguments());
			}
			if (forwardReq->callMethodRequest()->inputArguments()->size() != 2) {
				forwardRes->result()->statusCode(BadInvalidArgument);
				rc = false;
			}

			OpcUaUInt32 value1;
			OpcUaUInt32 value2;
			OpcUaUInt32 value3;
			OpcUaVariant::SPtr variant;

			// read input values
			if (rc) {
				forwardReq->callMethodRequest()->inputArguments()->get(0,variant);
				if (variant->variantType() != OpcUaBuildInType_OpcUaUInt32) {
					forwardRes->result()->statusCode(BadInvalidArgument);
					rc = false;
				}
				else {
					value1 = variant->get<OpcUaUInt32>();
				}
			}

			if (rc) {
				forwardReq->callMethodRequest()->inputArguments()->get(1,variant);
				if (variant->variantType() != OpcUaBuildInType_OpcUaUInt32) {
					forwardRes->result()->statusCode(BadInvalidArgument);
					rc = false;
				}
				else {
					value2 = variant->get<OpcUaUInt32>();
				}
			}

			// calculate result value
			value3 = value1 * value2;

			if (rc) {
				// set result value
				forwardRes->result()->outputArguments()->resize(1);
				variant = boost::make_shared<OpcUaVariant>();
				variant->variant(value3);
				forwardRes->result()->outputArguments()->push_back(variant);

				// set result
				forwardRes->result()->statusCode(Success);
			}
		}

		else {
			forwardRes->result()->statusCode(BadServiceUnsupported);
		}

		// send answer
		applicationServiceIf_->sendForwardTrx(forwardTransaction);
	}

	void
	FunctionForward::method(ApplicationMethodContext* applicationMethodContext)
	{
		Log(Debug, "method call")
			.parameter("ObjectNodeId", applicationMethodContext->objectNodeId_)
			.parameter("MethodNodeId", applicationMethodContext->methodNodeId_);

		// method func1
		if (applicationMethodContext->methodNodeId_ == OpcUaNodeId("func1", namespaceIndex_)) {
			applicationMethodContext->statusCode_ = Success;
		}

		// method func2
		else if (applicationMethodContext->methodNodeId_ == OpcUaNodeId("func2", namespaceIndex_)) {
			if (applicationMethodContext->inputArguments_->size() != 0) {
				Log(Debug, "input arguments")
				    .parameter("Arguments", applicationMethodContext->inputArguments_);
			}
			applicationMethodContext->statusCode_ = Success;
		}

		// method func3
		else if (applicationMethodContext->methodNodeId_ == OpcUaNodeId("func3", namespaceIndex_)) {
			if (applicationMethodContext->inputArguments_->size() != 0) {
				Log(Debug, "input arguments")
				    .parameter("Arguments", applicationMethodContext->inputArguments_);
			}

			OpcUaVariant::SPtr variant;
			applicationMethodContext->outputArguments_->resize(2);
			variant = boost::make_shared<OpcUaVariant>();
			variant->variant((OpcUaUInt32)1);
			applicationMethodContext->outputArguments_->push_back(variant);
			variant = boost::make_shared<OpcUaVariant>();
			variant->variant((OpcUaUInt32)2);
			applicationMethodContext->outputArguments_->push_back(variant);

			applicationMethodContext->statusCode_ = Success;
		}

		// method funcMult
		else if (applicationMethodContext->methodNodeId_ == OpcUaNodeId("funcMult", namespaceIndex_)) {
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
			variant = boost::make_shared<OpcUaVariant>();
			variant->variant(value3);
			applicationMethodContext->outputArguments_->push_back(variant);
			applicationMethodContext->statusCode_ = Success;
		}

		else {
			applicationMethodContext->statusCode_ = BadNotSupported;
		}

	}

}
