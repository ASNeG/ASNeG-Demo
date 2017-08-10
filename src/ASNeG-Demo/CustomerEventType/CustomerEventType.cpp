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

#include "ASNeG-Demo/CustomerEventType/CustomerEventType.h"

namespace OpcUaServerApplicationDemo
{

	CustomerEventType::CustomerEventType(void)
	: BaseEventType()
	, eventVariables_()
	{
		eventVariables_.registerEventVariable("EventType", OpcUaBuildInType_OpcUaNodeId);
		eventVariables_.registerEventVariable("Variable1", OpcUaBuildInType_OpcUaDouble);
		eventVariables_.registerEventVariable("Variable2", OpcUaBuildInType_OpcUaDouble);

		eventVariables_.eventType(OpcUaNodeId((OpcUaUInt32)1002));
		eventVariables_.namespaceIndex(0);
		eventVariables_.browseName(OpcUaQualifiedName("CustomerEventType"));
		eventVariables_.namespaceUri("http://ASNeG-Demo.de/Event/");
	}

	CustomerEventType::~CustomerEventType(void)
	{
	}

	bool
	CustomerEventType::variable1(OpcUaVariant::SPtr& variable1)
	{
		return eventVariables_.setValue("Variable1", variable1);
	}

	OpcUaVariant::SPtr
	CustomerEventType::variable1(void)
	{
		OpcUaVariant::SPtr value;
		eventVariables_.getValue("Variable1", value);
		return value;
	}

	bool
	CustomerEventType::variable2(OpcUaVariant::SPtr& variable2)
	{
		return eventVariables_.setValue("Variable2", variable2);
	}

	OpcUaVariant::SPtr
	CustomerEventType::variable2(void)
	{
		OpcUaVariant::SPtr value;
		eventVariables_.getValue("Variable2", value);
		return value;
	}

	void
	CustomerEventType::mapNamespaceUri(void)
	{
		std::cout << "CustomerEventType::mapNamespaceUri" << std::endl;
		uint32_t namespaceIndex;
		BaseEventType::mapNamespaceUri();

		OpcUaVariant::SPtr eventTypeVariable = constructSPtr<OpcUaVariant>();
		eventTypeVariable->setValue(eventVariables_.eventType());

		setNamespaceIndex(eventVariables_.namespaceUri(), namespaceIndex, eventVariables_.browseName(), eventTypeVariable);

		std::cout << "2 set event type: " << *eventTypeVariable << std::endl;
		eventType(eventTypeVariable);
		eventVariables_.eventType(eventTypeVariable);
		eventVariables_.namespaceIndex(namespaceIndex);
	}

	OpcUaVariant::SPtr
	CustomerEventType::get(
		OpcUaNodeId& eventType,
		std::list<OpcUaQualifiedName::SPtr>& browseNameList,
		EventResult::Code& resultCode
	)
	{
		resultCode = EventResult::Success;

		// check whether eventType and typeNodeId are identical
		if (eventType == eventVariables_.eventType()) {
			return eventVariables_.get(browseNameList, resultCode);
		}

		// the start item was not found. We delegate the search to the base class
		OpcUaVariant::SPtr variant;
		variant = BaseEventType::get(eventType, browseNameList, resultCode);
		if (resultCode != EventResult::Success || browseNameList.empty()) {
			return variant;
		}

		return eventVariables_.get(browseNameList, resultCode);
	}

}
