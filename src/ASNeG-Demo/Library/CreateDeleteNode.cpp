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
#include "ASNeG-Demo/Library/CreateDeleteNode.h"
#include "OpcUaStackServer/ServiceSetApplication/CreateNodeInstance.h"
#include "OpcUaStackServer/ServiceSetApplication/DeleteNodeInstance.h"

namespace OpcUaServerApplicationDemo
{

	CreateDeleteNode::CreateDeleteNode(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, slotTimerElement_()
	, nodesExist_(false)
	{
	}

	CreateDeleteNode::~CreateDeleteNode(void)
	{
	}

	bool
	CreateDeleteNode::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "CreateDeleteNode::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		slotTimerElement_ = boost::make_shared<SlotTimerElement>();
		slotTimerElement_->timeoutCallback(boost::bind(&CreateDeleteNode::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 30000);
		ioThread_->slotTimer()->start(slotTimerElement_);

		return true;
	}

	bool
	CreateDeleteNode::shutdown(void)
	{
		Log(Debug, "CreateDeleteNode::shutdown");

		if (slotTimerElement_.get() != nullptr) {
			ioThread_->slotTimer()->stop(slotTimerElement_);
			slotTimerElement_.reset();
		}

		return true;
	}

	void
	CreateDeleteNode::timerLoop(void)
	{
		if (nodesExist_) {

			Log(Debug, "Delete node instance")
				.parameter("NodeId", OpcUaNodeId("Dynamic", 1));

			DeleteNodeInstance deleteNodeInstance(OpcUaNodeId("Dynamic", 1));

			if (!deleteNodeInstance.query(applicationServiceIf_)) {
				std::cout << "deleteNodeInstance response error" << std::endl;
				return;
			}

			nodesExist_ = false;

			Log(Debug, "Delete node instance done")
				.parameter("NodeId", OpcUaNodeId("Dynamic", 1));
		}
		else {

			Log(Debug, "Create node instance")
				.parameter("NodeId", OpcUaNodeId("Dynamic", 1));


			CreateNodeInstance createNodeInstance(
				"DynamicVariable",								// name
				NodeClass::EnumVariable,						// node class
				OpcUaNodeId(85),								// parent node id (Objects)
				OpcUaNodeId("Dynamic", 1),						// node id
				OpcUaLocalizedText("en", "DynamicVariable"),	// dispplay name
				OpcUaQualifiedName("DynamicVariable", 1),		// browse name
				OpcUaNodeId(47),								// reference type id
				OpcUaNodeId(62)									// type node id
			);

			if (!createNodeInstance.query(applicationServiceIf_)) {
				std::cout << "createNodeInstance response error" << std::endl;
				return;
			}

			nodesExist_ = true;

			Log(Debug, "Create node instance done")
				.parameter("NodeId", OpcUaNodeId("Dynamic", 1));
		}
	}

}
