/*
   Copyright 2015-2021 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __ASNeGDemo_Library_h__
#define __ASNeGDemo_Library_h__

#include "OpcUaStackCore/Base/IOService.h"
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationReadContext.h"
#include "OpcUaStackCore/Application/ApplicationWriteContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"
#include "ASNeG-Demo/Library/CameraAnimation.h"
#include "ASNeG-Demo/Library/Function.h"
#include "ASNeG-Demo/Library/ServiceFunction.h"
#include "ASNeG-Demo/Library/TestFolderLib.h"
#include "ASNeG-Demo/Library/TestStatusCode.h"
#include "ASNeG-Demo/Library/Event.h"
#include "ASNeG-Demo/Library/Alarm.h"
#include "ASNeG-Demo/Library/Authentication.h"
#include "ASNeG-Demo/Library/HistoricalAccess.h"
#include "ASNeG-Demo/Library/Discovery.h"
#include "ASNeG-Demo/Library/CreateDeleteNode.h"
#include "ASNeG-Demo/Library/VariableType.h"
#include "ASNeG-Demo/Library/ObjectType.h"

namespace OpcUaServerApplicationDemo
{

	class DemoLibrary
	: public OpcUaStackServer::ApplicationIf
	{
	  public:
		DemoLibrary(void);
		virtual ~DemoLibrary(void);

		//- ApplicationIf -----------------------------------------------------
		bool startup(void) override;
		bool shutdown(void) override;
		std::string version(void) override;
		std::string gitCommit(void) override;
		std::string gitBranch(void) override;
		//- ApplicationIf -----------------------------------------------------

	  private:
		CameraAnimation cameraAnimation_;
		TestFolderLib testFolderLib_;
		TestStatusCode testStatusCode_;
		Function function_;
		ServiceFunction serviceFunction_;
		Event event_;
		Alarm alarm_;
		Authentication authentication_;
		HistoricalAccess historicalAccess_;
		Discovery discovery_;
		CreateDeleteNode createDeleteNode_;
		VariableType variableType_;
		ObjectType objectType_;
	};

}

#endif
