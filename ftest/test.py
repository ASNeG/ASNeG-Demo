#!/usr/local/bin/python

import os, sys
import binascii
from opcua.crypto import uacrypto

#nonce1 = binascii.unhexlify("01010101010101010101010101010101")
#nonce2 = binascii.unhexlify("02020202020202020202020202020202")
#
#keySize = (16, 16, 16)
#
#(sigkey, key, init_vec) = uacrypto.p_sha1(nonce1, nonce2, keySize)
#
#print(":".join("{:02x}".format(c) for c in nonce1))
#print(":".join("{:02x}".format(c) for c in nonce2))
#
#
#print(":".join("{:02x}".format(c) for c in sigkey))
#print(":".join("{:02x}".format(c) for c in key))
#print(":".join("{:02x}".format(c) for c in init_vec))


nonce1 = binascii.unhexlify("0101010101010101010101010101010101010101010101010101010101010101")
nonce2 = binascii.unhexlify("0202020202020202020202020202020202020202020202020202020202020202")

keySize = (24, 32, 16)

(sigkey, key, init_vec) = uacrypto.p_sha1(nonce1, nonce2, keySize)

#print(":".join("{:02x}".format(c) for c in nonce1))
#print(":".join("{:02x}".format(c) for c in nonce2))


print(":".join("{:02x}".format(c) for c in sigkey))
print(":".join("{:02x}".format(c) for c in key))
print(":".join("{:02x}".format(c) for c in init_vec))
