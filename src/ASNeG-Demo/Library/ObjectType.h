/*
   Copyright 2019-2020 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaServerApplicationDemo_ObjectType_h__
#define __OpcUaServerApplicationDemo_ObjectType_h__

#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Application/ApplicationMethodContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaStackServer/AddressSpaceModel/BaseNodeClass.h"
#include "OpcUaStackServer/StandardObjectType/FileType.h"

namespace OpcUaServerApplicationDemo
{

	class MyFileType
	: public OpcUaStackServer::FileType
	{
	  public:
		MyFileType(void);
		virtual ~MyFileType(void);

		void create(void);
		void cleanup(void);

	    virtual void call_Close_Method(
	    	OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
	    virtual void call_GetPosition_Method(
	    	OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
	    virtual void call_Open_Method(
	    	OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
	    virtual void call_Read_Method(
	    	OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
	    virtual void call_SetPosition_Method(
	    	OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
	    virtual void call_Write_Method(
	    	OpcUaStackCore::ApplicationMethodContext* applicationMethodContext
		);
	};

	class ObjectType
	{
	  public:
		ObjectType(void);
		~ObjectType(void);

		bool startup(
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackServer::ApplicationServiceIf& applicationServiceIf,
			OpcUaStackServer::ApplicationInfo* applicationInfo
		);
		bool shutdown(void);

	  private:
		bool createObject(void);

		OpcUaStackCore::IOThread::SPtr ioThread_;
		OpcUaStackServer::ApplicationServiceIf* applicationServiceIf_;
		OpcUaStackServer::ApplicationInfo* applicationInfo_;

		MyFileType::SPtr fileType_;
	};

}

#endif
