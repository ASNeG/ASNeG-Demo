K��b             8 h t t p : / / o p c f o u n d a t i o n . o r g / U A /   V h t t p : / / y o u r o r g a n i s a t i o n . o r g / N o d e S e t E x a m p l e /       )          �    MyObjectType����   MyObjectType    ��������                       �    MethodObjectType       MethodObjectType����                        @       �    EnumSimpleType����   EnumSimpleType    ��������                    EnumSimpleType   EnumSimpleType      Enum1��������   Enum1       Enum2��������   Enum2      Enum3��������   Enum3      @       �    EnumGapType����   EnumGapType    ��������                    EnumGapType   EnumGapType      Enum1��������   Enum1����   Enum2��������   Enum2      Enum3��������   Enum3      @       �    MyStructureType����   MyStructureType    ��������                     MyStructureType   MyStructureType      Element1����   todo   Element1         ����     Element2��������   Element2         ����     Element3��������   Element3         ����          @       �    MyComplexStructureType����   MyComplexStructureType    ��������                     MyComplexStructureType   MyComplexStructureType      Element1��������   Element1         ����     Element2��������   Element2       �����     Element3��������   Element3       �����                  �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       �    
BaseObject����   
BaseObject   
BaseObject��������                       �    
BaseObject����   
BaseObject   
BaseObject��������                       �    MyObjectObject����   MyObjectObject    ��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       �    MethodObjectValue       MethodObjectValue����                               q     EnumStrings����   EnumStrings    ��������                                       ����   Enum1����   Enum2����   Enum3                   r     
EnumValues����   
EnumValues    ��������                    �                          �   ������������   Enum1��������        �          ����   Enum2��������        �          ����   Enum3��������                   s    TypeDictionary����   TypeDictionary    ����   RCollects the data type descriptions of http://yourorganisation.org/NodeSetExample/                     ����                 <opc:TypeDictionary xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tns="http://yourorganisation.org/NodeSetExample/" DefaultByteOrder="LittleEndian" xmlns:opc="http://opcfoundation.org/BinarySchema/" xmlns:ua="http://opcfoundation.org/UA/" TargetNamespace="http://yourorganisation.org/NodeSetExample/">
 <opc:Import Namespace="http://opcfoundation.org/UA/"/>
 <opc:StructuredType BaseType="ua:ExtensionObject" Name="MyComplexStructureType">
  <opc:Field TypeName="opc:Double" Name="Element1"/>
  <opc:Field TypeName="tns:MyStructureType" Name="Element2"/>
  <opc:Field TypeName="tns:EnumSimpleType" Name="Element3"/>
 </opc:StructuredType>
 <opc:StructuredType BaseType="ua:ExtensionObject" Name="MyStructureType">
  <opc:Field TypeName="opc:Bit" Name="Element3Specified"/>
  <opc:Field Length="31" TypeName="opc:Bit" Name="Reserved1"/>
  <opc:Field TypeName="opc:Double" Name="Element1"/>
  <opc:Field TypeName="opc:Double" Name="Element2"/>
  <opc:Field SwitchField="Element3Specified" TypeName="opc:Double" Name="Element3"/>
 </opc:StructuredType>
 <opc:EnumeratedType LengthInBits="32" Name="EnumGapType">
  <opc:EnumeratedValue Name="Enum1" Value="-1"/>
  <opc:EnumeratedValue Name="Enum2" Value="3"/>
  <opc:EnumeratedValue Name="Enum3" Value="5"/>
 </opc:EnumeratedType>
 <opc:EnumeratedType LengthInBits="32" Name="EnumSimpleType">
  <opc:EnumeratedValue Name="Enum1" Value="0"/>
  <opc:EnumeratedValue Name="Enum2" Value="1"/>
  <opc:EnumeratedValue Name="Enum3" Value="2"/>
 </opc:EnumeratedType>
</opc:TypeDictionary>
                   t    EnumGapValue����   EnumGapValue    ��������                   �����               ����                   u    EnumSimpleValue����   EnumSimpleValue    ��������                   �����                                      v     NamespaceUri����   NamespaceUri    ��������                     ����              
    V h t t p : / / y o u r o r g a n i s a t i o n . o r g / N o d e S e t E x a m p l e /                  w    TypeDictionary����   TypeDictionary    ����   RCollects the data type descriptions of http://yourorganisation.org/NodeSetExample/                     ����                 D<xs:schema elementFormDefault="qualified" targetNamespace="http://yourorganisation.org/NodeSetExample/Types.xsd" xmlns:tns="http://yourorganisation.org/NodeSetExample/Types.xsd" xmlns:ua="http://opcfoundation.org/UA/2008/02/Types.xsd" xmlns:xs="http://www.w3.org/2001/XMLSchema">
 <xs:import namespace="http://opcfoundation.org/UA/2008/02/Types.xsd"/>
 <xs:simpleType name="EnumGapType">
  <xs:restriction base="xs:string">
   <xs:enumeration value="Enum1_-1"/>
   <xs:enumeration value="Enum2_3"/>
   <xs:enumeration value="Enum3_5"/>
  </xs:restriction>
 </xs:simpleType>
 <xs:element type="tns:EnumGapType" name="EnumGapType"/>
 <xs:complexType name="ListOfEnumGapType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:EnumGapType" name="EnumGapType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfEnumGapType" name="ListOfEnumGapType" nillable="true"/>
 <xs:simpleType name="EnumSimpleType">
  <xs:restriction base="xs:string">
   <xs:enumeration value="Enum1_0"/>
   <xs:enumeration value="Enum2_1"/>
   <xs:enumeration value="Enum3_2"/>
  </xs:restriction>
 </xs:simpleType>
 <xs:element type="tns:EnumSimpleType" name="EnumSimpleType"/>
 <xs:complexType name="ListOfEnumSimpleType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:EnumSimpleType" name="EnumSimpleType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfEnumSimpleType" name="ListOfEnumSimpleType" nillable="true"/>
 <xs:complexType name="MyComplexStructureType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="Element1"/>
   <xs:element minOccurs="0" maxOccurs="1" type="tns:MyStructureType" name="Element2"/>
   <xs:element minOccurs="0" maxOccurs="1" type="tns:EnumSimpleType" name="Element3"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:MyComplexStructureType" name="MyComplexStructureType"/>
 <xs:complexType name="ListOfMyComplexStructureType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:MyComplexStructureType" name="MyComplexStructureType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfMyComplexStructureType" name="ListOfMyComplexStructureType" nillable="true"/>
 <xs:complexType name="MyStructureType">
  <xs:sequence>
   <xs:element minOccurs="0" type="xs:unsignedInt" name="EncodingMask"/>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="Element1"/>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="Element2"/>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="Element3"/>
  </xs:sequence>
  <xs:attribute name="EncodingTable" fixed="Element3:0"/>
 </xs:complexType>
 <xs:element type="tns:MyStructureType" name="MyStructureType"/>
 <xs:complexType name="ListOfMyStructureType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:MyStructureType" name="MyStructureType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfMyStructureType" name="ListOfMyStructureType" nillable="true"/>
</xs:schema>
                   x     NamespaceUri����   NamespaceUri    ��������                     ����              
    V h t t p : / / y o u r o r g a n i s a t i o n . o r g / N o d e S e t E x a m p l e /                  y    MyStructureType����   MyStructureType    ��������                     ����              
     M y S t r u c t u r e T y p e                  z    MyStructureType����   MyStructureType    ��������                     ����              
    J / / x s : e l e m e n t [ @ n a m e = ' M y S t r u c t u r e T y p e ' ]                  {    Value1����   Value1   Value1��������                     ����                        |    MyStructureValue����   MyStructureValue    ��������                   �����                  �   	                                                 }    Value2����   Value2   Value2��������                     ����                        ~    Value1����   Value1   Value1��������                     ����                            Value2����   Value2   Value2��������                     ����                        �    MyComplexStructureType����   MyComplexStructureType    ��������                     ����              
    , M y C o m p l e x S t r u c t u r e T y p e                   �    MyComplexStructureType����   MyComplexStructureType    ��������                     ����              
    X / / x s : e l e m e n t [ @ n a m e = ' M y C o m p l e x S t r u c t u r e T y p e ' ]                   �     InputArguments����   InputArguments������������                    (                          (      P1��������         ����            (      P2��������         ����                       �    MyComplexStructureValue����   MyComplexStructureValue    ��������                   �����                  �   	   (?�         @       @      @                            �     OutputArguments����   OutputArguments������������                    (                          (      P1��������         ����            (      P2��������         ����                       �     InputArguments����   InputArguments������������                    (                          (      P1��������         ����            (      P2��������         ����                       �     OutputArguments����   OutputArguments������������                    (                          (      P1��������         ����            (      P2��������         ����                       Y    Method1����   Method1   Method1��������                      Z    Method2����   Method2   Method2��������                      [    Method1����   Method1   Method1��������                      \    Method2����   Method2   Method2��������                       F o l d e r    Folder����   Folder    ��������                U       �       �         /         :       �         -       �       {         /       �       }         /       �       Y         /       �       Z         /         :       �         -                �         -       �       q         .                �         -       �       r         .       �       �         &       �       �         &                �         -       �       �         &       �       �         &                �         -       �         L         (       �       y         '       �         L         (       �       z         '       �         :         (       �         P         %       �         :         (       �       �         /       �       �         (       �       ~         /       �                /       �         L         (       �       �         '       �         L         (       �       �         '       �       [         /       �       \         /       �       �         (       q         N         %       q         D         (       r         N         %       r         D         (       s         H         (       s       �         /       s       y         /       s       v         .         ]       s         /       t         ?         (       u         ?         (       v         D         (       w         H         (       w       �         /       w       z         /       w       x         .         \       w         /       x         D         (       y         E         (       z         E         (       {         ?         (       {         N         %       |         ?         (       }         ?         (       }         N         %       ~         ?         (                ?         (       �         E         (       �         E         (       �         N         %       �         D         (       �         ?         (       �         N         %       �         D         (       �         D         (       �         D         (       Y         N         %       Z       �         .       Z         N         %       Z       �         .       \       �         .       \       �         .        F o l d e r       t         #        F o l d e r       u         #         U        F o l d e r         #        F o l d e r         =         (        F o l d e r       �         #        F o l d e r       �         #        F o l d e r       �         #        F o l d e r       |         #          �       �           �       �           �       �          q       �           r       �           v       s           v        �          x       w           x         k          y       s           z       w           {       �           }       �           ~       �           ~       {                 �                  }          �       s           �       w           �       Z           �       Z           �       \           �       �          �       \           �       �          Y       �           Z       �           [       �           [       Y          \       �           \       Z                                    	Kai Hübl   	Kai Hübl %��e�|�    