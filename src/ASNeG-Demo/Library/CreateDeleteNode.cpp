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

namespace OpcUaServerApplicationDemo
{

	CreateDeleteNode::CreateDeleteNode(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
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

		return true;
	}

	bool
	CreateDeleteNode::shutdown(void)
	{
		Log(Debug, "CreateDeleteNode::shutdown");

		return true;
	}

}
