#
# Makefile
#
# $Id: //poco/1.4/Net/samples/neotracsrvtcp/Makefile#1 $
#
# Makefile for Poco neotracsrvtcp
#

include $(POCO_BASE)/build/rules/global

objects = neotracsrvtcp

target         = neotracsrvtcp
target_version = 1
target_libs    = PocoUtil PocoJSON PocoNet PocoXML PocoFoundation

include $(POCO_BASE)/build/rules/exec

ifdef POCO_UNBUNDLED
        SYSLIBS += -lz -lpcre -lexpat
endif

