/*
   Copyright 2017-2020 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaServerApplicationDemo_Event_h__
#define __OpcUaServerApplicationDemo_Event_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackCore/Application/ApplicationEventItemStartContext.h"
#include "OpcUaStackCore/Application/ApplicationEventItemStopContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"

namespace OpcUaServerApplicationDemo
{

	class Event
	{
	  public:
		Event(void);
		~Event(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

	  private:
		void eventItemStartCallback(
			OpcUaStackCore::ApplicationEventItemStartContext* applicationEventItemStartContext
		);
		void eventItemStopCallback(
			OpcUaStackCore::ApplicationEventItemStopContext* applicationEventItemStopContext
		);

		bool registerEventCallbacks(void);
		bool getNamespaceInfo(void);
		void startTimerLoop(void);
		void timerLoop(void);

		void sendEvent11(void);
		void sendEvent12(void);
		void sendEvent21(void);

		OpcUaStackCore::IOThread::SPtr ioThread_;
		OpcUaStackCore::SlotTimerElement::SPtr slotTimerElement_;
		OpcUaStackServer::ApplicationServiceIf* applicationServiceIf_;
		OpcUaStackServer::ApplicationInfo* applicationInfo_;

		uint32_t namespaceIndex_;
		uint32_t counter_;
	};

}

#endif
