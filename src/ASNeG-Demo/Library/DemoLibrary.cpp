/*
   Copyright 2015-2019 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include <iostream>
#include "BuildConfig.h"

namespace OpcUaServerApplicationDemo
{

	DemoLibrary::DemoLibrary(void)
	: ApplicationIf()
	, cameraAnimation_()
	, testFolderLib_()
	, function_()
	, serviceFunction_()
	, event_()
	, alarm_()
	, authentication_()
	, historicalAccess_()
	, discovery_()
	, createDeleteNode_()
	, variableType_()
	{
	}

	DemoLibrary::~DemoLibrary(void)
	{
	}

	bool
	DemoLibrary::startup(void)
	{
		Log(Debug, "DemoLibrary::startup");

		ioThread_.startup();
		testFolderLib_.startup(ioThread_, service(), applicationInfo());
		testStatusCode_.startup(ioThread_, service(), applicationInfo());
		cameraAnimation_.startup(ioThread_, service(), applicationInfo());
		function_.startup(ioThread_, service(), applicationInfo());
		serviceFunction_.startup(ioThread_, service(), applicationInfo());
		event_.startup(ioThread_, service(), applicationInfo());
		alarm_.startup(ioThread_, service(), applicationInfo());
		authentication_.startup(ioThread_, service(), applicationInfo());
		historicalAccess_.startup(ioThread_, service(), applicationInfo());
		discovery_.startup(ioThread_, service(), applicationInfo());
		createDeleteNode_.startup(ioThread_, service(), applicationInfo());
		variableType_.startup(ioThread_, service(), applicationInfo());
		return true;
	}

	bool
	DemoLibrary::shutdown(void)
	{
		Log(Debug, "DemoLibrary::shutdown");

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
		ioThread_.shutdown();

		return true;
	}

	std::string
	DemoLibrary::version(void)
	{
		std::stringstream version;

		version << LIBRARY_VERSION_MAJOR << "." << LIBRARY_VERSION_MINOR << "." << LIBRARY_VERSION_PATCH;
		return version.str();
	}

}

extern "C" DLLEXPORT void  init(ApplicationIf** applicationIf) {
    *applicationIf = new OpcUaServerApplicationDemo::DemoLibrary();
}

