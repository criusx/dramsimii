/* soapStub.h
   Generated by gSOAP 2.7.10 from reporting.h
   Copyright(C) 2000-2008, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef soapStub_H
#define soapStub_H
#include "stdsoap2.h"

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not redeclare here */

#endif

#ifndef SOAP_TYPE__ns2__submitEpochResultElement
#define SOAP_TYPE__ns2__submitEpochResultElement (7)
/* ns2:submitEpochResultElement */
class SOAP_CMAC _ns2__submitEpochResultElement
{
public:
	char *sessionID;	/* optional element of type xsd:string */
	LONG64 epoch;	/* required element of type xsd:long */
	int __sizechannel;	/* sequence of elements <channel> */
	int *channel;	/* optional element of type xsd:int */
	int __sizerank;	/* sequence of elements <rank> */
	int *rank;	/* optional element of type xsd:int */
	int __sizePsysACTSTBY;	/* sequence of elements <PsysACTSTBY> */
	float *PsysACTSTBY;	/* optional element of type xsd:float */
	int __sizePsysACT;	/* sequence of elements <PsysACT> */
	float *PsysACT;	/* optional element of type xsd:float */
	float PsysRD;	/* required element of type xsd:float */
	float PsysWR;	/* required element of type xsd:float */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 7; } /* = unique id SOAP_TYPE__ns2__submitEpochResultElement */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns2__submitEpochResultElement() : sessionID(NULL), epoch(0), __sizechannel(0), channel(NULL), __sizerank(0), rank(NULL), __sizePsysACTSTBY(0), PsysACTSTBY(NULL), __sizePsysACT(0), PsysACT(NULL), PsysRD(0), PsysWR(0), soap(NULL) { }
	virtual ~_ns2__submitEpochResultElement() { }
};
#endif

#ifndef SOAP_TYPE__ns2__submitEpochResultResponseElement
#define SOAP_TYPE__ns2__submitEpochResultResponseElement (8)
/* ns2:submitEpochResultResponseElement */
class SOAP_CMAC _ns2__submitEpochResultResponseElement
{
public:
	bool result;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 8; } /* = unique id SOAP_TYPE__ns2__submitEpochResultResponseElement */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns2__submitEpochResultResponseElement() : result((bool)0), soap(NULL) { }
	virtual ~_ns2__submitEpochResultResponseElement() { }
};
#endif

#ifndef SOAP_TYPE___ns1__submitEpochResult
#define SOAP_TYPE___ns1__submitEpochResult (18)
/* Operation wrapper: */
struct __ns1__submitEpochResult
{
public:
	_ns2__submitEpochResultElement *ns2__submitEpochResultElement;	/* optional element of type ns2:submitEpochResultElement */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (19)
/* SOAP Header: */
struct SOAP_ENV__Header
{
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (20)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (22)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
	char *__any;
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (25)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (26)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Typedef Synonyms                                                           *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Service Operations                                                         *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 __ns1__submitEpochResult(struct soap*, _ns2__submitEpochResultElement *ns2__submitEpochResultElement, _ns2__submitEpochResultResponseElement *ns2__submitEpochResultResponseElement);

/******************************************************************************\
 *                                                                            *
 * Stubs                                                                      *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__submitEpochResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, _ns2__submitEpochResultElement *ns2__submitEpochResultElement, _ns2__submitEpochResultResponseElement *ns2__submitEpochResultResponseElement);

/******************************************************************************\
 *                                                                            *
 * Skeletons                                                                  *
 *                                                                            *
\******************************************************************************/

SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___ns1__submitEpochResult(struct soap*);

#endif

/* End of soapStub.h */