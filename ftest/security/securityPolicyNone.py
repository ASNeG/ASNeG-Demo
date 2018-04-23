#!/usr/local/bion/python
import sys, os
import unittest

from opcua import Client

class TestStringMethods(unittest.TestCase):

    def test_get_endpointr(self):
        client = Client("opc.tcp://" + os.environ['TEST_IP'] + ":" + os.environ['TEST_PORT'])
        client.connect()
        endpoints = client.get_endpoints()
        print(endpoints)
        client.disconnect()

if __name__ == '__main__':
    unittest.main()

