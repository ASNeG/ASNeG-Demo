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

#ifndef __OpcUaServerApplicationDemo_Alarm_h__
#define __OpcUaServerApplicationDemo_Alarm_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"

namespace OpcUaServerApplicationDemo
{

	class Alarm
	{
	  public:
		Alarm(void);
		~Alarm(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

		void ackedState(
			const OpcUaStackCore::OpcUaLocalizedText& ackedState
		);
		OpcUaStackCore::OpcUaLocalizedText ackedState(void);
		void ackedState_Id(bool ackedState);
		bool ackedState_Id(void);
		void confirmedState(
			const OpcUaStackCore::OpcUaLocalizedText& confirmedState
		);
		OpcUaStackCore::OpcUaLocalizedText confirmedState(void);
		void confirmedState_Id(bool confirmedState);
		bool confirmedState_Id(void);
		void activeState(
			const OpcUaStackCore::OpcUaLocalizedText& activeState
		);
		OpcUaStackCore::OpcUaLocalizedText activeState(void);
		void activeState_Id(bool activeState);
		bool activeState_Id(void);
		void enabledState(
			const OpcUaStackCore::OpcUaLocalizedText& activeState
		);
		OpcUaStackCore::OpcUaLocalizedText enabledState(void);
		void enabledState_Id(bool activeState);
		bool enabledState_Id(void);
		void comment(
			const OpcUaStackCore::OpcUaLocalizedText& comment
		);
		OpcUaStackCore::OpcUaLocalizedText comment(void);

	  private:
		bool getNamespaceInfo(void);
		bool getNodeIds(void);
		bool createNodeReferences(void);
		bool registerCallbacks(void);
		bool registerCallback(
			const OpcUaStackCore::OpcUaNodeId& objectNodeId,
			OpcUaStackCore::OpcUaNodeId& methodNodeId,
			OpcUaStackCore::ApplicationCallback::Method callback
		);
		void acknowledge(
			OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
		void confirm(
			OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
		void enabled(
			OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
		void disable(
			OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
		void conditionRefresh(
			OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
		void startTimerLoop(void);
		void timerLoop(void);
		void sendAlarmEvent(const std::string& eventMessage);

		OpcUaStackCore::IOThread::SPtr ioThread_;
		OpcUaStackCore::SlotTimerElement::SPtr slotTimerElement_;
		OpcUaStackServer::ApplicationServiceIf* applicationServiceIf_;
		OpcUaStackServer::ApplicationInfo* applicationInfo_;

		uint32_t namespaceIndex_;

		//
		// node ids
		//
		OpcUaStackCore::OpcUaNodeId rootNodeId_;
		OpcUaStackCore::OpcUaNodeId ackedStateNodeId_;
		OpcUaStackCore::OpcUaNodeId ackedStateIdNodeId_;
		OpcUaStackCore::OpcUaNodeId confirmedStateNodeId_;
		OpcUaStackCore::OpcUaNodeId confirmedStateIdNodeId_;
		OpcUaStackCore::OpcUaNodeId activeStateNodeId_;
		OpcUaStackCore::OpcUaNodeId activeStateIdNodeId_;
		OpcUaStackCore::OpcUaNodeId enabledStateNodeId_;
		OpcUaStackCore::OpcUaNodeId enabledStateIdNodeId_;
		OpcUaStackCore::OpcUaNodeId commentNodeId_;
		OpcUaStackCore::OpcUaNodeId commentSourceTimestampNodeId_;

		OpcUaStackCore::OpcUaNodeId acknowlegeNodeId_;
		OpcUaStackCore::OpcUaNodeId confirmNodeId_;
		OpcUaStackCore::OpcUaNodeId addCommentNodeId_;
		OpcUaStackCore::OpcUaNodeId enabledNodeId_;
		OpcUaStackCore::OpcUaNodeId disableNodeId_;

		//
		// base node class references
		//
		OpcUaStackServer::BaseNodeClass::WPtr ackedState_;
		OpcUaStackServer::BaseNodeClass::WPtr ackedStateId_;
		OpcUaStackServer::BaseNodeClass::WPtr confirmedState_;
		OpcUaStackServer::BaseNodeClass::WPtr confirmedStateId_;
		OpcUaStackServer::BaseNodeClass::WPtr activeState_;
		OpcUaStackServer::BaseNodeClass::WPtr activeStateId_;
		OpcUaStackServer::BaseNodeClass::WPtr enabledState_;
		OpcUaStackServer::BaseNodeClass::WPtr enabledStateId_;
		OpcUaStackServer::BaseNodeClass::WPtr comment_;
		OpcUaStackServer::BaseNodeClass::WPtr commentSourceTimestamp_;

	};

}

#endif
