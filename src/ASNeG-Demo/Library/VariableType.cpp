/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

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
#include "ASNeG-Demo/Library/VariableType.h"
#include "OpcUaStackServer/ServiceSetApplication/CreateVariableInstance.h"

namespace OpcUaServerApplicationDemo
{

	VariableType::VariableType(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)

	, analogItemType_(constructSPtr<AnalogItemType>())
	{
	}

	VariableType::~VariableType(void)
	{
	}

	bool
	VariableType::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "VariableType::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// crerate variable
		if (!createVariable()) {
			Log(Error, "create variable error");
			return false;
		}

		return true;
	}

	bool
	VariableType::shutdown(void)
	{
		Log(Debug, "VariableType::shutdown");

		return true;
	}

	bool
	VariableType::createVariable(void)
	{
		Object::SPtr obj = analogItemType_;
		CreateVariableInstance createVariableInstance(
			OpcUaNodeId(),
			OpcUaNodeId(),
			obj
		);

		if (!createVariableInstance.query(applicationServiceIf_)) {
			Log(Error, "create variable response error");
			return false;
		}

		return true;
	}

#if 0
	const OpcUaNodeId& parentNodeId,
	const OpcUaNodeId& referenceTypeNodeId,
	Object::SPtr& variableInstance
#endif

}
