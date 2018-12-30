/*
   Copyright 2018 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaServerApplicationDemo_Authentication_h__
#define __OpcUaServerApplicationDemo_Authentication_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationAuthenticationContext.h"
#include "OpcUaStackCore/Application/ApplicationAutorizationContext.h"
#include "OpcUaStackCore/Application/ApplicationCloseSessionContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaServerApplicationDemo
{

	class UserProfile
	: public UserContext
	{
	  public:
		typedef boost::shared_ptr<UserProfile> SPtr;
		typedef std::map<std::string, UserProfile::SPtr> Map;

		UserProfile(void);
		~UserProfile(void);

		void username(const std::string& username);
		std::string& username(void);
		void password(const std::string& password);
		std::string& password(void);
		std::set<OpcUaNodeId>& readNotAllowed(void);
		std::set<OpcUaNodeId>& writeNotAllowed(void);
		std::set<OpcUaNodeId>& historicalReadNotAllowed(void);
		std::set<OpcUaNodeId>& historicalWriteNotAllowed(void);
		std::set<OpcUaNodeId>& monitoredItemNotAllowed(void);
		std::set<OpcUaNodeId>& eventItemNotAllowed(void);

	  private:
		std::string username_;
		std::string password_;

		std::set<OpcUaNodeId> readNotAllowed_;
		std::set<OpcUaNodeId> writeNotAllowed_;
		std::set<OpcUaNodeId> historicalReadNotAllowed_;
		std::set<OpcUaNodeId> historicalWriteNotAllowed_;
		std::set<OpcUaNodeId> monitoredItemNotAllowed_;
		std::set<OpcUaNodeId> eventItemNotAllowed_;
	};


	class Authentication
	{
	  public:
		Authentication(void);
		~Authentication(void);

		bool startup(IOThread& ioThread, ApplicationServiceIf& applicationServiceIf, ApplicationInfo* applicationInfo);
		bool shutdown(void);

	  private:
		IOThread* ioThread_;
		ApplicationServiceIf* applicationServiceIf_;
		ApplicationInfo* applicationInfo_;

		uint32_t namespaceIndex_;

		BaseNodeClass::WPtr value01_;
		BaseNodeClass::WPtr value02_;
		BaseNodeClass::WPtr value03_;
		BaseNodeClass::WPtr value04_;
		BaseNodeClass::WPtr value05_;

		UserProfile::Map userProfileMap_;

		bool getNamespaceInfo(void);
		bool registerAuthenticationCallback(void);
		bool createUserProfiles(void);
		void authenticationCallback(ApplicationAuthenticationContext* applicationAuthenitcationContext);
		void closeSessionCallback(ApplicationCloseSessionContext* applicationCloseSessionContext);
		void autorizationCallback(ApplicationAutorizationContext* applicationAutorizationContext);

		bool setValuesToDefault(void);
	};

}

#endif
