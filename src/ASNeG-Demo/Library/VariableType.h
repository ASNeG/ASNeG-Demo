/*
   Copyright 2019-2020 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaServerApplicationDemo_VariableType_h__
#define __OpcUaServerApplicationDemo_VariableType_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"
#include "OpcUaStackServer/StandardVariableType/AnalogItemType.h"

namespace OpcUaServerApplicationDemo
{

	class VariableType
	{
	  public:
		VariableType(void);
		~VariableType(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

	  private:
		bool createVariable(void);

		OpcUaStackCore::IOThread::SPtr ioThread_;
		OpcUaStackServer::ApplicationServiceIf* applicationServiceIf_;
		OpcUaStackServer::ApplicationInfo* applicationInfo_;

		OpcUaStackServer::AnalogItemType::SPtr analogItemType_;
	};

}

#endif
