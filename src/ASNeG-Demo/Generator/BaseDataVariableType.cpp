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
#include "ASNeG-Demo/Generator/BaseDataVariableType.h"

using namespace OpcUaStackCore;

namespace OpcUaServerApplicationDemo
{

	BaseDataVariableType::BaseDataVariableType(void)
	: BaseVariableType()
	{
		variableType(OpcUaNodeId(63));
	}

	BaseDataVariableType::~BaseDataVariableType(void)
	{
	}

	BaseNodeClass::SPtr
	BaseDataVariableType::value(void)
	{
		return value_.lock();
	}

	bool
	BaseDataVariableType::setValue(const OpcUaDataValue& dataValue)
	{
		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		BaseNodeClass::SPtr baseNodeClass;

		baseNodeClass = value_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		baseNodeClass->setValueSync(*(const_cast<OpcUaDataValue*>(&dataValue)));
		return true;
	}

	bool
	BaseDataVariableType::getValue(OpcUaDataValue& dataValue)
	{
		OpcUaLocalizedText ackedState;
		BaseNodeClass::SPtr baseNodeClass;

		baseNodeClass = value_.lock();
		if (baseNodeClass.get() == nullptr) return false;

		baseNodeClass->getValueSync(dataValue);
		return true;
	}

}