/*
   Copyright 2017 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackCore/BuildInTypes/BuildInTypes.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Generator/StateVariableType.h"

using namespace OpcUaStackCore;

namespace OpcUaServerApplicationDemo
{

	StateVariableType::StateVariableType(void)
	: BaseDataVariableType()
	{
		variableType(OpcUaNodeId(2755));
	}

	StateVariableType::~StateVariableType(void)
	{
	}

	BaseNodeClass::SPtr
	StateVariableType::effectiveDisplayName(void)
	{
		return effectiveDisplayName_.baseNode().lock();
	}

	bool
	StateVariableType::setEffectiveDisplayName(const OpcUaDataValue& dataValue)
	{
		return effectiveDisplayName_.setDataValue(dataValue);
	}

	bool
	StateVariableType::getEffectiveDisplayName(OpcUaDataValue& dataValue)
	{
		return effectiveDisplayName_.getDataValue(dataValue);
	}

	BaseNodeClass::SPtr
	StateVariableType::id(void)
	{
		return id_.baseNode().lock();
	}

	bool
	StateVariableType::setId(const OpcUaDataValue& dataValue)
	{
		return id_.setDataValue(dataValue);
	}

	bool
	StateVariableType::getId(OpcUaDataValue& dataValue)
	{
		return id_.getDataValue(dataValue);
	}

	BaseNodeClass::SPtr
	StateVariableType::name(void)
	{
		return name_.baseNode().lock();
	}

	bool
	StateVariableType::setName(const OpcUaDataValue& dataValue)
	{
		return name_.setDataValue(dataValue);
	}

	bool
	StateVariableType::getName(OpcUaDataValue& dataValue)
	{
		return name_.getDataValue(dataValue);
	}

	BaseNodeClass::SPtr
	StateVariableType::number(void)
	{
		return number_.baseNode().lock();
	}

	bool
	StateVariableType::setNumber(const OpcUaDataValue& dataValue)
	{
		return number_.setDataValue(dataValue);
	}

	bool
	StateVariableType::getNumber(OpcUaDataValue& dataValue)
	{
		return number_.getDataValue(dataValue);
	}

}
