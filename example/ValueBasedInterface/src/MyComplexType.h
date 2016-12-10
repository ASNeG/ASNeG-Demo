/*
   Copyright 2016 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef MyComplexType
#define MyComplexType

#include "OpcUaStackClient/ValueBasedInterface/VBIClient.h"

using namespace OpcUaStackClient;

class MyComplexType
: public Object
, public ExtensionObjectBase
{
  public:
	typedef boost::shared_ptr<MyComplexType> SPtr;

	MyComplexType(void);
	virtual ~MyComplexType(void);

	OpcUaDouble variable1_;
	OpcUaDouble variable2_;
	OpcUaByteString variable3_;

	void copyTo(MyComplexType& myComplexType) {}
	bool operator==(const MyComplexType& myComplexType) const {}

	//- ExtensionObjectBase -----------------------------------------------
	ExtensionObjectBase::SPtr factory(void)
	{
		return constructSPtr<MyComplexType>();
	}

	void opcUaBinaryEncode(std::ostream& os) const
	{
		;
	}
	void opcUaBinaryDecode(std::istream& is)
	{
		;
	}

	void copyTo(ExtensionObjectBase& extensionObjectBase)
	{
		;
	}

	bool equal(ExtensionObjectBase& extensionObjectBase) const
	{
		;
	}

	void out(std::ostream& os)
	{
		;
	}
	//- ExtensionObjectBase -----------------------------------------------

};

#endif
