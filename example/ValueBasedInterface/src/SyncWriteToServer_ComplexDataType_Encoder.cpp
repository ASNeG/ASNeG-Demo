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
	// Encode DataValue
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void encodeDataValue(
		OpcUaDouble variable1,						// in
		OpcUaDouble variable2,						// in
		OpcUaByteString& variable3,					// in
		OpcUaExtensionObject::SPtr& complexValue 	// out
	)
	{
		std::stringstream ss;
		OpcUaNumber::opcUaBinaryEncode(ss, (OpcUaDouble)variable1);
		OpcUaNumber::opcUaBinaryEncode(ss, (OpcUaDouble)variable2);
		variable3.opcUaBinaryEncode(ss);

		OpcUaByteString::SPtr byteString = boost::make_shared<OpcUaByteString>();
		byteString->value(ss.str().c_str(), ss.str().length());
		complexValue->typeId(OpcUaNodeId(3002,1));
		complexValue->byteString(byteString);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// write some data values to the opc ua server
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool writeToServer(void)
	{
		OpcUaDataValue dataValue;
		OpcUaStatusCode statusCode;
		OpcUaNodeId nodeId;

		boost::posix_time::ptime ptime1 = boost::posix_time::from_iso_string("20140506T102013.123456789");
		boost::posix_time::ptime ptime2 = boost::posix_time::from_iso_string("20140506T102014.123456789");
		OpcUaDateTime sourceTimestamp, serverTimestamp;

		//
		// write complex data type
		//
		OpcUaDouble variable1 = 1.1;
		OpcUaDouble variable2 = 2.2;
		OpcUaByteString variable3;
		variable3.value("Dies ist ein String");
		OpcUaExtensionObject::SPtr complexValue = boost::make_shared<OpcUaExtensionObject>();
		encodeDataValue(variable1, variable2, variable3, complexValue);

		dataValue.variant()->variant(complexValue);
		dataValue.statusCode((OpcUaStatusCode)Success);
		dataValue.sourceTimestamp(sourceTimestamp);
		dataValue.serverTimestamp(serverTimestamp);

		nodeId.set(std::string("ComplexVariable"), 6);
		statusCode = client.syncWrite(nodeId, dataValue);
		if (statusCode != Success) {
			std::cout << std::endl << "**** write to opc ua server error (Double) ****" << std::endl;
			return false;
		}
		out(dataValue);

		std::cout << std::endl << "**** write to opc ua server success ****" << std::endl;
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

	// write data to opc ua server
	if (!client.writeToServer()) return 0;

	// disconnect from the opc ua server
	if (!client.disconnectFromServer()) return 0;

	return 1;
}

