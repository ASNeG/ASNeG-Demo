/*
   Copyright 2020 Kai Huebl (kai@huebl-sgh.de)

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

#include "ASNeG-Demo/Library/ForwardContext.h"

using namespace OpcUaStackCore;

namespace OpcUaServerApplicationDemo
{

	ForwardContext::ForwardContext(void)
	: BaseClass()
	{
	}

	ForwardContext::~ForwardContext(void)
	{
	}

	void
	ForwardContext::dataValue(const OpcUaDataValue::SPtr& dataValue)
	{
		dataValue_ = dataValue;
	}

	OpcUaDataValue::SPtr
	ForwardContext::dataValue(void)
	{
		return dataValue_;
	}

	void
	ForwardContext::forwardCallback(ForwardCallback forwardCallback)
	{
		forwardCallback_ = forwardCallback;
	}

	ForwardContext::ForwardCallback
	ForwardContext::forwardCallback(void)
	{
		return forwardCallback_;
	}

}
