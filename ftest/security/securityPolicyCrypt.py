#!/usr/local/bion/python
import sys, os
import unittest

from opcua import Client

#import logging
#logging.basicConfig(format="%(levelname)s: %(message)s", level="DEBUG")

class TestStringMethods(unittest.TestCase):

    def test_basic128Rsa15(self):
        client = Client("opc.tcp://127.0.0.1:8889")
        client.set_security_string("Basic128Rsa15,SignAndEncrypt,certificate.der,private-key.pem")
        client.connect()
        client.disconnect()


if __name__ == '__main__':
    unittest.main()

