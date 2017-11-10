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

#ifndef __OpcUaServerApplicationDemo_Alarm_h__
#define __OpcUaServerApplicationDemo_Alarm_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	class Alarm
	{
	  public:
		Alarm(void);
		~Alarm(void);

		bool startup(IOThread& ioThread, ApplicationServiceIf& applicationServiceIf, ApplicationInfo* applicationInfo);
		bool shutdown(void);

		void ackedState(const OpcUaLocalizedText& ackedState);
		OpcUaLocalizedText ackedState(void);
		void ackedState_Id(bool ackedState);
		bool ackedState_Id(void);
		void confirmedState(const OpcUaLocalizedText& confirmedState);
		OpcUaLocalizedText confirmedState(void);
		void confirmedState_Id(bool confirmedState);
		bool confirmedState_Id(void);
		void activeState(const OpcUaLocalizedText& activeState);
		OpcUaLocalizedText activeState(void);
		void activeState_Id(bool activeState);
		bool activeState_Id(void);
		void enabledState(const OpcUaLocalizedText& activeState);
		OpcUaLocalizedText enabledState(void);
		void enabledState_Id(bool activeState);
		bool enabledState_Id(void);
		void comment(const OpcUaLocalizedText& comment);
		OpcUaLocalizedText comment(void);

	  private:
		bool getNamespaceInfo(void);
		bool getNodeIds(void);
		bool getNodeIdFromResponse(BrowsePathToNodeIdResponse::SPtr& res, uint32_t idx, OpcUaNodeId::SPtr& nodeId);
		bool createNodeReferences(void);
		bool getRefFromResponse(GetNodeReferenceResponse::SPtr& res, uint32_t idx, BaseNodeClass::WPtr& ref);
		bool registerCallbacks(void);
		bool registerCallback(OpcUaNodeId& objectNodeId, OpcUaNodeId& methodNodeId, Callback* callback);
		void acknowledge(ApplicationMethodContext* applicationMethodContext);
		void confirm(ApplicationMethodContext* applicationMethodContext);
		void enabled(ApplicationMethodContext* applicationMethodContext);
		void disable(ApplicationMethodContext* applicationMethodContext);
		void conditionRefresh(ApplicationMethodContext* applicationMethodContext);
		void startTimerLoop(void);
		void timerLoop(void);
		void sendAlarmEvent(const std::string& eventMessage);

		IOThread* ioThread_;
		SlotTimerElement::SPtr slotTimerElement_;
		ApplicationServiceIf* applicationServiceIf_;
		ApplicationInfo* applicationInfo_;

		uint32_t namespaceIndex_;

		//
		// node ids
		//
		OpcUaNodeId::SPtr rootNodeId_;
		OpcUaNodeId::SPtr ackedStateNodeId_;
		OpcUaNodeId::SPtr ackedStateIdNodeId_;
		OpcUaNodeId::SPtr confirmedStateNodeId_;
		OpcUaNodeId::SPtr confirmedStateIdNodeId_;
		OpcUaNodeId::SPtr activeStateNodeId_;
		OpcUaNodeId::SPtr activeStateIdNodeId_;
		OpcUaNodeId::SPtr enabledStateNodeId_;
		OpcUaNodeId::SPtr enabledStateIdNodeId_;
		OpcUaNodeId::SPtr commentNodeId_;
		OpcUaNodeId::SPtr commentSourceTimestampNodeId_;

		OpcUaNodeId::SPtr acknowlegeNodeId_;
		OpcUaNodeId::SPtr confirmNodeId_;
		OpcUaNodeId::SPtr addCommentNodeId_;
		OpcUaNodeId::SPtr enabledNodeId_;
		OpcUaNodeId::SPtr disableNodeId_;

		//
		// base node class references
		//
		BaseNodeClass::WPtr ackedState_;
		BaseNodeClass::WPtr ackedStateId_;
		BaseNodeClass::WPtr confirmedState_;
		BaseNodeClass::WPtr confirmedStateId_;
		BaseNodeClass::WPtr activeState_;
		BaseNodeClass::WPtr activeStateId_;
		BaseNodeClass::WPtr enabledState_;
		BaseNodeClass::WPtr enabledStateId_;
		BaseNodeClass::WPtr comment_;
		BaseNodeClass::WPtr commentSourceTimestamp_;

		//
		// alarm callbacks
		//
		Callback acknowledgeCallback_;
		Callback confirmCallback_;
		Callback enabledCallback_;
		Callback disableCallback_;
		Callback conditionRefreshCallback_;
	};

}

#endif
