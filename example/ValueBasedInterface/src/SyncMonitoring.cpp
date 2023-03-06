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
	uint32_t subscriptionId;
	uint32_t monitoredItemId;

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
	// data change callback
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void dataChangeCallback(OpcUaUInt32 clientHandle, OpcUaDataValue& dataValue)
	{
		out(dataValue);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// start monitoring
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool startMonitoring(void)
	{
		OpcUaStatusCode statusCode;

		// set data change callback
		client.setDataChangeHandler(
			boost::bind(&ExampleClient::dataChangeCallback, this, _1, _2)
		);

		// create subscription
		statusCode = client.syncCreateSubscription(subscriptionId);
		if (statusCode != Success) {
			std::cout << std::endl << "**** create subscription error ****" << std::endl;
			return false;
		}

		// create monitored item
		OpcUaNodeId nodeId(216,3);
		statusCode = client.syncCreateMonitoredItem(nodeId, subscriptionId, 4711, monitoredItemId);
		if (statusCode != Success) {
			std::cout << std::endl << "**** create monitored item error ****" << std::endl;
			return false;
		}

		std::cout << std::endl << "**** start monitored item success ****" << std::endl;
		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// start monitoring
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool stopMonitoring(void)
	{
		OpcUaStatusCode statusCode;

		statusCode = client.syncDeleteMonitoredItem(subscriptionId, monitoredItemId);
		if (statusCode != Success) {
			std::cout << std::endl << "**** delete monitored item error ****" << std::endl;
			return false;
		}

		statusCode = client.syncDeleteSubscription(subscriptionId);
		if (statusCode != Success) {
			std::cout << std::endl << "**** delete subscription error ****" << std::endl;
			return false;
		}

		std::cout << std::endl << "**** stop monitored item success ****" << std::endl;
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

	// start monitoring
	if (!client.startMonitoring()) return 0;

	sleep(1000000);

	// stop monitoring
	if (!client.stopMonitoring()) return 0;

	// disconnect from the opc ua server
	if (!client.disconnectFromServer()) return 0;

	return 1;
}

