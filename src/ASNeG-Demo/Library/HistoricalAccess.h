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

#ifndef __OpcUaServerApplicationDemo_HistoricalAccess_h__
#define __OpcUaServerApplicationDemo_HistoricalAccess_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationHReadContext.h"
#include "OpcUaStackCore/Application/ApplicationHReadEventContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{


	class HistoricalAccess
	{
	  public:
		HistoricalAccess(void);
		~HistoricalAccess(void);

		bool startup(IOThread& ioThread, ApplicationServiceIf& applicationServiceIf, ApplicationInfo* applicationInfo);
		bool shutdown(void);

	  private:
		bool getNamespaceInfo(void);
		bool registerHADataCallback(void);
		bool registerHAEventCallback(void);

		void readHValue(ApplicationHReadContext* applicationHReadContext);
		void readHEvent(ApplicationHReadEventContext* applicationHReadEventContext);

		IOThread* ioThread_;
		ApplicationServiceIf* applicationServiceIf_;
		ApplicationInfo* applicationInfo_;

		uint32_t namespaceIndex_;
	};

}

#endif
