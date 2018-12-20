/*
   Copyright 2015-2017 Kai Huebl (kai@huebl-sgh.de)

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
#include <iostream>

namespace OpcUaServerApplicationDemo
{

	TestFolderLib::TestFolderLib(void)
	: namespaceIndex_(0)
	, loopTime_()
	, readCallback_(boost::bind(&TestFolderLib::readValue, this, _1))
	, readLoopTimeCallback_(boost::bind(&TestFolderLib::readLoopTimeValue, this, _1))
	, writeCallback_(boost::bind(&TestFolderLib::writeValue, this, _1))
	, writeLoopTimeCallback_(boost::bind(&TestFolderLib::writeLoopTimeValue, this, _1))
	, valueMap_()
	, valueVec_()
	, ioThread_()
	, slotTimerElement_()
	{
		Log(Debug, "TestFolderLib::TestFolderLib");

		loopTime_ = createDataValue();
		loopTime_->variant()->variant((uint32_t)0);
	}

	TestFolderLib::~TestFolderLib(void)
	{
		Log(Debug, "TestFolderLib::~TestFolderLib");
	}

	bool
	TestFolderLib::startup(IOThread& ioThread, ApplicationServiceIf& applicationServiceIf, ApplicationInfo* applicationInfo)
	{
		Log(Debug, "TestFolderLib::startup");

		ioThread_ = &ioThread;
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
		return true;
	}

	bool
	TestFolderLib::getNamespaceInfo(void)
	{
		ServiceTransactionNamespaceInfo::SPtr trx = constructSPtr<ServiceTransactionNamespaceInfo>();
		NamespaceInfoRequest::SPtr req = trx->request();
		NamespaceInfoResponse::SPtr res = trx->response();

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		NamespaceInfoResponse::Index2NamespaceMap::iterator it;
		for (
		    it = res->index2NamespaceMap().begin();
			it != res->index2NamespaceMap().end();
			it++
		)
		{
			if (it->second == "http://ASNeG-Demo.de/Test-Server-Lib/") {
				namespaceIndex_ = it->first;
			}
 		}

		return true;
	}

	OpcUaDataValue::SPtr
	TestFolderLib::createDataValue(void)
	{
		OpcUaDataValue::SPtr dataValue;
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->statusCode(Success);
		dataValue->sourceTimestamp(OpcUaDateTime(boost::posix_time::microsec_clock::universal_time()));
		dataValue->serverTimestamp(OpcUaDateTime(boost::posix_time::microsec_clock::universal_time()));
		return dataValue;
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
			constructSPtr<OpcUaDataValue>((OpcUaSByte)11, Success, now)
		));

		// SByteByteArray
		std::vector<OpcUaSByte> sByteVec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(201, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(sByteVec, Success, now)
		));

		// Byte
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(202, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaByte)32, Success, now)
		));

		// ByteByteArray
		std::vector<OpcUaByte> byteVec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(203, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(byteVec, Success, now)
		));

		// Int16
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(204, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaInt16)25, Success, now)
		));

		// Int16Array
		std::vector<OpcUaInt16> int16Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(205, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(int16Vec, Success, now)
		));

		// UInt16
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(206, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaUInt16)50, Success, now)
		));

		// UInt16Array
		std::vector<OpcUaUInt16> uint16Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(207, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(uint16Vec, Success, now)
		));

		// Int32
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(208, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaInt32)180, Success, now)
		));

		// Int32Array
		std::vector<OpcUaInt32> int32Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(209, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(int32Vec, Success, now)
		));

		// UInt32
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(210, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaUInt32)180, Success, now)
		));

		// UInt32Array
		std::vector<OpcUaUInt32> uint32Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(211, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(uint32Vec, Success, now)
		));

		// Int64
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(212, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaInt64)0, Success, now)
		));

		// Int64Array
		std::vector<OpcUaInt64> int64Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(213, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(int64Vec, Success, now)
		));

		// UInt64
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(214, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaUInt64)1500, Success, now)
		));

		// UInt64Array
		std::vector<OpcUaUInt64> uint64Vec = {0, 1, 2};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(215, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(uint64Vec, Success, now)
		));

		// Float
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(216, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaFloat)500, Success, now)
		));

		// FloatArray
		std::vector<OpcUaFloat> floatVec = {0.0, 6.7, 10.1};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(217, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(floatVec, Success, now)
		));

		// Double
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(218, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaDouble)500, Success, now)
		));

		// DoubleArray
		std::vector<OpcUaDouble> doubleVec = {0.0, 6.7, 10.1};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(219, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(doubleVec, Success, now)
		));

		// Boolean
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(220, namespaceIndex_),
			constructSPtr<OpcUaDataValue>((OpcUaBoolean)true, Success, now)
		));

		// DoubleArray
		std::vector<OpcUaBoolean> booleanVec = {false, false, false};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(221, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(booleanVec, Success, now)
		));

		// String
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(222, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(OpcUaString("TestString"), Success, now)
		));

		// StringArray
		std::vector<OpcUaString::SPtr> stringVec = {
			constructSPtr<OpcUaString>("TestString1"),
			constructSPtr<OpcUaString>("TestString2"),
			constructSPtr<OpcUaString>("TestString3")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(223, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(stringVec, Success, now)
		));

		// ByteString
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(224, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(OpcUaByteString("TestByteString"), Success, now)
		));

		// ByteStringArray
		std::vector<OpcUaByteString::SPtr> byteStringVec = {
			constructSPtr<OpcUaByteString>("TestByteString1"),
			constructSPtr<OpcUaByteString>("TestByteString2"),
			constructSPtr<OpcUaByteString>("TestByteString3")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(225, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(byteStringVec, Success, now)
		));

		// LocalizedText
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(226, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(OpcUaLocalizedText("de", "Text"), Success, now)
		));

		// LocalizedTextArray
		std::vector<OpcUaLocalizedText::SPtr> localizedTextVec = {
			constructSPtr<OpcUaLocalizedText>("de", "Test1"),
			constructSPtr<OpcUaLocalizedText>("de", "Test2"),
			constructSPtr<OpcUaLocalizedText>("de", "Test3")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(227, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(localizedTextVec, Success, now)
		));

		// DateTime
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(228, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(now, Success, now)
		));

		// DateTimeArray
		std::vector<OpcUaDateTime> dateTimeVec = { now, now, now };
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(229, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(dateTimeVec, Success, now)
		));

		// GUID
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(230, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(OpcUaGuid("12345678-9ABC-DEF0-1234-56789ABCDEF0"), Success, now)
		));

		// GuidArray
		std::vector<OpcUaGuid::SPtr> guidVec = {
			constructSPtr<OpcUaGuid>("12345678-9ABC-DEF0-1234-56789ABCDEF0"),
			constructSPtr<OpcUaGuid>("12345678-9ABC-DEF0-1234-56789ABCDEF0"),
			constructSPtr<OpcUaGuid>("12345678-9ABC-DEF0-1234-56789ABCDEF0")
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(231, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(guidVec, Success, now)
		));

		// NodeId
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(232, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(OpcUaNodeId(1,1), Success, now)
		));

		// nodeIdArray
		std::vector<OpcUaNodeId::SPtr> nodeIdVec = {
			constructSPtr<OpcUaNodeId>(1,1),
			constructSPtr<OpcUaNodeId>(2,1),
			constructSPtr<OpcUaNodeId>(3,1)
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(233, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(nodeIdVec, Success, now)
		));

		// QualifiedName
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(234, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(OpcUaQualifiedName("Name", 1), Success, now)
		));

		// QualifiedNameArray
		std::vector<OpcUaQualifiedName::SPtr> qualifiedNameVec = {
			constructSPtr<OpcUaQualifiedName>("Name1", 1),
			constructSPtr<OpcUaQualifiedName>("Name2", 1),
			constructSPtr<OpcUaQualifiedName>("Name3", 1)
		};
		valueMap_.insert(std::make_pair(
			OpcUaNodeId(235, namespaceIndex_),
			constructSPtr<OpcUaDataValue>(qualifiedNameVec, Success, now)
		));

		// create vector of node identifier
		for (auto it : valueMap_) valueVec_.push_back(it.first);

		return true;
	}

	bool
	TestFolderLib::registerCallbacks(void)
	{
	  	ServiceTransactionRegisterForwardNode::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardNode>();
	  	RegisterForwardNodeRequest::SPtr req = trx->request();
	  	RegisterForwardNodeResponse::SPtr res = trx->response();

	  	req->forwardNodeSync()->readService().setCallback(readCallback_);
	  	req->forwardNodeSync()->writeService().setCallback(writeCallback_);
	  	req->nodesToRegister()->resize(valueMap_.size());

	  	uint32_t pos = 0;
	  	for (auto it = valueMap_.begin(); it != valueMap_.end(); it++) {
	  		OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>();
	  		*nodeId = it->first;

	  		req->nodesToRegister()->set(pos, nodeId);
	  		pos++;
	  	}

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		std::cout << "response error" << std::endl;
	  		return false;
	  	}

	  	for (pos = 0; pos < res->statusCodeArray()->size(); pos++) {
	  		OpcUaStatusCode statusCode;
	  		res->statusCodeArray()->get(pos, statusCode);
	  		if (statusCode != Success) {
	  			std::cout << "register value error" << std::endl;
	  			return false;
	  		}
	  	}

	    return true;
	}

	bool
	TestFolderLib::registerLoopTimeCallbacks(void)
	{
	  	OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>();
	  	nodeId->set(3, namespaceIndex_);

	  	ServiceTransactionRegisterForwardNode::SPtr trx = constructSPtr<ServiceTransactionRegisterForwardNode>();
	  	RegisterForwardNodeRequest::SPtr req = trx->request();
	  	RegisterForwardNodeResponse::SPtr res = trx->response();

	  	req->forwardNodeSync()->readService().setCallback(readLoopTimeCallback_);
	  	req->forwardNodeSync()->writeService().setCallback(writeLoopTimeCallback_);
	  	req->nodesToRegister()->resize(1);
	  	req->nodesToRegister()->set(0, nodeId);

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		std::cout << "response error" << std::endl;
	  	  	return false;
	  	}

	  	OpcUaStatusCode statusCode;
	  	res->statusCodeArray()->get(0, statusCode);
	  	if (statusCode != Success) {
	  	  	std::cout << "register value error" << std::endl;
	  	  	return false;
	  	}

	  	return true;
	}

	bool
	TestFolderLib::createNodeReferences(void)
	{
		ServiceTransactionGetNodeReference::SPtr trx = constructSPtr<ServiceTransactionGetNodeReference>();
		GetNodeReferenceRequest::SPtr req = trx->request();
		GetNodeReferenceResponse::SPtr res = trx->response();

	  	uint32_t pos = 0;
	  	ValueMap::iterator it;
	  	req->nodes()->resize(valueMap_.size());
	  	for (it = valueMap_.begin(); it != valueMap_.end(); it++) {
	  		OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>();
	  		*nodeId = it->first;

	  		req->nodes()->set(pos, nodeId);
	  		pos++;
	  	}

	  	applicationServiceIf_->sendSync(trx);
	  	if (trx->statusCode() != Success) {
	  		std::cout << "response error" << std::endl;
	  		return false;
	  	}

	  	for (pos = 0; pos < res->nodeReferenceArray()->size(); pos++) {
	  		NodeReference::SPtr nodeReference;
	  		res->nodeReferenceArray()->get(pos, nodeReference);
	  		if (nodeReference->statusCode() != Success) {
	  			std::cout << "node reference error" << std::endl;
	  			return false;
	  		}

	  		OpcUaNodeId::SPtr nodeId;
	  		req->nodes()->get(pos, nodeId);

	  		NodeReferenceApplication::SPtr nodeReferenceApplication;
	  		nodeReferenceApplication = boost::static_pointer_cast<NodeReferenceApplication>(nodeReference);
	  		baseNodeClassWMap_.insert(std::make_pair(*nodeId, nodeReferenceApplication->baseNodeClass()));
	  	}

		return true;
	}

	void
	TestFolderLib::readValue(ApplicationReadContext* applicationReadContext)
	{
	    ValueMap::iterator it;
	    it = valueMap_.find(applicationReadContext->nodeId_);
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
		//std::cout << "read loop time value ..." << applicationReadContext->nodeId_ << std::endl;

		applicationReadContext->statusCode_ = Success;
		loopTime_->copyTo(applicationReadContext->dataValue_);
	}

	void
	TestFolderLib::writeValue(ApplicationWriteContext* applicationWriteContext)
	{
		//std::cout << "write value ..." << applicationWriteContext->nodeId_  << std::endl;

		ValueMap::iterator it;
		it = valueMap_.find(applicationWriteContext->nodeId_);
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
		applicationWriteContext->dataValue_.copyTo(*loopTime_);

		if (slotTimerElement_.get() != nullptr) {
			ioThread_->slotTimer()->stop(slotTimerElement_);
			slotTimerElement_.reset();
		}
		if (timerInterval_ == 0) return;

		slotTimerElement_ = constructSPtr<SlotTimerElement>();
		slotTimerElement_->callback().reset(boost::bind(&TestFolderLib::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), timerInterval_);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	TestFolderLib::startTimerLoop(void)
	{
		// TimerInterval
		OpcUaUInt32 loopTime(1111);
		loopTime_->variant()->variant(loopTime);

		slotTimerElement_ = constructSPtr<SlotTimerElement>();
		slotTimerElement_->callback().reset(boost::bind(&TestFolderLib::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 1111);
		ioThread_->slotTimer()->start(slotTimerElement_);
	}

	void
	TestFolderLib::timerLoop(void)
	{
	  	ValueMap::iterator it1;
	  	for (it1 = valueMap_.begin(); it1 != valueMap_.end(); it1++) {

	  		OpcUaNodeId::SPtr nodeId = constructSPtr<OpcUaNodeId>();
	  		OpcUaDataValue::SPtr dataValue = it1->second;
	  		*nodeId = it1->first;

	  		//std::cout << "update " << *nodeId << std::endl;

	  		BaseNodeClassWMap::iterator it2;
	  		it2 = baseNodeClassWMap_.find(*nodeId);
	  		if (it2 == baseNodeClassWMap_.end()) {
	  			std::cout << "baseNodeClass not exist: " << *nodeId << std::endl;
	  			continue;
	  		}
	  		BaseNodeClass::WPtr baseNodeClassWPtr = it2->second;
	  		BaseNodeClass::SPtr baseNodeClass = baseNodeClassWPtr.lock();
	  		if (baseNodeClass.get() == nullptr) {
	  			std::cout << "baseNodeClass is deleted: " << *nodeId << std::endl;
	  			continue;
	  		}

	  		if (dataValue->variant()->isArray()) {
	  			updateArray(*nodeId, dataValue, baseNodeClass);
	  		}
	  		else {
	  			updateSingle(*nodeId, dataValue, baseNodeClass);
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
				OpcUaSByte sByte = dataValue->variant()->get<OpcUaSByte>();
				sByte++;
				dataValue->variant()->set(sByte);
				break;
			}
			case OpcUaBuildInType_OpcUaByte: // 0 - 64
			{
				OpcUaByte byte = dataValue->variant()->get<OpcUaByte>();
				byte += (64 * myPercent() * mySign());
			    if (byte < 0) byte = 0;
			    if (byte > 64) byte = 64;
				dataValue->variant()->set(byte);
				break;
			}
			case OpcUaBuildInType_OpcUaInt16: // -50 - 100
			{
				OpcUaInt16 int16 = dataValue->variant()->get<OpcUaInt16>();
				int16 += (150 * myPercent() * mySign());
				if (int16 < -50) int16 = -50;
				if (int16 > 100) int16 = 100;
				dataValue->variant()->set(int16);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt16: // 0 - 100
			{
				OpcUaUInt16 uint16 = dataValue->variant()->get<OpcUaUInt16>();
				uint16 += (100 * myPercent() * mySign());
				if (uint16 < 0) uint16 = 0;
				if (uint16 > 100) uint16 = 100;
				dataValue->variant()->set(uint16);
				break;
			}
			case OpcUaBuildInType_OpcUaInt32: // 0 - 360
			{
				OpcUaInt32 int32 = dataValue->variant()->get<OpcUaInt32>();
				int32 += (360 * myPercent() * mySign());
				if (int32 < 0) int32 = 0;
				if (int32 > 360) int32 = 360;
				dataValue->variant()->set(int32);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt32: // 0 - 360
			{
				OpcUaUInt32 uint32 = dataValue->variant()->get<OpcUaUInt32>();
				uint32 += (360 * myPercent() * mySign());
				if (uint32 < 0) uint32 = 0;
				if (uint32 > 360) uint32 = 360;
				dataValue->variant()->set(uint32);
				break;
			}
			case OpcUaBuildInType_OpcUaInt64: // -1000 - 1000
			{
				OpcUaInt64 int64 = dataValue->variant()->get<OpcUaInt64>();
				int64 += (2000 * myPercent() * mySign());
				if (int64 < -1000) int64 = 1000;
				if (int64 > 1000) int64 = 1000;
				dataValue->variant()->set(int64);
				break;
			}
			case OpcUaBuildInType_OpcUaUInt64: // 0 -3000
			{
				OpcUaUInt64 uint64 = dataValue->variant()->get<OpcUaUInt64>();
				uint64 += (3000 * myPercent() * mySign());
				if (uint64 < 0) uint64 = 0;
				if (uint64 > 3000) uint64 = 3000;
				dataValue->variant()->set(uint64);
				break;
			}
			case OpcUaBuildInType_OpcUaFloat: // 0 - 1000
			{
				OpcUaFloat floats = dataValue->variant()->get<OpcUaFloat>();
				floats += (1000 * myPercent() * mySign());
				if (floats < 0) floats = 0;
				if (floats > 1000) floats = 1000;
				dataValue->variant()->set(floats);
				break;
			}
			case OpcUaBuildInType_OpcUaDouble: // 0 - 1000
			{
				OpcUaDouble doubles = dataValue->variant()->get<OpcUaDouble>();
				doubles += (1000 * myPercent() * mySign());
				if (doubles < 0) doubles = 0;
				if (doubles > 1000) doubles = 1000;
				dataValue->variant()->set(doubles);
				break;
			}
			case OpcUaBuildInType_OpcUaBoolean:
			{
				OpcUaBoolean boolean = dataValue->variant()->get<OpcUaBoolean>();
				if (boolean) boolean = false;
				else boolean = true;
				dataValue->variant()->set(boolean);
				break;
			}
			case OpcUaBuildInType_OpcUaString:
			{
				OpcUaString::SPtr str = dataValue->variant()->getSPtr<OpcUaString>();
				std::stringstream ss;
				ss << "String" << rand();
				str->value(ss.str());
				dataValue->variant()->set(str);
				break;
			}
			case OpcUaBuildInType_OpcUaByteString:
			{
				OpcUaByteString::SPtr bstr = dataValue->variant()->getSPtr<OpcUaByteString>();
				std::stringstream ss;
				ss << "String" << rand();
				bstr->value(ss.str());
				dataValue->variant()->set(bstr);
				break;
			}
			case OpcUaBuildInType_OpcUaLocalizedText:
			{
				OpcUaLocalizedText::SPtr ltext = dataValue->variant()->getSPtr<OpcUaLocalizedText>();
				std::stringstream ss;
				ss << "String" << rand();
				ltext->set("de", ss.str());
				dataValue->variant()->set(ltext);
				break;
			}
			case OpcUaBuildInType_OpcUaDateTime:
			{
				OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
				dataValue->variant()->set(dateTime);
				break;
			}
			case OpcUaBuildInType_OpcUaGuid:
			{
				OpcUaGuid::SPtr guid = dataValue->variant()->getSPtr<OpcUaGuid>();
				guid->data1(guid->data1()+1);
				dataValue->variant()->set(guid);
				break;
			}
			case OpcUaBuildInType_OpcUaNodeId:
			{
				OpcUaNodeId::SPtr nodeIdValue = dataValue->variant()->getSPtr<OpcUaNodeId>();
				nodeIdValue->set(rand(),1);
				dataValue->variant()->set(nodeIdValue);
				break;
			}
			case OpcUaBuildInType_OpcUaQualifiedName:
			{
				OpcUaQualifiedName::SPtr qualifiedName = dataValue->variant()->getSPtr<OpcUaQualifiedName>();
				qualifiedName->set("Name", rand());
				dataValue->variant()->set(qualifiedName);
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
	}

	void
	TestFolderLib::updateArray(const OpcUaNodeId& nodeId, const OpcUaDataValue::SPtr dataValue, const BaseNodeClass::SPtr baseNodeClass)
	{
		switch (dataValue->variant()->variantType())
		{
			case OpcUaBuildInType_OpcUaSByte:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaSByte sByte = dataValue->variant()->get<OpcUaSByte>(pos);
					sByte++;
					dataValue->variant()->set(pos, sByte);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaByte:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaByte byte = dataValue->variant()->get<OpcUaByte>(pos);
					byte++;
					dataValue->variant()->set(pos, byte);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaInt16:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaInt16 int16 = dataValue->variant()->get<OpcUaInt16>(pos);
					int16++;
					dataValue->variant()->set(pos, int16);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaUInt16:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaUInt16 uint16 = dataValue->variant()->get<OpcUaUInt16>(pos);
					uint16++;
					dataValue->variant()->set(pos, uint16);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaInt32:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaInt32 int32 = dataValue->variant()->get<OpcUaInt32>(pos);
					int32++;
					dataValue->variant()->set(pos, int32);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaUInt32:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaUInt32 uint32 = dataValue->variant()->get<OpcUaUInt32>(pos);
					uint32++;
					dataValue->variant()->set(pos, uint32);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaInt64:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaInt64 int64 = dataValue->variant()->get<OpcUaInt64>(pos);
					int64++;
					dataValue->variant()->set(pos, int64);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaUInt64:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaUInt64 uint64 = dataValue->variant()->get<OpcUaUInt64>(pos);
					uint64++;
					dataValue->variant()->set(pos, uint64);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaFloat:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaFloat floats = dataValue->variant()->get<OpcUaFloat>(pos);
					floats += 3.354;
					dataValue->variant()->set(pos, floats);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaDouble:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaDouble doubles = dataValue->variant()->get<OpcUaDouble>(pos);
					doubles += 33.354;
					dataValue->variant()->set(pos, doubles);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaBoolean:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaBoolean boolean = dataValue->variant()->get<OpcUaBoolean>(pos);
					if (boolean) boolean = false;
					else boolean = true;
					dataValue->variant()->set(pos, boolean);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaString:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaString::SPtr str = dataValue->variant()->getSPtr<OpcUaString>(pos);
					std::stringstream ss;
					ss << "String" << rand();
					str->value(ss.str());
					dataValue->variant()->set(pos, str);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaByteString:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaByteString::SPtr bstr = dataValue->variant()->getSPtr<OpcUaByteString>(pos);
					std::stringstream ss;
					ss << "String" << rand();
					bstr->value(ss.str());
					dataValue->variant()->set(pos, bstr);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaLocalizedText:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaLocalizedText::SPtr ltext = dataValue->variant()->getSPtr<OpcUaLocalizedText>(pos);
					std::stringstream ss;
					ss << "String" << rand();
					ltext->set("de", ss.str());
					dataValue->variant()->set(pos, ltext);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaDateTime:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaDateTime dateTime(boost::posix_time::microsec_clock::universal_time());
					dataValue->variant()->set(pos, dateTime);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaGuid:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaGuid::SPtr guid = dataValue->variant()->getSPtr<OpcUaGuid>(pos);
					guid->data1(guid->data1()+1);
					dataValue->variant()->set(pos, guid);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaNodeId:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaNodeId::SPtr nodeIdValue = dataValue->variant()->getSPtr<OpcUaNodeId>(pos);
					nodeIdValue->set(rand(), 1);
					dataValue->variant()->set(pos, nodeIdValue);
				}
				break;
			}
			case OpcUaBuildInType_OpcUaQualifiedName:
			{
				for (uint32_t pos = 0; pos < dataValue->variant()->variant().size(); pos++) {
					OpcUaQualifiedName::SPtr qualifiedName = dataValue->variant()->getSPtr<OpcUaQualifiedName>(pos);
					qualifiedName->set("Name", rand());
					dataValue->variant()->set(pos, qualifiedName);
				}
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
	}

}

