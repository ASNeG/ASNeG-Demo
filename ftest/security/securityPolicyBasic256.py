#!/usr/local/bion/python
import sys, os
import unittest

from opcua import Client

#import logging
#logging.basicConfig(format="%(levelname)s: %(message)s", level="DEBUG")

class TestStringMethods(unittest.TestCase):

    def test_basic256(self):
        client = Client("opc.tcp://" + os.environ['TEST_IP'] + ":" + os.environ['TEST_PORT'])
        client.application_uri = "urn:127.0.0.1:ASNeG:FTestClient"
        client.set_security_string("Basic256,SignAndEncrypt,FTestClient.der,FTestClient.pem")
        client.connect()
        client.disconnect()




if __name__ == '__main__':
    unittest.main()

