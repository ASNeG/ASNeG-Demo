/*
   Copyright 2016-2020 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaServerApplicationDemo_Discovery_h__
#define __OpcUaServerApplicationDemo_Discovery_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackCore/Application/ApplicationRegisterServerContext.h"
#include "OpcUaStackCore/Application/ApplicationFindServerContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"

namespace OpcUaServerApplicationDemo
{

	class Discovery
	{
	  public:
		Discovery(void);
		~Discovery(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

		void findServer(
			OpcUaStackCore::ApplicationFindServerContext* applicationFindServerContext
		);

	  private:
	};

}

#endif
