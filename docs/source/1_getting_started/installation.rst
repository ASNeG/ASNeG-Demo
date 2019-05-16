Installation
====================

ASNeG Demo Server provides different ways of installation.


Source Code
--------------

In order to compile and install the ASNeG Demo Server from the source code, you should meet
the following requirements:

* OpcUaStack >= 4.0.0


**Linux (Ubuntu or Debian)**

To install the requirments, follow the instructions in the following documents.

* `Installation OpcUaStack <https://opcuastack.readthedocs.io/en/release4/1_getting_started/installation.html>`_. 

The next stept is the compilation of the source code and its installation. You should 
run in the root directory of the sources:

::

  $ sh build.sh -t local -s ~/.ASNeG

Now the ASNeG Demo Server can be started.

::
  
  $ export PATH=$PATH:~/.ASNeG/usr/bin
  $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.ASNeG/usr/lib/
  $ OpcUaServer4 ~/.ASNeG/etc/OpcUaStack/ASNeG-Demo/OpcUaServer.xml

**Windows**

todo


DEP Packages
--------------

todo


RPM Packages
-------------

todo


MSI Packages
--------------

todo


Docker
-----------

todo
