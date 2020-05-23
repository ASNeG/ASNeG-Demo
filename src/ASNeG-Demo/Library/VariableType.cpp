/*
   Copyright 2019-2020 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaStackCore/BuildInTypes/OpcUaIdentifier.h"
#include "OpcUaStackCore/BuildInTypes/OpcUaExtensionObject.h"
#include "OpcUaStackCore/StandardDataTypes/Range.h"
#include "OpcUaStackCore/StandardDataTypes/EUInformation.h"
#include "ASNeG-Demo/Library/VariableType.h"
#include "OpcUaStackServer/ServiceSetApplication/CreateVariableInstance.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	VariableType::VariableType(void)
	: applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)

	, analogItemType_(boost::make_shared<AnalogItemType>())
	{
	}

	VariableType::~VariableType(void)
	{
	}

	bool
	VariableType::startup(
		const IOThread::SPtr& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "VariableType::startup");

		ioThread_ = ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// create variable
		if (!createVariable()) {
			Log(Error, "create variable error");
			return false;
		}

		return true;
	}

	bool
	VariableType::shutdown(void)
	{
		Log(Debug, "VariableType::shutdown");

		return true;
	}

	bool
	VariableType::createVariable(void)
	{
		Log(Debug, "Create variable type");

		//
		// create a new variable instance in the opc ua information model
		//
		Object::SPtr obj = analogItemType_;
		CreateVariableInstance createVariableInstance(
			"http://ASNeG-Demo/VariableType/",				// namespace name of the variable instance
			OpcUaLocalizedText("", "VariableType"),			// display name of the variable instance
			OpcUaNodeId(85),								// parent node of the variable instance
			OpcUaNodeId(35),								// reference type between parent and variable instance
			obj
		);

		if (!createVariableInstance.query(applicationServiceIf_)) {
			Log(Error, "create variable response error");
			return false;
		}


		//
		// set variable values of the variable instance
		//
		analogItemType_->set_Variable(OpcUaDataValue((OpcUaDouble)12.34));

		analogItemType_->set_Definition_Variable(OpcUaDataValue(OpcUaString("Definiton Variable")));

		OpcUaExtensionObject range(OpcUaNodeId(OpcUaId_Range_Encoding_DefaultBinary));
		range.parameter<Range>()->low() = 0;
		range.parameter<Range>()->high() = 1000;
		analogItemType_->set_EURange_Variable(OpcUaDataValue(range));

		OpcUaExtensionObject engineeringUnits(OpcUaNodeId(OpcUaId_EUInformation_Encoding_DefaultBinary));
		engineeringUnits.parameter<EUInformation>()->namespaceUri() = OpcUaString("http://ASNeG-Demo/VariableType/");
		engineeringUnits.parameter<EUInformation>()->unitId() = 4711;
		engineeringUnits.parameter<EUInformation>()->displayName() = OpcUaLocalizedText("de", "DisplayName");
		engineeringUnits.parameter<EUInformation>()->description() = OpcUaLocalizedText("de", "Description");
		analogItemType_->set_EngineeringUnits_Variable(OpcUaDataValue(engineeringUnits));

		OpcUaExtensionObject instrumentRange(OpcUaNodeId(OpcUaId_Range_Encoding_DefaultBinary));
		instrumentRange.parameter<Range>()->low() = -1000;
		instrumentRange.parameter<Range>()->high() = 2000;
		analogItemType_->set_InstrumentRange_Variable(OpcUaDataValue(instrumentRange));

		analogItemType_->set_ValuePrecision_Variable(OpcUaDataValue((OpcUaDouble)56.78));

		Log(Debug, "Create variable type done");
		return true;
	}


}
