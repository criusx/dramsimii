; Listing generated by Microsoft (R) Optimizing Compiler Version 15.00.30729.01 

	TITLE	c:\Users\CriuS\Documents\Visual Studio 2008\Projects\DRAMsimIIhg\src\simulationParameters.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB OLDNAMES

PUBLIC	??_C@_08CPHEIFNE@?$FN?5Input?$FL?$AA@		; `string'
PUBLIC	??_C@_09LLKHONOO@ReqCount?$FL?$AA@		; `string'
;	COMDAT ??_C@_09LLKHONOO@ReqCount?$FL?$AA@
CONST	SEGMENT
??_C@_09LLKHONOO@ReqCount?$FL?$AA@ DB 'ReqCount[', 00H	; `string'
CONST	ENDS
;	COMDAT ??_C@_08CPHEIFNE@?$FN?5Input?$FL?$AA@
CONST	SEGMENT
??_C@_08CPHEIFNE@?$FN?5Input?$FL?$AA@ DB '] Input[', 00H ; `string'
__bad_alloc_Message DD FLAT:??_C@_0P@GHFPNOJB@bad?5allocation?$AA@
PUBLIC	??8SimulationParameters@DRAMsimII@@QBE_NABV01@@Z ; DRAMsimII::SimulationParameters::operator==
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\simulationparameters.cpp
_TEXT	SEGMENT
??8SimulationParameters@DRAMsimII@@QBE_NABV01@@Z PROC	; DRAMsimII::SimulationParameters::operator==
; _this$ = ecx
; _right$ = eax

; 34   : 	return requestCount == right.requestCount && inputType == right.inputType;

	mov	edx, DWORD PTR [ecx]
	cmp	edx, DWORD PTR [eax]
	jne	SHORT $LN3@operator@11
	mov	edx, DWORD PTR [ecx+4]
	cmp	edx, DWORD PTR [eax+4]
	jne	SHORT $LN3@operator@11
	mov	ecx, DWORD PTR [ecx+8]
	cmp	ecx, DWORD PTR [eax+8]
	jne	SHORT $LN3@operator@11
	mov	eax, 1

; 35   : }

	ret	0
$LN3@operator@11:

; 34   : 	return requestCount == right.requestCount && inputType == right.inputType;

	xor	eax, eax

; 35   : }

	ret	0
??8SimulationParameters@DRAMsimII@@QBE_NABV01@@Z ENDP	; DRAMsimII::SimulationParameters::operator==
_TEXT	ENDS
PUBLIC	??0SimulationParameters@DRAMsimII@@QAE@ABVSettings@1@@Z ; DRAMsimII::SimulationParameters::SimulationParameters
; Function compile flags: /Ogtpy
_TEXT	SEGMENT
??0SimulationParameters@DRAMsimII@@QAE@ABVSettings@1@@Z PROC ; DRAMsimII::SimulationParameters::SimulationParameters
; _this$ = eax
; _settings$ = ecx

; 24   : {}

	mov	edx, DWORD PTR [ecx+184]
	mov	DWORD PTR [eax], edx
	mov	ecx, DWORD PTR [ecx+188]
	mov	DWORD PTR [eax+4], ecx
	mov	DWORD PTR [eax+8], 2
	ret	0
??0SimulationParameters@DRAMsimII@@QAE@ABVSettings@1@@Z ENDP ; DRAMsimII::SimulationParameters::SimulationParameters
PUBLIC	??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVSimulationParameters@0@@Z ; DRAMsimII::operator<<
; Function compile flags: /Ogtpy
_TEXT	SEGMENT
_in$ = 8						; size = 4
??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVSimulationParameters@0@@Z PROC ; DRAMsimII::operator<<
; _params$ = edi

; 28   : 	in << "ReqCount[" << params.requestCount << "] Input[" << params.inputType << "]" << std::endl;

	mov	eax, DWORD PTR [edi+4]
	mov	ecx, DWORD PTR [edi]
	push	ebx
	mov	ebx, DWORD PTR _in$[esp]
	push	esi
	push	OFFSET ??_C@_01LBDDMOBJ@?$FN?$AA@
	push	OFFSET ??_C@_08CPHEIFNE@?$FN?5Input?$FL?$AA@
	push	eax
	push	ecx
	push	OFFSET ??_C@_09LLKHONOO@ReqCount?$FL?$AA@
	push	ebx
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	push	eax
	call	??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@_J@Z ; std::basic_ostream<char,std::char_traits<char> >::operator<<
	push	eax
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	mov	esi, eax
	mov	eax, DWORD PTR [edi+8]
	add	esp, 8
	call	??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@W4InputType@InputStream@0@@Z ; DRAMsimII::operator<<
	push	eax
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	mov	esi, eax
	push	10					; 0000000aH
	mov	ecx, esi
	call	?put@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@D@Z ; std::basic_ostream<char,std::char_traits<char> >::put
	mov	ecx, esi
	call	?flush@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@XZ ; std::basic_ostream<char,std::char_traits<char> >::flush
	pop	esi

; 29   : 	return in;

	mov	eax, ebx
	pop	ebx

; 30   : }

	ret	0
??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVSimulationParameters@0@@Z ENDP ; DRAMsimII::operator<<
END
