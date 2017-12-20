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

#include "ASNeG-Demo/Generator/MyStateVariableType.h"

namespace OpcUaServerApplicationDemo
{

	MyStateVariableType::MyStateVariableType(void)
	: callbackUpdateValue_(constructSPtr<Callback>(boost::bind(&MyStateVariableType::updateValue, this, _1, _2)))
	{
		setUpdateCallbackValue(callbackUpdateValue_);
	}

	MyStateVariableType::~MyStateVariableType(void)
	{
	}

	void
	MyStateVariableType::updateValue(uint32_t attributeId, OpcUaDataValue* dataValue)
	{
		std::cout << "MyStateVariableType::updateValue: " << attributeId << " " << *dataValue << std::endl;
	}

}
