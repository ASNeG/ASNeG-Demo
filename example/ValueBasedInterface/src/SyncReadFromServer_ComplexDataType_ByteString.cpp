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

#define REAL_SERVER_URI "opc.tcp://127.0.0.1:8889"
#define REAL_SESSION_NAME "urn:127.0.0.1:ASNeG.de:ASNeG-Client"

#include "OpcUaStackClient/ValueBasedInterface/VBIClient.h"
#include "MyComplexType.h"

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

	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
	//
	// register complex type
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void registerComplexTypes(void)
	{
		OpcUaExtensionObject eo;
		eo.registerFactoryElement<MyComplexType>(3002, 1);
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

		OpcUaExtensionObject::SPtr extObject = dataValue.variant()->variantSPtr<OpcUaExtensionObject>();
		MyComplexType::SPtr complexValue = extObject->parameter<MyComplexType>(OpcUaNodeId(3002,1));
		out(dataValue);

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

	// register complex type
	client.registerComplexTypes();

	// connect to the opc ua server
	if (!client.connectToServer()) return 0;

	// read data from opc ua server
	if (!client.readFromServer()) return 0;

	// disconnect from the opc ua server
	if (!client.disconnectFromServer()) return 0;

	return 1;
}

