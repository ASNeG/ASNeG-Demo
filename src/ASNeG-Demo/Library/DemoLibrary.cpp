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
#include "ASNeG-Demo/Library/DemoLibrary.h"
#include "ASNeG-Demo/Library/ForwardContext.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include <iostream>
#include "BuildConfig.h"

using namespace OpcUaStackServer;
using namespace OpcUaStackCore;

namespace OpcUaServerApplicationDemo
{

	DemoLibrary::DemoLibrary(void)
	: ApplicationIf()
	, cameraAnimation_()
	, testFolderLib_()
	, testForward_()
	, function_()
	, functionForward_()
	, serviceFunction_()
	, event_()
	, alarm_()
	, authentication_()
	, historicalAccess_()
	, discovery_()
	, createDeleteNode_()
	, variableType_()
	, objectType_()
	{
	}

	DemoLibrary::~DemoLibrary(void)
	{
	}

	bool
	DemoLibrary::startup(void)
	{
		Log(Debug, "DemoLibrary::startup");

		IOThread::SPtr& ioThread = this->applicationThreadPool();

		testFolderLib_.startup(ioThread, service(), applicationInfo());
		testForward_.startup(ioThread, service(), applicationInfo());
		testStatusCode_.startup(ioThread, service(), applicationInfo());
		cameraAnimation_.startup(ioThread, service(), applicationInfo());
		function_.startup(ioThread, service(), applicationInfo());
		serviceFunction_.startup(ioThread, service(), applicationInfo());
		event_.startup(ioThread, service(), applicationInfo());
		alarm_.startup(ioThread, service(), applicationInfo());
		authentication_.startup(ioThread, service(), applicationInfo());
		historicalAccess_.startup(ioThread, service(), applicationInfo());
		discovery_.startup(ioThread, service(), applicationInfo());
		createDeleteNode_.startup(ioThread, service(), applicationInfo());
		variableType_.startup(ioThread, service(), applicationInfo());
		objectType_.startup(ioThread, service(), applicationInfo());
		functionForward_.startup(ioThread, service(), applicationInfo());
		return true;
	}

	bool
	DemoLibrary::shutdown(void)
	{
		Log(Debug, "DemoLibrary::shutdown");

		functionForward_.shutdown();
		objectType_.shutdown();
		variableType_.shutdown();
		createDeleteNode_.shutdown();
		discovery_.shutdown();
		historicalAccess_.shutdown();
		authentication_.shutdown();
		alarm_.shutdown();
		event_.shutdown();
		serviceFunction_.shutdown();
		function_.shutdown();
		cameraAnimation_.shutdown();
		testStatusCode_.shutdown();
		testFolderLib_.shutdown();
		testForward_.shutdown();

		return true;
	}

	std::string
	DemoLibrary::version(void)
	{
		std::stringstream version;

		version << LIBRARY_VERSION_MAJOR << "." << LIBRARY_VERSION_MINOR << "." << LIBRARY_VERSION_PATCH;
		return version.str();
	}

	std::string
	DemoLibrary::gitCommit(void)
	{
		return LIBRARY_GIT_COMMIT;
	}

	std::string
	DemoLibrary::gitBranch(void)
	{
		return LIBRARY_GIT_BRANCH;
	}

	void
	DemoLibrary::receiveForwardTrx(OpcUaStackServer::ForwardTransaction::SPtr forwardTransaction)
	{
		auto forwardContext = boost::static_pointer_cast<ForwardContext>(forwardTransaction->applicationContext());
		forwardContext->forwardCallback()(forwardTransaction);
	}

}

extern "C" DLLEXPORT void  init(ApplicationIf** applicationIf) {
    *applicationIf = new OpcUaServerApplicationDemo::DemoLibrary();
}

