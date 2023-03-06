/*
   Copyright 2015-2022 Kai Huebl (kai@huebl-sgh.de)

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

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "ASNeG-Demo/Library/TestFolderLib.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNodeReference.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNamespaceInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/RegisterForwardNode.h"
#include <iostream>

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	TestFolderLib::TestFolderLib(void)
	: namespaceIndex_(0)
	, loopTime_(0)
	, valueMap_()
	, valueVec_()
	, ioThread_()
	, slotTimerElement_()
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	{
		Log(Debug, "TestFolderLib::TestFolderLib");
	}

	TestFolderLib::~TestFolderLib(void)
	{
		Log(Debug, "TestFolderLib::~TestFolderLib");
	}

	bool
	TestFolderLib::startup(
		const OpcUaStackCore::IOThread::SPtr& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "TestFolderLib::startup");

		ioThread_ = ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace info from server service
		if (!getNamespaceInfo()) {
			return false;
		}

		// create value map
		if (!createValueMap()) {
			return false;
		}

		// register read and write callbacks
		if (!registerCallbacks()) {
			return false;
		}

		// register loop time callbacks
		if (!registerLoopTimeCallbacks()) {
			return false;
		}

		// create node references
		if (!createNodeReferences()) {
			return false;
		}

		startTimerLoop();

		return true;
	}

	bool
	TestFolderLib::shutdown(void)
	{
		Log(Debug, "TestFolderLib::shutdown");

		ioThread_->slotTimer()->stop(slotTimerElement_);
		slotTimerElement_.reset();
		return true;
	}

	bool
	TestFolderLib::getNamespaceInfo(void)
	{
		GetNamespaceInfo getNamespaceInfo;

		if (!getNamespaceInfo.query(applicationServiceIf_)) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		namespaceIndex_ = getNamespaceInfo.getNamespaceIndex("http://ASNeG-Demo.de/Test-Server-Lib/");
		if (namespaceIndex_ == -1) {
			std::cout << "namespace index not found: http://ASNeG-Demo.de/Test-Server-Lib/" << std::endl;
			return false;
		}

		return true;
	}

	bool
	TestFolderLib::createValueMap(void)
	{
		//
		// The nodes identifier and their variables are stored in a map. Each variable
		// in the map is assigned an inital variable.
		//

		OpcUaDateTime now(boost::posix_time::microsec_clock::universal_time());

		// SByte
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(200, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaSByte)11, Success, now)
		));

		// SByteByteArray
		std::vector<OpcUaSByte> sByteVec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(201, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(sByteVec, Success, now)
		));

		// Byte
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(202, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaByte)32, Success, now)
		));

		// ByteByteArray
		std::vector<OpcUaByte> byteVec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(203, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(byteVec, Success, now)
		));

		// Int16
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(204, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaInt16)25, Success, now)
		));

		// Int16Array
		std::vector<OpcUaInt16> int16Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(205, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(int16Vec, Success, now)
		));

		// UInt16
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(206, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaUInt16)50, Success, now)
		));

		// UInt16Array
		std::vector<OpcUaUInt16> uint16Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(207, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(uint16Vec, Success, now)
		));

		// Int32
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(208, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaInt32)180, Success, now)
		));

		// Int32Array
		std::vector<OpcUaInt32> int32Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(209, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(int32Vec, Success, now)
		));

		// UInt32
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(210, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaUInt32)180, Success, now)
		));

		// UInt32Array
		std::vector<OpcUaUInt32> uint32Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(211, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(uint32Vec, Success, now)
		));

		// Int64
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(212, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaInt64)0, Success, now)
		));

		// Int64Array
		std::vector<OpcUaInt64> int64Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(213, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(int64Vec, Success, now)
		));

		// UInt64
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(214, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaUInt64)1500, Success, now)
		));

		// UInt64Array
		std::vector<OpcUaUInt64> uint64Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(215, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(uint64Vec, Success, now)
		));

		// Float
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(216, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaFloat)500, Success, now)
		));

		// FloatArray
		std::vector<OpcUaFloat> floatVec = {0.0, 6.7, 10.1};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(217, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(floatVec, Success, now)
		));

		// Double
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(218, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaDouble)500, Success, now)
		));

		// DoubleArray
		std::vector<OpcUaDouble> doubleVec = {0.0, 6.7, 10.1};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(219, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(doubleVec, Success, now)
		));

		// Boolean
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(220, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>((OpcUaBoolean)true, Success, now)
		));

		// DoubleArray
		std::vector<OpcUaBoolean> booleanVec = {false, false, false};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(221, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(booleanVec, Success, now)
		));

		// String
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(222, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(OpcUaString("TestString"), Success, now)
		));

		// StringArray
		std::vector<OpcUaString::SPtr> stringVec = {
			boost::make_shared<OpcUaString>("TestString1"),
			boost::make_shared<OpcUaString>("TestString2"),
			boost::make_shared<OpcUaString>("TestString3")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(223, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(stringVec, Success, now)
		));

		// ByteString
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(224, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(OpcUaByteString("TestByteString"), Success, now)
		));

		// ByteStringArray
		std::vector<OpcUaByteString::SPtr> byteStringVec = {
			boost::make_shared<OpcUaByteString>("TestByteString1"),
			boost::make_shared<OpcUaByteString>("TestByteString2"),
			boost::make_shared<OpcUaByteString>("TestByteString3")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(225, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(byteStringVec, Success, now)
		));

		// LocalizedText
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(226, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(OpcUaLocalizedText("de", "Text"), Success, now)
		));

		// LocalizedTextArray
		std::vector<OpcUaLocalizedText::SPtr> localizedTextVec = {
			boost::make_shared<OpcUaLocalizedText>("de", "Test1"),
			boost::make_shared<OpcUaLocalizedText>("de", "Test2"),
			boost::make_shared<OpcUaLocalizedText>("de", "Test3")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(227, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(localizedTextVec, Success, now)
		));

		// DateTime
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(228, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(now, Success, now)
		));

		// DateTimeArray
		std::vector<OpcUaDateTime> dateTimeVec = { now, now, now };
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(229, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(dateTimeVec, Success, now)
		));

		// GUID
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(230, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(OpcUaGuid("12345678-9ABC-DEF0-1234-56789ABCDEF0"), Success, now)
		));

		// GuidArray
		std::vector<OpcUaGuid::SPtr> guidVec = {
			boost::make_shared<OpcUaGuid>("12345678-9ABC-DEF0-1234-56789ABCDEF0"),
			boost::make_shared<OpcUaGuid>("12345678-9ABC-DEF0-1234-56789ABCDEF0"),
			boost::make_shared<OpcUaGuid>("12345678-9ABC-DEF0-1234-56789ABCDEF0")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(231, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(guidVec, Success, now)
		));

		// NodeId
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(232, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(OpcUaNodeId(1,1), Success, now)
		));

		// nodeIdArray
		std::vector<OpcUaNodeId::SPtr> nodeIdVec = {
			boost::make_shared<OpcUaNodeId>(1,1),
			boost::make_shared<OpcUaNodeId>(2,1),
			boost::make_shared<OpcUaNodeId>(3,1)
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(233, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(nodeIdVec, Success, now)
		));

		// QualifiedName
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(234, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(OpcUaQualifiedName("Name", 1), Success, now)
		));

		// QualifiedNameArray
		std::vector<OpcUaQualifiedName::SPtr> qualifiedNameVec = {
			boost::make_shared<OpcUaQualifiedName>("Name1", 1),
			boost::make_shared<OpcUaQualifiedName>("Name2", 1),
			boost::make_shared<OpcUaQualifiedName>("Name3", 1)
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(235, namespaceIndex_),
			boost::make_shared<OpcUaDataValue>(qualifiedNameVec, Success, now)
		));

		// create vector of node identifier
		for (auto it : valueMap_) valueVec_.push_back(it.first);

		return true;
	}

	bool
	TestFolderLib::registerCallbacks(void)
	{
		RegisterForwardNode registerForwardNode(valueVec_);
		registerForwardNode.setReadCallback(boost::bind(&TestFolderLib::readValue, this, boost::placeholders::_1));
		registerForwardNode.setWriteCallback(boost::bind(&TestFolderLib::writeValue, this, boost::placeholders::_1));
		if (!registerForwardNode.query(applicationServiceIf_, true)) {
			std::cout << "registerForwardNode response error" << std::endl;
			return false;
		}
	    return true;
	}

	bool
	TestFolderLib::registerLoopTimeCallbacks(void)
	{
		RegisterForwardNode registerForwardNode(OpcUaNodeId(3, namespaceIndex_));
		registerForwardNode.setReadCallback(boost::bind(&TestFolderLib::readLoopTimeValue, this, boost::placeholders::_1));
		registerForwardNode.setWriteCallback(boost::bind(&TestFolderLib::writeLoopTimeValue, this, boost::placeholders::_1));
		if (!registerForwardNode.query(applicationServiceIf_, true)) {
			std::cout << "registerForwardNode response error" << std::endl;
			return false;
		}
	    return true;
	}

	bool
	TestFolderLib::createNodeReferences(void)
	{
		// read node references
		GetNodeReference getNodeReference(valueVec_);
		if (!getNodeReference.query(applicationServiceIf_)) {
	  		std::cout << "response error" << std::endl;
	  		return false;
		}

		// check and save node references
		for (uint32_t idx = 0; idx < getNodeReference.nodeReferences().size(); idx++) {
			if (getNodeReference.statuses()[idx] != Success) {
	  			std::cout << "node reference error" << std::endl;
	  			return false;
			}

			BaseNodeClass::WPtr baseNodeClass = getNodeReference.nodeReferences()[idx];
			OpcUaNodeId nodeId = valueVec_[idx];
			baseNodeClassWMap_.insert(std::make_pair(nodeId, baseNodeClass));
		}

		return true;
	}

	void
	TestFolderLib::readValue(ApplicationReadContext* applicationReadContext)
	{
		//std::cout << "read value ..." << applicationReadContext->nodeId_ << std::endl;

	    auto it = valueMap_.find(applicationReadContext->nodeId_);
	    if (it == valueMap_.end()) {
	    	applicationReadContext->statusCode_ = BadInternalError;
	    	return;
	    }
	    applicationReadContext->statusCode_ = Success;
	    it->second->copyTo(applicationReadContext->dataValue_);
	}

	void
	TestFolderLib::readLoopTimeValue(ApplicationReadContext* applicationReadContext)
	{
		applicationReadContext->statusCode_ = Success;
		applicationReadContext->dataValue_ = OpcUaDataValue(loopTime_);
	}

	void
	TestFolderLib::writeValue(ApplicationWriteContext* applicationWriteContext)
	{
		//std::cout << "write value ..." << applicationWriteContext->nodeId_  << std::endl;

		auto it = valueMap_.find(applicationWriteContext->nodeId_);
		if (it == valueMap_.end()) {
			applicationWriteContext->statusCode_ = BadInternalError;
			return;
		}
		applicationWriteContext->statusCode_ = Success;
		applicationWriteContext->dataValue_.copyTo(*it->second);
	}

	void
	TestFolderLib::writeLoopTimeValue(ApplicationWriteContext* applicationWriteContext)
	{
		//std::cout << "write loop time value ..." << applicationWriteContext->nodeId_  << std::endl;

		uint32_t timerInterval_ = applicationWriteContext->dataValue_.variant()->variant<OpcUaUInt32>();

		if (timerInterval_ < 100) {
			timerInterval_ = 100;
			applicationWriteContext->dataValue_.variant()->variant(timerInterval_);
		}

		applicationWriteContext->statusCode_ = Success;
		applicationWriteContext->dataValue_.getValue(loopTime_);

		if (slotTimerElement_.get() != nullptr) {
			ioThread_->slotTimer()->stop(slotTimerElement_);
			slotTimerElement_.reset();
		}
		if (timerInterval_ == 0) return;

		slotTimerElement_ = boost::make_shared<SlotTimerElement>();
		slotTimerElement_->timeoutCallback(boost::bind(&TestFolderLib::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), timerInterval_);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	TestFolderLib::startTimerLoop(void)
	{
		// TimerInterval
		loopTime_ = 1111;

		slotTimerElement_ = boost::make_shared<SlotTimerElement>();
		slotTimerElement_->timeoutCallback(boost::bind(&TestFolderLib::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 1111);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	TestFolderLib::timerLoop(void)
	{
	  	for (auto it1 : valueMap_) {

	  		auto nodeId = it1.first;
	  		auto dataValue = it1.second;

	  		// find pointer to node
	  		auto it2 = baseNodeClassWMap_.find(nodeId);
	  		if (it2 == baseNodeClassWMap_.end()) {
	  			std::cout << "baseNodeClass not exist: " << nodeId << std::endl;
	  			continue;
	  		}

	  		// check if node already exist
	  		auto baseNodeClass = it2->second.lock();
	  		if (baseNodeClass.get() == nullptr) {
	  			std::cout << "baseNodeClass is deleted: " << nodeId << std::endl;
	  			continue;
	  		}

	  		// update node
	  		if (dataValue->variant()->isArray()) {
	  			updateArray(nodeId, dataValue, baseNodeClass);
	  		}
	  		else {
	  			updateSingle(nodeId, dataValue, baseNodeClass);
	  		}
	  	}
	}

	int32_t
	TestFolderLib::mySign(void)
	{
		if ((rand() % 2) == 1) return -1;
		return 1;
	}

	double
	TestFolderLib::myPercent(void)
	{
		return ((rand() % 5) / 100.0);
	}

	void
	TestFolderLib::updateSingle(const OpcUaNodeId& nodeId, const OpcUaDataValue::SPtr dataValue, const BaseNodeClass::SPtr baseNodeClass)
	{
		switch (dataValue->variant()->variantType())
		{
			case OpcUaBuildInType_OpcUaSByte:
			{
				OpcUaSByte value;
				dataValue->getValue(value);
				value++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaByte: // 0 - 64
			{
				OpcUaByte value;
				dataValue->getValue(value);
				value += (64 * myPercent() * mySign());
			    if (value < 0) value = 0;
			    if (value > 64) value = 64;
			    dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaInt16: // -50 - 100
			{
				OpcUaInt16 value;
				dataValue->getValue(value);
				value += (150 * myPercent() * mySign());
				if (value < -50) value = -50;
				if (value > 100) value = 100;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt16: // 0 - 100
			{
				OpcUaUInt16 value;
				dataValue->getValue(value);
				value += (100 * myPercent() * mySign());
				if (value < 0) value = 0;
				if (value > 100) value = 100;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaInt32: // 0 - 360
			{
				OpcUaInt32 value;
				dataValue->getValue(value);
				value += (360 * myPercent() * mySign());
				if (value < 0) value = 0;
				if (value > 360) value = 360;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt32: // 0 - 360
			{
				OpcUaUInt32 value;
				dataValue->getValue(value);
				value += (360 * myPercent() * mySign());
				if (value < 0) value = 0;
				if (value > 360) value = 360;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaInt64: // -1000 - 1000
			{
				OpcUaInt64 value;
				dataValue->getValue(value);
				value += (2000 * myPercent() * mySign());
				if (value < -1000) value = 1000;
				if (value > 1000) value = 1000;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt64: // 0 -3000
			{
				OpcUaUInt64 value;
				dataValue->getValue(value);
				value += (3000 * myPercent() * mySign());
				if (value < 0) value = 0;
				if (value > 3000) value = 3000;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaFloat: // 0 - 1000
			{
				OpcUaFloat value;
				dataValue->getValue(value);
				value += (1000 * myPercent() * mySign());
				if (value < 0) value = 0;
				if (value > 1000) value = 1000;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaDouble: // 0 - 1000
			{
				OpcUaDouble value;
				dataValue->getValue(value);
				value += (1000 * myPercent() * mySign());
				if (value < 0) value = 0;
				if (value > 1000) value = 1000;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaBoolean:
			{
				OpcUaBoolean value;
				dataValue->getValue(value);
				if (value) value = false;
				else value = true;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaString:
			{
				std::stringstream ss;
				ss << "String" << rand();
				OpcUaString value(ss.str());
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaByteString:
			{
				std::stringstream ss;
				ss << "String" << rand();
				OpcUaByteString value(ss.str());
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaLocalizedText:
			{
				std::stringstream ss;
				ss << "String" << rand();
				OpcUaLocalizedText value("de", ss.str());
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaDateTime:
			{
				OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
				dataValue->setValue(dateTime);
				break;
			}
			case OpcUaBuildInType_OpcUaGuid:
			{
				OpcUaGuid value;
				dataValue->getValue(value);
				value.data1(value.data1()+1);
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaNodeId:
			{
				OpcUaNodeId value(rand(),1);
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaQualifiedName:
			{
				OpcUaQualifiedName value("Name", rand());
				dataValue->setValue(value);
				break;
			}
			default:
			{
				std::cout << "data type unknown in update single" << std::endl;
				return;
			}
		}

		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		dataValue->serverTimestamp(dateTime);
		dataValue->sourceTimestamp(dateTime);
		dataValue->statusCode(Success);
		baseNodeClass->setValueSync(*dataValue);

		// set some other attributes
		baseNodeClass->setAccessLevelExSync(0xFF);
		baseNodeClass->setAccessRestrictions(0x00);

		RolePermissionType::SPtr rolePermission;
		RolePermissionTypeArray rolePermissions;
		rolePermissions.resize(3);
		for (uint32_t idx = 0; idx < 3; idx++) {
			rolePermission = boost::make_shared<RolePermissionType>();
			rolePermission->roleId().set(idx);
			rolePermission->permissions() = idx;
			rolePermissions.push_back(rolePermission);
		}
		baseNodeClass->setRolePermissions(rolePermissions);
	}

	void
	TestFolderLib::updateArray(const OpcUaNodeId& nodeId, const OpcUaDataValue::SPtr dataValue, const BaseNodeClass::SPtr baseNodeClass)
	{
		switch (dataValue->variant()->variantType())
		{
			case OpcUaBuildInType_OpcUaSByte:
			{
				std::vector<OpcUaSByte> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaByte:
			{
				std::vector<OpcUaByte> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaInt16:
			{
				std::vector<OpcUaInt16> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt16:
			{
				std::vector<OpcUaUInt16> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaInt32:
			{
				std::vector<OpcUaInt32> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt32:
			{
				std::vector<OpcUaUInt32> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaInt64:
			{
				std::vector<OpcUaInt64> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt64:
			{
				std::vector<OpcUaUInt64> value;
				dataValue->getValue(value);
				for (auto& v : value) v++;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaFloat:
			{
				std::vector<OpcUaFloat> value;
				dataValue->getValue(value);
				for (auto& v : value) v += 3.354;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaDouble:
			{
				std::vector<OpcUaDouble> value;
				dataValue->getValue(value);
				for (auto& v : value) v += 33.354;
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaBoolean:
			{
				std::vector<OpcUaBoolean> value;
				dataValue->getValue(value);
				for (uint32_t idx=0; idx<value.size(); idx++) value[idx] = !value[idx];
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaString:
			{
				std::vector<OpcUaString::SPtr> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					std::stringstream ss;
					ss << "String" << rand();
					*v = ss.str();
				}
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaByteString:
			{
				std::vector<OpcUaByteString::SPtr> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					std::stringstream ss;
					ss << "String" << rand();
					*v = ss.str();
				}
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaLocalizedText:
			{
				std::vector<OpcUaLocalizedText::SPtr> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					std::stringstream ss;
					ss << "String" << rand();
					v->set("de", ss.str());
				}
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaDateTime:
			{
				std::vector<OpcUaDateTime> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					v.dateTime(boost::posix_time::microsec_clock::universal_time());
				}
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaGuid:
			{
				std::vector<OpcUaGuid::SPtr> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					v->data1(v->data1()+1);
				}
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaNodeId:
			{
				std::vector<OpcUaNodeId::SPtr> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					v->set(rand(), 1);
				}
				dataValue->setValue(value);
				break;
			}
			case OpcUaBuildInType_OpcUaQualifiedName:
			{
				std::vector<OpcUaQualifiedName::SPtr> value;
				dataValue->getValue(value);
				for (auto& v : value) {
					v->set("Name", rand());
				}
				dataValue->setValue(value);
				break;
			}
			default:
			{
				std::cout << "data type unknown in update array" << std::endl;
				return;
			}
		}


		OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
		dataValue->serverTimestamp(dateTime);
		dataValue->sourceTimestamp(dateTime);
		dataValue->statusCode(Success);
		baseNodeClass->setValueSync(*dataValue);

		// set some other attributes
		baseNodeClass->setAccessLevelExSync(0xFF);
		baseNodeClass->setAccessRestrictions(0x00);

		RolePermissionType::SPtr rolePermission;
		RolePermissionTypeArray rolePermissions;
		rolePermissions.resize(3);
		for (uint32_t idx = 0; idx < 3; idx++) {
			rolePermission = boost::make_shared<RolePermissionType>();
			rolePermission->roleId().set(idx);
			rolePermission->permissions() = idx;
			rolePermissions.push_back(rolePermission);
		}
		baseNodeClass->setRolePermissions(rolePermissions);
	}

}

