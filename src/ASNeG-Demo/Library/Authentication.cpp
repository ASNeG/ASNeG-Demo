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

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/ServiceSet/UserNameIdentityToken.h"
#include "OpcUaStackCore/Certificate/Certificate.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNamespaceInfo.h"
#include "OpcUaStackServer/ServiceSetApplication/RegisterForwardGlobal.h"
#include "OpcUaStackServer/ServiceSetApplication/GetNodeReference.h"
#include "ASNeG-Demo/Library/Authentication.h"

namespace OpcUaServerApplicationDemo
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// User Profile
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	UserProfile::UserProfile(void)
	: UserContext()
	, username_("")
	, password_("")
	, readNotAllowed_()
	, writeNotAllowed_()
	, historicalReadNotAllowed_()
	, historicalWriteNotAllowed_()
	, monitoredItemNotAllowed_()
	, eventItemNotAllowed_()
	{
	}

	UserProfile::~UserProfile(void)
	{
	}

	void
	UserProfile::username(const std::string& username)
	{
		username_ = username;
	}

	std::string&
	UserProfile::username(void)
	{
		return username_;
	}

	void
	UserProfile::password(const std::string& password)
	{
		password_= password;
	}

	std::string&
	UserProfile::password(void)
	{
		return password_;
	}

	std::set<OpcUaNodeId>&
	UserProfile::readNotAllowed(void)
	{
		return readNotAllowed_;
	}

	std::set<OpcUaNodeId>&
	UserProfile::writeNotAllowed(void)
	{
		return writeNotAllowed_;
	}

	std::set<OpcUaNodeId>&
	UserProfile::historicalReadNotAllowed(void)
	{
		return historicalReadNotAllowed_;
	}

	std::set<OpcUaNodeId>&
	UserProfile::historicalWriteNotAllowed(void)
	{
		return historicalWriteNotAllowed_;
	}

	std::set<OpcUaNodeId>&
	UserProfile::monitoredItemNotAllowed(void)
	{
		return monitoredItemNotAllowed_;
	}

	std::set<OpcUaNodeId>&
	UserProfile::eventItemNotAllowed(void)
	{
		return eventItemNotAllowed_;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Authentication
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	Authentication::Authentication(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)
	, namespaceIndex_(0)
	{
	}

	Authentication::~Authentication(void)
	{
	}

	bool
	Authentication::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "Authentication::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// read namespace array from opc ua server
		if (!getNamespaceInfo()) {
			return false;
		}

		// register authentication callback
		if (!registerAuthenticationCallback()) {
			return false;
		}

		// create node references and set variables to default
		if (!setValuesToDefault()) {
			return false;
		}

		if (!createUserProfiles()) {
			return false;
		}

		return true;
	}

	bool
	Authentication::shutdown(void)
	{
		Log(Debug, "Authentication::shutdown");

		return true;
	}

	bool
	Authentication::getNamespaceInfo(void)
	{
		GetNamespaceInfo getNamespaceInfo;

		if (!getNamespaceInfo.query(applicationServiceIf_)) {
			std::cout << "NamespaceInfoResponse error" << std::endl;
			return false;
		}

		namespaceIndex_ = getNamespaceInfo.getNamespaceIndex("http://ASNeG-Demo.de/Auth/");
		if (namespaceIndex_ == -1) {
			std::cout << "namespace index not found: http://ASNeG-Demo.de/Auth/" << std::endl;
			return false;
		}

		return true;
	}

	bool
	Authentication::registerAuthenticationCallback(void)
	{
		RegisterForwardGlobal registerForwardGlobal;
		registerForwardGlobal.setAuthenticationCallback(boost::bind(&Authentication::authenticationCallback, this, _1));
		registerForwardGlobal.setAutorizationCallback(boost::bind(&Authentication::autorizationCallback, this, _1));
		registerForwardGlobal.setCloseSessionCallback(boost::bind(&Authentication::closeSessionCallback, this, _1));
		if (!registerForwardGlobal.query(applicationServiceIf_)) {
			std::cout << "registerForwardGlobal response error" << std::endl;
			return false;
		}
	    return true;
	}

	bool
	Authentication::createUserProfiles(void)
	{
		UserProfile::SPtr userProfile;

		// user1 cannot read variables
		userProfile = constructSPtr<UserProfile>();
		userProfile->username("user1");
		userProfile->password("password1");
		userProfile->readNotAllowed().insert(OpcUaNodeId("Auth.Value01", namespaceIndex_));
		userProfile->readNotAllowed().insert(OpcUaNodeId("Auth.Value02", namespaceIndex_));
		userProfile->readNotAllowed().insert(OpcUaNodeId("Auth.Value03", namespaceIndex_));
		userProfile->readNotAllowed().insert(OpcUaNodeId("Auth.Value04", namespaceIndex_));
		userProfile->readNotAllowed().insert(OpcUaNodeId("Auth.Value05", namespaceIndex_));
		userProfileMap_.insert(std::make_pair("user1", userProfile));

		// user2 can not write variables
		userProfile = constructSPtr<UserProfile>();
		userProfile->username("user2");
		userProfile->password("password2");
		userProfile->writeNotAllowed().insert(OpcUaNodeId("Auth.Value01", namespaceIndex_));
		userProfile->writeNotAllowed().insert(OpcUaNodeId("Auth.Value02", namespaceIndex_));
		userProfile->writeNotAllowed().insert(OpcUaNodeId("Auth.Value03", namespaceIndex_));
		userProfile->writeNotAllowed().insert(OpcUaNodeId("Auth.Value04", namespaceIndex_));
		userProfile->writeNotAllowed().insert(OpcUaNodeId("Auth.Value05", namespaceIndex_));
		userProfileMap_.insert(std::make_pair("user2", userProfile));

		// user3 can not monitor variables
		userProfile = constructSPtr<UserProfile>();
		userProfile->username("user3");
		userProfile->password("password3");
		userProfile->monitoredItemNotAllowed().insert(OpcUaNodeId("Auth.Value01", namespaceIndex_));
		userProfile->monitoredItemNotAllowed().insert(OpcUaNodeId("Auth.Value02", namespaceIndex_));
		userProfile->monitoredItemNotAllowed().insert(OpcUaNodeId("Auth.Value03", namespaceIndex_));
		userProfile->monitoredItemNotAllowed().insert(OpcUaNodeId("Auth.Value04", namespaceIndex_));
		userProfile->monitoredItemNotAllowed().insert(OpcUaNodeId("Auth.Value05", namespaceIndex_));
		userProfileMap_.insert(std::make_pair("user3", userProfile));

		// user4 cannot receive events from objects
		userProfile = constructSPtr<UserProfile>();
		userProfile->username("user4");
		userProfile->password("password4");
		userProfile->eventItemNotAllowed().insert(OpcUaNodeId("AuthObject01", namespaceIndex_));
		userProfile->eventItemNotAllowed().insert(OpcUaNodeId("AuthObject02", namespaceIndex_));
		userProfile->eventItemNotAllowed().insert(OpcUaNodeId("AuthObject03", namespaceIndex_));
		userProfile->eventItemNotAllowed().insert(OpcUaNodeId("AuthObject04", namespaceIndex_));
		userProfile->eventItemNotAllowed().insert(OpcUaNodeId("AuthObject05", namespaceIndex_));
		userProfileMap_.insert(std::make_pair("user4", userProfile));

		// user5 can not read historical values
		userProfile = constructSPtr<UserProfile>();
		userProfile->username("user5");
		userProfile->password("password5");
		userProfile->historicalReadNotAllowed().insert(OpcUaNodeId("Auth.Value01", namespaceIndex_));
		userProfile->historicalReadNotAllowed().insert(OpcUaNodeId("Auth.Value02", namespaceIndex_));
		userProfile->historicalReadNotAllowed().insert(OpcUaNodeId("Auth.Value03", namespaceIndex_));
		userProfile->historicalReadNotAllowed().insert(OpcUaNodeId("Auth.Value04", namespaceIndex_));
		userProfile->historicalReadNotAllowed().insert(OpcUaNodeId("Auth.Value05", namespaceIndex_));
		userProfileMap_.insert(std::make_pair("user5", userProfile));

		// user6can not write historical values
		userProfile = constructSPtr<UserProfile>();
		userProfile->username("user6");
		userProfile->password("password6");
		userProfile->historicalWriteNotAllowed().insert(OpcUaNodeId("Auth.Value01", namespaceIndex_));
		userProfile->historicalWriteNotAllowed().insert(OpcUaNodeId("Auth.Value02", namespaceIndex_));
		userProfile->historicalWriteNotAllowed().insert(OpcUaNodeId("Auth.Value03", namespaceIndex_));
		userProfile->historicalWriteNotAllowed().insert(OpcUaNodeId("Auth.Value04", namespaceIndex_));
		userProfile->historicalWriteNotAllowed().insert(OpcUaNodeId("Auth.Value05", namespaceIndex_));
		userProfileMap_.insert(std::make_pair("user6", userProfile));

		return true;
	}

	void
	Authentication::authenticationCallback(ApplicationAuthenticationContext* applicationAuthenitcationContext)
	{
		Log(Debug, "Event::authenticationCallback")
			.parameter("SessionId", applicationAuthenitcationContext->sessionId_);


		if (applicationAuthenitcationContext->authenticationType_ == OpcUaId_AnonymousIdentityToken_Encoding_DefaultBinary) {
			applicationAuthenitcationContext->statusCode_ = Success;
		}
		else if (applicationAuthenitcationContext->authenticationType_ == OpcUaId_UserNameIdentityToken_Encoding_DefaultBinary) {

			ExtensibleParameter::SPtr parameter = applicationAuthenitcationContext->parameter_;
			UserNameIdentityToken::SPtr token = parameter->parameter<UserNameIdentityToken>();

			// find user profile
			UserProfile::Map::iterator it;
			it = userProfileMap_.find(token->userName());
			if ( it == userProfileMap_.end()) {
				applicationAuthenitcationContext->statusCode_ = BadUserAccessDenied;
				return;
			}
			UserProfile::SPtr userProfile = it->second;

			// get password
			char* buf;
		    int32_t bufLen;
			token->password((OpcUaByte**)&buf, (OpcUaInt32*)&bufLen);
			if (bufLen <= 0) {
				applicationAuthenitcationContext->statusCode_ = BadUserAccessDenied;
				return;
			}
			std::string password(buf, bufLen);

			// check password
			if (password != userProfile->password()) {
				applicationAuthenitcationContext->statusCode_ = BadUserAccessDenied;
				return;
			}

			applicationAuthenitcationContext->userContext_ = userProfile;
			applicationAuthenitcationContext->statusCode_ = Success;
		}
		else if (applicationAuthenitcationContext->authenticationType_ == OpcUaId_X509IdentityToken_Encoding_DefaultBinary) {
			applicationAuthenitcationContext->statusCode_ = Success;
		}
		else {
			applicationAuthenitcationContext->statusCode_ = BadIdentityTokenInvalid;
		}
	}

	void
	Authentication::closeSessionCallback(ApplicationCloseSessionContext* applicationCloseSessionContext)
	{
		// FIXME: todo
		std::cout << "close session..." << std::endl;
	}

	void
	Authentication::autorizationCallback(ApplicationAutorizationContext* applicationAutorizationContext)
	{
		Log(Debug, "Event::autorizationCallback");

		if (applicationAutorizationContext->userContext_.get() == nullptr) {
			applicationAutorizationContext->statusCode_ = Success;
			return;
		}

		UserProfile::SPtr userProfile = boost::static_pointer_cast<UserProfile>(applicationAutorizationContext->userContext_);

		std::set<OpcUaNodeId>::iterator it;
		bool notAllowed = false;
		switch (applicationAutorizationContext->serviceOperation_)
		{
			case Read:
			{
				if (applicationAutorizationContext->attributeId_ != AttributeId_Value) {
					break;
				}
				it = userProfile->readNotAllowed().find(applicationAutorizationContext->nodeId_);
				if (it != userProfile->readNotAllowed().end()) {
					notAllowed = true;
				}
				break;
			}
			case Write:
			{
				it = userProfile->writeNotAllowed().find(applicationAutorizationContext->nodeId_);
				if (it != userProfile->writeNotAllowed().end()) {
					notAllowed = true;
				}
				break;
			}
			case HRead:
			{
				it = userProfile->historicalReadNotAllowed().find(applicationAutorizationContext->nodeId_);
				if (it != userProfile->historicalReadNotAllowed().end()) {
					notAllowed = true;
				}
				break;
			}
			case HWrite:
			{
				it = userProfile->historicalWriteNotAllowed().find(applicationAutorizationContext->nodeId_);
				if (it != userProfile->historicalWriteNotAllowed().end()) {
					notAllowed = true;
				}
				break;
			}
			case MonitoredItem:
			{
				it = userProfile->monitoredItemNotAllowed().find(applicationAutorizationContext->nodeId_);
				if (it != userProfile->monitoredItemNotAllowed().end()) {
					notAllowed = true;
				}
				break;
			}
			case EventItem:
			{
				it = userProfile->eventItemNotAllowed().find(applicationAutorizationContext->nodeId_);
				if (it != userProfile->eventItemNotAllowed().end()) {
					notAllowed = true;
				}
				break;
			}
			case Method:
			{
				// all methods allowed in this example
				break;
			}
			default:
			{
				break;
			}
		}

		if (notAllowed) {
			applicationAutorizationContext->statusCode_ = BadUserAccessDenied;
		}
		else {
			applicationAutorizationContext->statusCode_ = Success;
		}
	}

	bool
	Authentication::setValuesToDefault(void)
	{
		Log(Debug, "get references");

		// read node references
		GetNodeReference getNodeReference({
			OpcUaNodeId("Auth.Value01", namespaceIndex_),
			OpcUaNodeId("Auth.Value02", namespaceIndex_),
			OpcUaNodeId("Auth.Value03", namespaceIndex_),
			OpcUaNodeId("Auth.Value04", namespaceIndex_),
			OpcUaNodeId("Auth.Value05", namespaceIndex_),
		});
		if (!getNodeReference.query(applicationServiceIf_, true)) {
	  		std::cout << "response error" << std::endl;
	  		return false;
		}

		value01_ = getNodeReference.nodeReferences()[0];
		value02_ = getNodeReference.nodeReferences()[1];
		value03_ = getNodeReference.nodeReferences()[2];
		value04_ = getNodeReference.nodeReferences()[3];
		value05_ = getNodeReference.nodeReferences()[4];

		BaseNodeClass::SPtr baseNodeClass;

		baseNodeClass = value01_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		OpcUaDataValue dataValue1((double)1);
		baseNodeClass->setValueSync(dataValue1);

		baseNodeClass = value02_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		OpcUaDataValue dataValue2((double)2);
		baseNodeClass->setValueSync(dataValue2);

		baseNodeClass = value03_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		OpcUaDataValue dataValue3((double)3);
		baseNodeClass->setValueSync(dataValue3);

		baseNodeClass = value04_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		OpcUaDataValue dataValue4((double)4);
		baseNodeClass->setValueSync(dataValue4);

		baseNodeClass = value05_.lock();
		if (baseNodeClass.get() == nullptr) return false;
		OpcUaDataValue dataValue5((double)5);
		baseNodeClass->setValueSync(dataValue5);

		return true;
	}

	bool
	Authentication::getRefFromResponse(GetNodeReferenceResponse::SPtr& res, uint32_t idx, BaseNodeClass::WPtr& ref)
	{
		NodeReference::SPtr nodeReference;
		if (!res->nodeReferenceArray()->get(idx, nodeReference)) {
			Log(Error, "reference result not exist in response")
				.parameter("Idx", idx);
			return false;
		}

		if (nodeReference->statusCode() != Success) {
			Log(Error, "reference error in response")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(nodeReference->statusCode()))
				.parameter("Idx", idx);
			return false;
		}

  		NodeReferenceApplication::SPtr nodeReferenceApplication;
  		nodeReferenceApplication = boost::static_pointer_cast<NodeReferenceApplication>(nodeReference);
  		ref = nodeReferenceApplication->baseNodeClass();

		return true;
	}

}
