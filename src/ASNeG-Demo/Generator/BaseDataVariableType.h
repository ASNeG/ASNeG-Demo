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

#ifndef __OpcUaServerApplicationDemo_BaseDataVariableType_h__
#define __OpcUaServerApplicationDemo_BaseDataVariableType_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackServer/StandardVariableType/BaseVariableType.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	class BaseDataVariableType
	: public BaseVariableType
	{
	  public:
		BaseDataVariableType(void);
		virtual ~BaseDataVariableType(void);

		virtual bool linkInstanceWithModel(const OpcUaNodeId& nodeId);

		BaseNodeClass::SPtr value(void);
		bool setValue(const OpcUaDataValue& dataValue);
		bool getValue(OpcUaDataValue& dataValue);
		void setUpdateCallbackValue(Callback::SPtr& callback);

	  private:
		uint16_t namespaceIndex_;
		std::string namespaceName_;
		ServerVariable::SPtr value_;
	};

}

#endif
