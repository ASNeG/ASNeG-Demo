/*
   Copyright 2016-2017 Kai Huebl (kai@huebl-sgh.de)

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

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "ASNeG-Demo/Library/Generator.h"

namespace OpcUaServerApplicationDemo
{

	Generator::Generator(void)
	{
	}

	Generator::~Generator(void)
	{
	}

	bool
	Generator::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Generator::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua information model
		// we will find the right namespace index
		if (!getNamespaceInfo()) {
			return false;
		}

		// connect to variable type instance in opc ua information model
		if (!connectToVariableType()) {
			return false;
		}

		// create and connect to variable type instance in opc ua information model
		if (!createAndConnectToVariableType()) {
			return true;
		}

		return true;
	}

	bool
	Generator::shutdown(void)
	{
		Log(Debug, "Generator::shutdown");
		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// private functions
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Generator::getNamespaceInfo(void)
	{
		Log(Debug, "get namespace info");

		ServiceTransactionNamespaceInfo::SPtr trx = constructSPtr<ServiceTransactionNamespaceInfo>();
		NamespaceInfoRequest::SPtr req = trx->request();
		NamespaceInfoResponse::SPtr res = trx->response();

		applicationServiceIf_->sendSync(trx);
		if (trx->statusCode() != Success) {
			Log(Error, "NamespaceInfoResponse error")
			    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->statusCode()));
			return false;
		}

		NamespaceInfoResponse::Index2NamespaceMap::iterator it;
		for (
		    it = res->index2NamespaceMap().begin();
			it != res->index2NamespaceMap().end();
			it++
		)
		{
			if (it->second == "http://ASNeG-Demo.de/Generator/") {
				namespaceIndex_ = it->first;
				return true;
			}
 		}

		Log(Error, "namespace not found in opc ua information model")
	        .parameter("NamespaceUri", "http://ASNeG-Demo.de/Generator/");

		return false;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// variable type functions
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Generator::connectToVariableType(void)
	{
		Log(Debug, "connect to variable type");

		OpcUaNodeId nodeId;
		OpcUaDataValue::SPtr dataValue;


		stateVariable1_.applicationServiceIf(applicationServiceIf_);
		nodeId.set("VariableTypes.StateVariable1", namespaceIndex_);
		if (!stateVariable1_.linkInstanceWithModel(nodeId)) {
			Log(Error, "link opc ua information model error")
				.parameter("NodeId", nodeId);
			return false;
		}
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "StateVariable1"));
		stateVariable1_.setValue(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "EffectiveDisplayName1"));
		stateVariable1_.setEffectiveDisplayName(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "Id1"));
		stateVariable1_.setId(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaQualifiedName("Name1"));
		stateVariable1_.setName(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue((OpcUaUInt32)1);
		stateVariable1_.setNumber(*dataValue);


		stateVariable2_.applicationServiceIf(applicationServiceIf_);
		nodeId.set("VariableTypes.StateVariable2", namespaceIndex_);
		if (!stateVariable2_.linkInstanceWithModel(nodeId)) {
			Log(Error, "link opc ua information model error")
				.parameter("NodeId", nodeId);
			return false;
		}
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "StateVariable2"));
		stateVariable2_.setValue(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "EffectiveDisplayName2"));
		stateVariable2_.setEffectiveDisplayName(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "Id2"));
		stateVariable2_.setId(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaQualifiedName("Name2"));
		stateVariable2_.setName(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue((OpcUaUInt32)2);
		stateVariable2_.setNumber(*dataValue);


		stateVariable3_.applicationServiceIf(applicationServiceIf_);
		nodeId.set("VariableTypes.StateVariable3", namespaceIndex_);
		if (!stateVariable3_.linkInstanceWithModel(nodeId)) {
			Log(Error, "link opc ua information model error")
				.parameter("NodeId", nodeId);
			return false;
		}
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "StateVariable3"));
		stateVariable3_.setValue(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "EffectiveDisplayName3"));
		stateVariable3_.setEffectiveDisplayName(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaLocalizedText("", "Id3"));
		stateVariable3_.setId(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue(OpcUaQualifiedName("Name3"));
		stateVariable3_.setName(*dataValue);
		dataValue = constructSPtr<OpcUaDataValue>();
		dataValue->variant()->setValue((OpcUaUInt32)3);
		stateVariable3_.setNumber(*dataValue);

		return true;
	}

	bool
	Generator::createAndConnectToVariableType(void)
	{
		OpcUaDataValue::SPtr dataValue;
		bool result;

		StateVariableType stateVariable[1200];
		for (uint32_t idx=0; idx<1200; idx++) {

			stateVariable[idx].applicationServiceIf(applicationServiceIf_);

			std::stringstream ss;
			ss << std::setfill('0') << std::setw(3) << idx;
			std::string name("StateVariable" + ss.str());
			OpcUaNodeId parentNodeId("VariableTypeFolder", namespaceIndex_);
			OpcUaNodeId nodeId("StateVariabl" + ss.str(), namespaceIndex_);
			OpcUaLocalizedText displayName("", "StateVariable" + ss.str());
			OpcUaQualifiedName browseName("StateVariable" + ss.str(), namespaceIndex_);
			OpcUaNodeId referenceNodeId(35);
			result = stateVariable[idx].createAndLinkInstanceWithModel(
				name,
				parentNodeId,
				nodeId,
				displayName,
				browseName,
				referenceNodeId
			);

			if (!result) {
				Log(Error, "create and link opc ua information model error")
					.parameter("NodeId", nodeId);
				return false;
			}

			dataValue = constructSPtr<OpcUaDataValue>();
			dataValue->variant()->setValue(OpcUaLocalizedText("", "StateVariable" + ss.str()));
			stateVariable[idx].setValue(*dataValue);
			dataValue = constructSPtr<OpcUaDataValue>();
			dataValue->variant()->setValue(OpcUaLocalizedText("", "EffectiveDisplayName" + ss.str()));
			stateVariable[idx].setEffectiveDisplayName(*dataValue);
			dataValue = constructSPtr<OpcUaDataValue>();
			dataValue->variant()->setValue(OpcUaLocalizedText("", "Id" + ss.str()));
			stateVariable[idx].setId(*dataValue);
			dataValue = constructSPtr<OpcUaDataValue>();
			dataValue->variant()->setValue(OpcUaQualifiedName("Name" + ss.str()));
			stateVariable[idx].setName(*dataValue);
			dataValue = constructSPtr<OpcUaDataValue>();
			dataValue->variant()->setValue((OpcUaUInt32)idx);
			stateVariable[idx].setNumber(*dataValue);
		}

		return true;
	}

}
