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

#ifndef __OpcUaServerApplicationDemo_StateVariableType_h__
#define __OpcUaServerApplicationDemo_StateVariableType_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/VariableType/ServerVariables.h"
#include "ASNeG-Demo/Generator/BaseDataVariableType.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	class StateVariableType
	: public BaseDataVariableType
	{
	  public:
		StateVariableType(void);
		~StateVariableType(void);

		virtual bool linkInstanceWithModel(const OpcUaNodeId& nodeId);

		BaseNodeClass::SPtr effectiveDisplayName(void);
		bool setEffectiveDisplayName(const OpcUaDataValue& dataValue);
		bool getEffectiveDisplayName(OpcUaDataValue& dataValue);

		BaseNodeClass::SPtr id(void);
		bool setId(const OpcUaDataValue& dataValue);
		bool getId(OpcUaDataValue& dataValue);

		BaseNodeClass::SPtr name(void);
		bool setName(const OpcUaDataValue& dataValue);
		bool getName(OpcUaDataValue& dataValue);

		BaseNodeClass::SPtr number(void);
		bool setNumber(const OpcUaDataValue& dataValue);
		bool getNumber(OpcUaDataValue& dataValue);

	  private:
		std::string namespaceName_;
		uint16_t namespaceIndex_;
		ServerVariable effectiveDisplayName_;
		ServerVariable id_;
		ServerVariable name_;
		ServerVariable number_;

	};

}

#endif
