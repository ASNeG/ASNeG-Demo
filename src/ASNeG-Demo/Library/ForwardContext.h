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

#ifndef __OpcUaServerApplicationDemo_ForwardContext_h__
#define __OpcUaServerApplicationDemo_ForwardContext_h__

#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>

#include "OpcUaStackCore/Base/BaseClass.h"
#include "OpcUaStackCore/BuildInTypes/OpcUaDataValue.h"
#include "OpcUaStackServer/Forward/ForwardTransaction.h"

namespace OpcUaServerApplicationDemo
{

	class ForwardContext
	: public OpcUaStackCore::BaseClass
	{
	  public:
		using SPtr = boost::shared_ptr<ForwardContext>;
		using Vec = std::vector<SPtr>;
		using Map = std::map<OpcUaStackCore::OpcUaNodeId, SPtr>;
		using ForwardCallback = std::function<void (OpcUaStackServer::ForwardTransaction::SPtr&)>;

		ForwardContext(void);
		~ForwardContext(void);

		void dataValue(const OpcUaStackCore::OpcUaDataValue::SPtr& dataValue);
		OpcUaStackCore::OpcUaDataValue::SPtr dataValue(void);
		void forwardCallback(ForwardCallback forwardCallback);
		ForwardCallback forwardCallback(void);

	  private:
		OpcUaStackCore::OpcUaDataValue::SPtr dataValue_ = nullptr;
		ForwardCallback forwardCallback_;
	};

}

#endif
