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

#ifndef __MyComplexType_h__
#define __MyComplexType_h__

#include "OpcUaStackClient/ValueBasedInterface/VBIClient.h"
#include "OpcUaStackCore/Base/ObjectPool.h"
#include "OpcUaStackCore/BuildInTypes/BuildInTypes.h"

using namespace OpcUaStackClient;
using namespace OpcUaStackCore;

class MyComplexType
: public Object
, public ExtensionObjectBase
{
  public:
	typedef boost::shared_ptr<MyComplexType> SPtr;

	MyComplexType(void)
	: Object()
	, variable1_(0.0)
	, variable2_(0.0)
	, variable3_()
	{}
	virtual ~MyComplexType(void) {}

	OpcUaDouble variable1_;
	OpcUaDouble variable2_;
	OpcUaByteString variable3_;

	void copyTo(MyComplexType& myComplexType)
	{
		myComplexType.variable1_ = variable1_;
		myComplexType.variable2_ = variable2_;
		variable3_.copyTo(myComplexType.variable3_);
	}
	bool operator==(const MyComplexType& myComplexType) const
	{
		MyComplexType* dst = const_cast<MyComplexType*>(&myComplexType);
		return
			variable1_ == dst->variable1_ &&
			variable2_ == dst->variable2_ &&
			variable3_ == dst->variable3_;
	}

	//- ExtensionObjectBase -----------------------------------------------
	ExtensionObjectBase::SPtr factory(void)
	{
		return constructSPtr<MyComplexType>();
	}

	void opcUaBinaryEncode(std::ostream& os) const
	{
		OpcUaNumber::opcUaBinaryEncode(os, variable1_);
		OpcUaNumber::opcUaBinaryEncode(os, variable2_);
		variable3_.opcUaBinaryEncode(os);
	}

	void opcUaBinaryDecode(std::istream& is)
	{
		OpcUaNumber::opcUaBinaryDecode(is, variable1_);
		OpcUaNumber::opcUaBinaryDecode(is, variable2_);
		variable3_.opcUaBinaryDecode(is);
	}

	void copyTo(ExtensionObjectBase& extensionObjectBase)
	{
		MyComplexType* dst = dynamic_cast<MyComplexType*>(&extensionObjectBase);
		copyTo(*dst);
	}

	bool equal(ExtensionObjectBase& extensionObjectBase) const
	{
		MyComplexType* dst = dynamic_cast<MyComplexType*>(&extensionObjectBase);
		return *this == *dst;
	}

	void out(std::ostream& os)
	{
		os << "Variable1=" << (double)variable1_;
		os << ", Variable2=" << (double)variable2_;
		os << ", Variable3="; variable3_.out(os);
	}
	//- ExtensionObjectBase -----------------------------------------------

};

#endif
