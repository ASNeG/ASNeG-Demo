/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

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
#include "ASNeG-Demo/Library/ObjectType.h"
#include "OpcUaStackServer/ServiceSetApplication/CreateObjectInstance.h"

namespace OpcUaServerApplicationDemo
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// MyFileType
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	MyFileType::MyFileType(void)
	: FileType()
	{
	}

	MyFileType::~MyFileType(void)
	{
	}

	void
	MyFileType::create(void)
	{
	}

	void
	MyFileType::cleanup(void)
	{
	}

    void
	MyFileType::call_Close_Method(ApplicationMethodContext* applicationMethodContext)
    {
    	std::cout << "MyFileType::call_Close_Method" << std::endl;
    }

    void
	MyFileType::call_GetPosition_Method(ApplicationMethodContext* applicationMethodContext)
    {
    	std::cout << "MyFileType::call_GetPosition_Method" << std::endl;
    }

    void
	MyFileType::call_Open_Method(ApplicationMethodContext* applicationMethodContext)
    {
    	std::cout << "MyFileType::call_Open_Method" << std::endl;
    }

    void
	MyFileType::call_Read_Method(ApplicationMethodContext* applicationMethodContext)
    {
    	std::cout << "MyFileType::call_Read_Method" << std::endl;
    }

    void
	MyFileType::call_SetPosition_Method(ApplicationMethodContext* applicationMethodContext)
    {
    	std::cout << "MyFileType::call_SetPosition_Method" << std::endl;
    }

    void
	MyFileType::call_Write_Method(ApplicationMethodContext* applicationMethodContext)
    {
    	std::cout << "MyFileType::call_Write_Method" << std::endl;
    }

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ObjectType
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ObjectType::ObjectType(void)
	: ioThread_(nullptr)
	, applicationServiceIf_(nullptr)
	, applicationInfo_(nullptr)

	, fileType_()
	{
	}

	ObjectType::~ObjectType(void)
	{
	}

	bool
	ObjectType::startup(
		IOThread& ioThread,
		ApplicationServiceIf& applicationServiceIf,
		ApplicationInfo* applicationInfo
	)
	{
		Log(Debug, "ObjectType::startup");

		ioThread_ = &ioThread;
		applicationServiceIf_ = &applicationServiceIf;
		applicationInfo_ = applicationInfo;

		// create object instance
		if (!createObject()) {
			Log(Error, "create object error");
			return false;
		}

		return true;
	}

	bool
	ObjectType::shutdown(void)
	{
		Log(Debug, "ObjectType::shutdown");

		return true;
	}

	bool
	ObjectType::createObject(void)
	{
		Log(Debug, "Create object type");

		//
		// create a new object instance in the opc ua information model
		//
		fileType_ = boost::make_shared<MyFileType>();
		Object::SPtr obj = fileType_;
		CreateObjectInstance createObjectInstance(
			"http://ASNeG-Demo/ObjectType/",				// namespace name of the object instance
			OpcUaLocalizedText("", "MyFileType"),			// display name of the object instance
			OpcUaNodeId(85),								// parent node of the object instance
			OpcUaNodeId(35),								// reference type between object and variable instance
			obj
		);

		if (!createObjectInstance.query(applicationServiceIf_)) {
			Log(Error, "create object response error");
			return false;
		}

		// FIXME: todo

		Log(Debug, "Create object type done");
		return true;
	}


}
