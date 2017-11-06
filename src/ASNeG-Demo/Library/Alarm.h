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

	  private:
		bool getNamespaceInfo(void);
		bool getNodeIds(void);
		bool getNodeIdFromResponse(BrowsePathToNodeIdResponse::SPtr& res, uint32_t idx, OpcUaNodeId::SPtr& nodeId);
		bool createNodeReferences(void);
		bool getRefFromResponse(GetNodeReferenceResponse::SPtr& res, uint32_t idx, BaseNodeClass::WPtr& ref);

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
		OpcUaNodeId::SPtr activeStateNodeId_;
		OpcUaNodeId::SPtr activeStateIdNodeId_;
		OpcUaNodeId::SPtr enableStateNodeId_;
		OpcUaNodeId::SPtr enableStateIdNodeId_;
		OpcUaNodeId::SPtr commentNodeId_;
		OpcUaNodeId::SPtr commentSourceTimestampNodeId_;

		OpcUaNodeId::SPtr acknowlegeNodeId_;
		OpcUaNodeId::SPtr confirmNodeId_;
		OpcUaNodeId::SPtr addCommentNodeId_;
		OpcUaNodeId::SPtr enableNodeId_;
		OpcUaNodeId::SPtr disableNodeId_;

		//
		// base node class references
		//
		BaseNodeClass::WPtr ackedState_;
		BaseNodeClass::WPtr ackedStateId_;
		BaseNodeClass::WPtr activeState_;
		BaseNodeClass::WPtr activeStateId_;
		BaseNodeClass::WPtr enableState_;
		BaseNodeClass::WPtr enableStateId_;
		BaseNodeClass::WPtr comment_;
		BaseNodeClass::WPtr commentSourceTimestamp_;
	};

}

#endif
