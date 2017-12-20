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

#ifndef __OpcUaServerApplicationDemo_MyStateVariableType_h__
#define __OpcUaServerApplicationDemo_MyStateVariableType_h__

#include "ASNeG-Demo/Generator/StateVariableType.h"

namespace OpcUaServerApplicationDemo
{

	class MyStateVariableType
	: public StateVariableType
	{
	  public:
		MyStateVariableType(void);
		virtual ~MyStateVariableType(void);

		void updateValue(uint32_t attributeId, OpcUaDataValue* dataValue);

	  private:
		Callback::SPtr callbackUpdateValue_;
	};

}

#endif
