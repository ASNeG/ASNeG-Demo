/*
   Copyright 2016-2023 Kai Huebl (kai@huebl-sgh.de)

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

#define REAL_SERVER_URI "opc.tcp://127.0.0.1:8889"
#define REAL_SESSION_NAME "urn:127.0.0.1:ASNeG.de:ASNeG-Client"

#include "OpcUaStackClient/ValueBasedInterface/VBIClient.h"
#include "CryptoManagerTest.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackClient;

class ExampleClient
{
  public:
	VBIClient client;

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// open a secure channel to the opc ua server
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool connectToServer(void)
	{
		OpcUaStatusCode statusCode;
		ConnectContext connectContext;

		//
		// connect to the opc ua server
		//
		connectContext.endpointUrl_ = REAL_SERVER_URI;
		connectContext.sessionName_ = REAL_SESSION_NAME;
		connectContext.cryptoManager_ = CryptoManagerTest::getInstance();

		statusCode = client.syncConnect(connectContext);
		if (statusCode != Success) {
			std::cout << std::endl << "**** connect to opc ua server error ****" << std::endl;
			return false;
		}

		std::cout << std::endl << "**** connect to opc ua server success ****" << std::endl;
		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// close the secure channel to the opc ua server
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool disconnectFromServer(void)
	{
		OpcUaStatusCode statusCode;

		//
		// diconnect from the opc ua server
		//
		statusCode = client.syncDisconnect();
		if (statusCode != Success) {
			std::cout << std::endl << "**** disconnect from opc ua server error ****" << std::endl;
			return false;
		}

		std::cout << std::endl << "**** disconnect from opc ua server success ****" << std::endl;
		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Decoder DataValue
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void decodeDataValue(
		OpcUaExtensionObject::SPtr& complexValue, 	// in
		OpcUaDouble& variable1,						// out
		OpcUaDouble& variable2,						// out
		OpcUaByteString& variable3					// out
	)
	{
		char *buffer;
		int32_t bufferLen;
		std::stringstream ss;
		OpcUaByteString::SPtr byteString = complexValue->byteString();
		byteString->value(&buffer, &bufferLen);
		std::string str(buffer, bufferLen);
		ss << str;

		OpcUaNumber::opcUaBinaryDecode(ss, variable1);
		OpcUaNumber::opcUaBinaryDecode(ss, variable2);
		variable3.opcUaBinaryDecode(ss);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// read complex data type
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool readFromServer(void)
	{
		OpcUaDataValue dataValue;
		OpcUaStatusCode statusCode;
		OpcUaNodeId nodeId;

		//
		// read complex variable
		//
		nodeId.set(std::string("ComplexVariable"), 6);

		statusCode = client.syncRead(nodeId, dataValue);
		if (statusCode != Success) {
			std::cout << std::endl << "**** read from opc ua server error (Double) ****" << std::endl;
			return false;
		}
		out(dataValue);

		// get data variable
		if (dataValue.variant()->variantType() != OpcUaBuildInType_OpcUaExtensionObject) {
			std::cout << std::endl << "**** data value build in type error ****" << std::endl;
			return false;
		}
		OpcUaExtensionObject::SPtr complexVariable;
		complexVariable = dataValue.variant()->variantSPtr<OpcUaExtensionObject>();

		if (complexVariable->typeId() != OpcUaNodeId(3002, 1)) {
			std::cout << std::endl << "**** data value type error ****" << std::endl;
			return false;
		}

		OpcUaDouble variable1;
		OpcUaDouble variable2;
		OpcUaByteString variable3;
		decodeDataValue(complexVariable, variable1, variable2, variable3);

		std::cout << std::endl << "**** variable1: " << (double)variable1 << " ****" << std::endl;
		std::cout << std::endl << "**** variable2: " << (double)variable2 << " ****" << std::endl;
		std::cout << std::endl << "**** variable3: " << variable3.toString() << " ****" << std::endl;

		std::cout << std::endl << "**** read from opc ua server success ****" << std::endl;
		return true;
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// print data value to screen
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void out(OpcUaDataValue& dataValue)
	{
		std::cout << "**** ";
		dataValue.out(std::cout);
		std::cout << " ****" << std::endl;
	}
};


int main(int argc, char**argv)
{
	ExampleClient client;

	// connect to the opc ua server
	if (!client.connectToServer()) return 0;

	// read data from opc ua server
	if (!client.readFromServer()) return 0;

	// disconnect from the opc ua server
	if (!client.disconnectFromServer()) return 0;

	return 1;
}

