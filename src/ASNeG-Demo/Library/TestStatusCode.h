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

#ifndef __OpcUaServerApplicationDemo_TestStatusCode_h__
#define __OpcUaServerApplicationDemo_TestStatusCode_h__

#include "OpcUaStackCore/Base/IOService.h"
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationReadContext.h"
#include "OpcUaStackCore/Application/ApplicationWriteContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"

namespace OpcUaServerApplicationDemo
{

	class TestStatusCode
	{
	  public:
		TestStatusCode(void);
		virtual ~TestStatusCode(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

	  private:
		int32_t mySign(void);
		double myPercent(void);
		bool getNamespaceInfo(void);
		void readValue(
			OpcUaStackCore::ApplicationReadContext* applicationReadContext
		);
		void readLoopTimeValue(
			OpcUaStackCore::ApplicationReadContext* applicationReadContext
		);
		void writeValue(
			OpcUaStackCore::ApplicationWriteContext* applicationWriteContext
		);
		void writeLoopTimeValue(
			OpcUaStackCore::ApplicationWriteContext* applicationWriteContext
		);
		void updateSingle(
			const OpcUaStackCore::OpcUaNodeId& nodeId,
			const OpcUaStackCore::OpcUaDataValue::SPtr dataValue,
			const OpcUaStackServer::BaseNodeClass::SPtr baseNodeClass
		);
		void updateArray(
			const OpcUaStackCore::OpcUaNodeId& nodeId,
			const OpcUaStackCore::OpcUaDataValue::SPtr dataValue,
			const OpcUaStackServer::BaseNodeClass::SPtr baseNodeClass
		);

		OpcUaStackCore::OpcUaDataValue::SPtr createDataValue(void);
		bool createValueMap(void);
		bool registerCallbacks(void);
		bool registerLoopTimeCallbacks(void);
		bool createNodeReferences(void);

		void startTimerLoop(void);
		void timerLoop(void);

		uint32_t namespaceIndex_;
		OpcUaStackCore::OpcUaDataValue::SPtr loopTime_;
		boost::posix_time::ptime switchTime_;
		OpcUaStackCore::OpcUaStatusCode statusCode_;

		using ValueMap = std::map<OpcUaStackCore::OpcUaNodeId, OpcUaStackCore::OpcUaDataValue::SPtr>;
		using BaseNodeClassWMap = std::map<OpcUaStackCore::OpcUaNodeId, OpcUaStackServer::BaseNodeClass::WPtr>;
		ValueMap valueMap_;
		BaseNodeClassWMap baseNodeClassWMap_;

		OpcUaStackCore::IOThread::SPtr ioThread_;
		OpcUaStackCore::SlotTimerElement::SPtr slotTimerElement_;
		OpcUaStackServer::ApplicationServiceIf* applicationServiceIf_;
		OpcUaStackServer::ApplicationInfo* applicationInfo_;
	};

}

#endif
