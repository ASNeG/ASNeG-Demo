#!/usr/local/bion/python
import sys, os
import unittest

from opcua import Client

class TestStringMethods(unittest.TestCase):

    def test_get_endpointr(self):
        client = Client("opc.tcp://127.0.0.1:8889")
        client.connect()
        client.disconnect()

if __name__ == '__main__':
    unittest.main()

