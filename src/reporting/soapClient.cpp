/* soapClient.cpp
   Generated by gSOAP 2.7.11 from reporting.h
   Copyright(C) 2000-2008, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapClient.cpp ver 2.7.11 2008-09-12 02:01:59 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__getSessionID(struct soap *soap, const char *soap_endpoint, const char *soap_action, _ns2__getSessionIDElement *ns2__getSessionIDElement, _ns2__getSessionIDResponseElement *ns2__getSessionIDResponseElement)
{	struct __ns1__getSessionID soap_tmp___ns1__getSessionID;
	if (!soap_endpoint)
		soap_endpoint = "http://id2.gentag.com:7777/DRAMsim/DRAMsimWSSoapHttpPort";
	if (!soap_action)
		soap_action = "http://DRAMsimReporter//getSessionID";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__getSessionID.ns2__getSessionIDElement = ns2__getSessionIDElement;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__getSessionID(soap, &soap_tmp___ns1__getSessionID);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__getSessionID(soap, &soap_tmp___ns1__getSessionID, "-ns1:getSessionID", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__getSessionID(soap, &soap_tmp___ns1__getSessionID, "-ns1:getSessionID", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns2__getSessionIDResponseElement)
		return soap_closesock(soap);
	ns2__getSessionIDResponseElement->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns2__getSessionIDResponseElement->soap_get(soap, "ns2:getSessionIDResponseElement", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__getSettingsFile(struct soap *soap, const char *soap_endpoint, const char *soap_action, _ns2__getSettingsFileElement *ns2__getSettingsFileElement, _ns2__getSettingsFileResponseElement *ns2__getSettingsFileResponseElement)
{	struct __ns1__getSettingsFile soap_tmp___ns1__getSettingsFile;
	if (!soap_endpoint)
		soap_endpoint = "http://id2.gentag.com:7777/DRAMsim/DRAMsimWSSoapHttpPort";
	if (!soap_action)
		soap_action = "http://DRAMsimReporter//getSettingsFile";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__getSettingsFile.ns2__getSettingsFileElement = ns2__getSettingsFileElement;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__getSettingsFile(soap, &soap_tmp___ns1__getSettingsFile);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__getSettingsFile(soap, &soap_tmp___ns1__getSettingsFile, "-ns1:getSettingsFile", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__getSettingsFile(soap, &soap_tmp___ns1__getSettingsFile, "-ns1:getSettingsFile", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns2__getSettingsFileResponseElement)
		return soap_closesock(soap);
	ns2__getSettingsFileResponseElement->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns2__getSettingsFileResponseElement->soap_get(soap, "ns2:getSettingsFileResponseElement", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__submitEpochResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, _ns2__submitEpochResultElement *ns2__submitEpochResultElement, _ns2__submitEpochResultResponseElement *ns2__submitEpochResultResponseElement)
{	struct __ns1__submitEpochResult soap_tmp___ns1__submitEpochResult;
	if (!soap_endpoint)
		soap_endpoint = "http://id2.gentag.com:7777/DRAMsim/DRAMsimWSSoapHttpPort";
	if (!soap_action)
		soap_action = "http://DRAMsimReporter//submitEpochResult";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__submitEpochResult.ns2__submitEpochResultElement = ns2__submitEpochResultElement;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__submitEpochResult(soap, &soap_tmp___ns1__submitEpochResult);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__submitEpochResult(soap, &soap_tmp___ns1__submitEpochResult, "-ns1:submitEpochResult", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__submitEpochResult(soap, &soap_tmp___ns1__submitEpochResult, "-ns1:submitEpochResult", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns2__submitEpochResultResponseElement)
		return soap_closesock(soap);
	ns2__submitEpochResultResponseElement->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns2__submitEpochResultResponseElement->soap_get(soap, "ns2:submitEpochResultResponseElement", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

/* End of soapClient.cpp */
