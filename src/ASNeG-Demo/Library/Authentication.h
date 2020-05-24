/*
   Copyright 2018-2020 Kai Huebl (kai@huebl-sgh.de)

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
namespace OpcUaServerApplicationDemo
{

	class UserProfile
	: public OpcUaStackCore::UserContext
	{
	  public:
		using SPtr = boost::shared_ptr<UserProfile>;
		using Map = std::map<std::string, UserProfile::SPtr>;

		UserProfile(void);
		~UserProfile(void);

		void username(const std::string& username);
		std::string& username(void);
		void password(const std::string& password);
		std::string& password(void);
		std::set<OpcUaStackCore::OpcUaNodeId>& readNotAllowed(void);
		std::set<OpcUaStackCore::OpcUaNodeId>& writeNotAllowed(void);
		std::set<OpcUaStackCore::OpcUaNodeId>& historicalReadNotAllowed(void);
		std::set<OpcUaStackCore::OpcUaNodeId>& historicalWriteNotAllowed(void);
		std::set<OpcUaStackCore::OpcUaNodeId>& monitoredItemNotAllowed(void);
		std::set<OpcUaStackCore::OpcUaNodeId>& eventItemNotAllowed(void);

	  private:
		std::string username_;
		std::string password_;

		std::set<OpcUaStackCore::OpcUaNodeId> readNotAllowed_;
		std::set<OpcUaStackCore::OpcUaNodeId> writeNotAllowed_;
		std::set<OpcUaStackCore::OpcUaNodeId> historicalReadNotAllowed_;
		std::set<OpcUaStackCore::OpcUaNodeId> historicalWriteNotAllowed_;
		std::set<OpcUaStackCore::OpcUaNodeId> monitoredItemNotAllowed_;
		std::set<OpcUaStackCore::OpcUaNodeId> eventItemNotAllowed_;
	};


	class Authentication
	{
	  public:
		Authentication(void);
		~Authentication(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

	  private:
		OpcUaStackCore::IOThread::SPtr ioThread_;
		OpcUaStackServer::ApplicationServiceIf* applicationServiceIf_;
		OpcUaStackServer::ApplicationInfo* applicationInfo_;

		uint32_t namespaceIndex_;
		UserProfile::Map userProfileMap_;

		bool getNamespaceInfo(void);
		bool registerAuthenticationCallback(void);
		bool createUserProfiles(void);
		void authenticationCallback(
			OpcUaStackCore::ApplicationAuthenticationContext* applicationAuthenitcationContext
		);
		void closeSessionCallback(
			OpcUaStackCore::ApplicationCloseSessionContext* applicationCloseSessionContext
		);
		void autorizationCallback(
			OpcUaStackCore::ApplicationAutorizationContext* applicationAutorizationContext
		);

		bool setValuesToDefault(void);
	};

}

#endif
