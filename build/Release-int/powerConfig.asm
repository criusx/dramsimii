; Listing generated by Microsoft (R) Optimizing Compiler Version 15.00.30729.01 

	TITLE	c:\Users\CriuS\Documents\Visual Studio 2008\Projects\DRAMsimIIhg\src\powerConfig.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB OLDNAMES

PUBLIC	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z		; DRAMsimII::AlmostEqual<double>
PUBLIC	?put@?$num_put@DV?$ostreambuf_iterator@DU?$char_traits@D@std@@@std@@@std@@QBE?AV?$ostreambuf_iterator@DU?$char_traits@D@std@@@2@V32@AAVios_base@2@DJ@Z ; std::num_put<char,std::ostreambuf_iterator<char,std::char_traits<char> > >::put
PUBLIC	??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z ; std::basic_ostream<char,std::char_traits<char> >::operator<<
PUBLIC	??_C@_04NAJNOOFP@IDD0?$AA@			; `string'
PUBLIC	??_C@_0M@EONNHDNC@PowerConfig?$AA@		; `string'
PUBLIC	??_C@_03NDFLNPIA@VDD?$AA@			; `string'
PUBLIC	??_C@_06HMJDDHMI@VDDmax?$AA@			; `string'
PUBLIC	??_C@_05NHLPHMBD@IDD2p?$AA@			; `string'
;	COMDAT ??_C@_05NHLPHMBD@IDD2p?$AA@
CONST	SEGMENT
??_C@_05NHLPHMBD@IDD2p?$AA@ DB 'IDD2p', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_06HMJDDHMI@VDDmax?$AA@
CONST	SEGMENT
??_C@_06HMJDDHMI@VDDmax?$AA@ DB 'VDDmax', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_03NDFLNPIA@VDD?$AA@
CONST	SEGMENT
??_C@_03NDFLNPIA@VDD?$AA@ DB 'VDD', 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_0M@EONNHDNC@PowerConfig?$AA@
CONST	SEGMENT
??_C@_0M@EONNHDNC@PowerConfig?$AA@ DB 'PowerConfig', 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_04NAJNOOFP@IDD0?$AA@
CONST	SEGMENT
??_C@_04NAJNOOFP@IDD0?$AA@ DB 'IDD0', 00H		; `string'
__bad_alloc_Message DD FLAT:??_C@_0P@GHFPNOJB@bad?5allocation?$AA@
; Function compile flags: /Ogtpy
; File c:\program files\microsoft visual studio 9.0\vc\include\xlocnum
;	COMDAT ?put@?$num_put@DV?$ostreambuf_iterator@DU?$char_traits@D@std@@@std@@@std@@QBE?AV?$ostreambuf_iterator@DU?$char_traits@D@std@@@2@V32@AAVios_base@2@DJ@Z
_TEXT	SEGMENT
__Dest$ = 8						; size = 8
__Iosbase$ = 16						; size = 4
__Fill$ = 20						; size = 1
?put@?$num_put@DV?$ostreambuf_iterator@DU?$char_traits@D@std@@@std@@@std@@QBE?AV?$ostreambuf_iterator@DU?$char_traits@D@std@@@2@V32@AAVios_base@2@DJ@Z PROC ; std::num_put<char,std::ostreambuf_iterator<char,std::char_traits<char> > >::put, COMDAT
; _this$ = ecx
; ___$ReturnUdt$ = esi
; __Val$ = edx

; 1079 : 		return (do_put(_Dest, _Iosbase, _Fill, _Val));

	mov	eax, DWORD PTR [ecx]
	mov	eax, DWORD PTR [eax+28]
	push	edx
	mov	edx, DWORD PTR __Fill$[esp]
	push	edx
	mov	edx, DWORD PTR __Iosbase$[esp+4]
	push	edx
	mov	edx, DWORD PTR __Dest$[esp+12]
	push	edx
	mov	edx, DWORD PTR __Dest$[esp+12]
	push	edx
	push	esi
	call	eax
	mov	eax, esi

; 1080 : 		}

	ret	16					; 00000010H
?put@?$num_put@DV?$ostreambuf_iterator@DU?$char_traits@D@std@@@std@@@std@@QBE?AV?$ostreambuf_iterator@DU?$char_traits@D@std@@@2@V32@AAVios_base@2@DJ@Z ENDP ; std::num_put<char,std::ostreambuf_iterator<char,std::char_traits<char> > >::put
_TEXT	ENDS
PUBLIC	__real@bff0000000000000
PUBLIC	__real@bf800000
PUBLIC	??0PowerConfig@DRAMsimII@@AAE@XZ		; DRAMsimII::PowerConfig::PowerConfig
;	COMDAT __real@bff0000000000000
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\powerconfig.cpp
CONST	SEGMENT
__real@bff0000000000000 DQ 0bff0000000000000r	; -1
CONST	ENDS
;	COMDAT __real@bf800000
CONST	SEGMENT
__real@bf800000 DD 0bf800000r			; -1
; Function compile flags: /Ogtpy
CONST	ENDS
_TEXT	SEGMENT
??0PowerConfig@DRAMsimII@@AAE@XZ PROC			; DRAMsimII::PowerConfig::PowerConfig
; _this$ = eax

; 104  : {}

	movss	xmm0, DWORD PTR __real@bf800000
	mov	edx, 2147483647				; 7fffffffH
	or	ecx, -1
	movss	DWORD PTR [eax], xmm0
	movss	DWORD PTR [eax+4], xmm0
	movsd	xmm0, QWORD PTR __real@bff0000000000000
	mov	DWORD PTR [eax+8], edx
	mov	DWORD PTR [eax+12], edx
	mov	DWORD PTR [eax+16], edx
	mov	DWORD PTR [eax+20], edx
	mov	DWORD PTR [eax+24], edx
	mov	DWORD PTR [eax+28], edx
	mov	DWORD PTR [eax+32], edx
	mov	DWORD PTR [eax+36], edx
	mov	DWORD PTR [eax+40], edx
	movsd	QWORD PTR [eax+48], xmm0
	movsd	QWORD PTR [eax+56], xmm0
	movsd	QWORD PTR [eax+64], xmm0
	movsd	QWORD PTR [eax+72], xmm0
	movsd	QWORD PTR [eax+80], xmm0
	movsd	QWORD PTR [eax+88], xmm0
	movsd	QWORD PTR [eax+96], xmm0
	movsd	QWORD PTR [eax+104], xmm0
	movsd	QWORD PTR [eax+112], xmm0
	movsd	QWORD PTR [eax+120], xmm0
	movsd	QWORD PTR [eax+128], xmm0
	movsd	QWORD PTR [eax+136], xmm0
	movsd	QWORD PTR [eax+144], xmm0
	movsd	QWORD PTR [eax+152], xmm0
	mov	DWORD PTR [eax+160], ecx
	mov	DWORD PTR [eax+164], ecx
	mov	DWORD PTR [eax+168], ecx
	mov	DWORD PTR [eax+172], ecx
	mov	DWORD PTR [eax+176], ecx
	mov	DWORD PTR [eax+180], ecx
	mov	DWORD PTR [eax+184], ecx
	mov	DWORD PTR [eax+188], ecx
	mov	DWORD PTR [eax+192], ecx
	mov	DWORD PTR [eax+200], ecx
	mov	DWORD PTR [eax+204], edx
	ret	0
??0PowerConfig@DRAMsimII@@AAE@XZ ENDP			; DRAMsimII::PowerConfig::PowerConfig
_TEXT	ENDS
PUBLIC	??1PowerConfig@DRAMsimII@@QAE@XZ		; DRAMsimII::PowerConfig::~PowerConfig
; Function compile flags: /Ogtpy
_TEXT	SEGMENT
??1PowerConfig@DRAMsimII@@QAE@XZ PROC			; DRAMsimII::PowerConfig::~PowerConfig

; 65   : {}

	ret	0
??1PowerConfig@DRAMsimII@@QAE@XZ ENDP			; DRAMsimII::PowerConfig::~PowerConfig
_TEXT	ENDS
PUBLIC	__real@41f0000000000000
PUBLIC	__real@4f800000
PUBLIC	??0PowerConfig@DRAMsimII@@QAE@ABVSettings@1@@Z	; DRAMsimII::PowerConfig::PowerConfig
;	COMDAT __real@41f0000000000000
CONST	SEGMENT
__real@41f0000000000000 DQ 041f0000000000000r	; 4.29497e+009
CONST	ENDS
;	COMDAT __real@4f800000
CONST	SEGMENT
__real@4f800000 DD 04f800000r			; 4.29497e+009
; Function compile flags: /Ogtpy
CONST	ENDS
_TEXT	SEGMENT
tv497 = -4						; size = 4
tv493 = -4						; size = 4
tv489 = -4						; size = 4
tv484 = -4						; size = 4
tv480 = -4						; size = 4
tv477 = -4						; size = 4
tv474 = -4						; size = 4
tv464 = -4						; size = 4
tv461 = -4						; size = 4
tv460 = -4						; size = 4
??0PowerConfig@DRAMsimII@@QAE@ABVSettings@1@@Z PROC	; DRAMsimII::PowerConfig::PowerConfig
; _this$ = esi
; _settings$ = ecx

; 62   : {}

	push	ecx
	fld	DWORD PTR [ecx+436]
	push	ebx
	fstp	DWORD PTR [esi]
	push	edi
	fld	DWORD PTR [ecx+432]
	fstp	DWORD PTR [esi+4]
	mov	eax, DWORD PTR [ecx+440]
	mov	DWORD PTR [esi+8], eax
	mov	edx, DWORD PTR [ecx+444]
	mov	DWORD PTR [esi+12], edx
	mov	eax, DWORD PTR [ecx+448]
	mov	DWORD PTR [esi+16], eax
	mov	edx, DWORD PTR [ecx+452]
	mov	DWORD PTR [esi+20], edx
	mov	eax, DWORD PTR [ecx+456]
	mov	DWORD PTR [esi+24], eax
	mov	edx, DWORD PTR [ecx+460]
	mov	DWORD PTR [esi+28], edx
	mov	eax, DWORD PTR [ecx+468]
	mov	DWORD PTR [esi+32], eax
	mov	edx, DWORD PTR [ecx+464]
	mov	DWORD PTR [esi+36], edx
	mov	eax, DWORD PTR [ecx+472]
	mov	DWORD PTR [esi+40], eax
	mov	edi, DWORD PTR [ecx+348]
	mov	edx, DWORD PTR [ecx+344]
	mov	ebx, DWORD PTR [ecx+460]
	imul	ebx, edx
	mov	eax, edi
	sub	eax, edx
	imul	eax, DWORD PTR [ecx+452]
	xor	edx, edx
	add	eax, ebx
	div	edi
	mov	edx, DWORD PTR [ecx+440]
	sub	edx, eax
	mov	DWORD PTR tv497[esp+12], edx
	fild	DWORD PTR tv497[esp+12]
	test	edx, edx
	jge	SHORT $LN3@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN3@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+48]
	mov	eax, DWORD PTR [ecx+460]
	fild	DWORD PTR [ecx+460]
	test	eax, eax
	jge	SHORT $LN4@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN4@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+56]
	mov	edx, DWORD PTR [ecx+452]
	fild	DWORD PTR [ecx+452]
	test	edx, edx
	jge	SHORT $LN5@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN5@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+64]
	mov	eax, DWORD PTR [ecx+456]
	fild	DWORD PTR [ecx+456]
	test	eax, eax
	jge	SHORT $LN6@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN6@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+72]
	mov	edx, DWORD PTR [ecx+448]
	fild	DWORD PTR [ecx+448]
	test	edx, edx
	jge	SHORT $LN7@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN7@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+80]
	mov	eax, DWORD PTR [ecx+468]
	sub	eax, DWORD PTR [ecx+460]
	mov	DWORD PTR tv477[esp+12], eax
	fild	DWORD PTR tv477[esp+12]
	test	eax, eax
	jge	SHORT $LN8@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN8@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+88]
	mov	edx, DWORD PTR [ecx+464]
	sub	edx, DWORD PTR [ecx+460]
	mov	DWORD PTR tv474[esp+12], edx
	fild	DWORD PTR tv474[esp+12]
	test	edx, edx
	jge	SHORT $LN9@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN9@PowerConfi:
	fmul	DWORD PTR [ecx+436]
	fstp	QWORD PTR [esi+96]
	movss	xmm0, DWORD PTR [ecx+436]
	movss	xmm1, DWORD PTR [ecx+432]
	cvtps2pd xmm1, xmm1
	cvtps2pd xmm0, xmm0
	divsd	xmm0, xmm1
	movapd	xmm1, xmm0
	mulsd	xmm1, xmm0
	movsd	QWORD PTR [esi+104], xmm1
	fild	DWORD PTR [ecx+204]
	mov	eax, DWORD PTR [ecx+204]
	test	eax, eax
	jge	SHORT $LN10@PowerConfi
	fadd	QWORD PTR __real@41f0000000000000
$LN10@PowerConfi:
	mov	edx, DWORD PTR [ecx+428]
	fild	DWORD PTR [ecx+428]
	test	edx, edx
	jge	SHORT $LN11@PowerConfi
	fadd	QWORD PTR __real@41f0000000000000
$LN11@PowerConfi:
	fdivp	ST(1), ST(0)
	fstp	QWORD PTR [esi+112]
	mov	eax, DWORD PTR [ecx+420]
	add	eax, DWORD PTR [ecx+424]
	add	eax, DWORD PTR [ecx+416]
	mov	DWORD PTR tv460[esp+12], eax
	fild	DWORD PTR tv460[esp+12]
	test	eax, eax
	jge	SHORT $LN12@PowerConfi
	fadd	DWORD PTR __real@4f800000
$LN12@PowerConfi:
	fmul	DWORD PTR [ecx+404]
	pop	edi
	pop	ebx
	fstp	QWORD PTR [esi+120]
	movss	xmm0, DWORD PTR [ecx+400]
	cvtps2pd xmm0, xmm0
	movsd	QWORD PTR [esi+128], xmm0
	movss	xmm0, DWORD PTR [ecx+404]
	cvtps2pd xmm0, xmm0
	movsd	QWORD PTR [esi+136], xmm0
	movss	xmm0, DWORD PTR [ecx+408]
	cvtps2pd xmm0, xmm0
	movsd	QWORD PTR [esi+144], xmm0
	movss	xmm0, DWORD PTR [ecx+412]
	cvtps2pd xmm0, xmm0
	movsd	QWORD PTR [esi+152], xmm0
	mov	edx, DWORD PTR [ecx+416]
	mov	DWORD PTR [esi+160], edx
	mov	eax, DWORD PTR [ecx+420]
	mov	DWORD PTR [esi+164], eax
	mov	edx, DWORD PTR [ecx+424]
	mov	DWORD PTR [esi+168], edx
	mov	eax, DWORD PTR [ecx+232]
	add	eax, eax
	add	eax, eax
	add	eax, eax
	xor	edx, edx
	div	DWORD PTR [esi+160]
	mov	DWORD PTR [esi+172], eax
	mov	eax, DWORD PTR [ecx+204]
	mov	DWORD PTR [esi+176], eax
	mov	edx, DWORD PTR [ecx+428]
	mov	DWORD PTR [esi+180], edx
	mov	eax, DWORD PTR [ecx+328]
	mov	DWORD PTR [esi+184], eax
	mov	edx, DWORD PTR [ecx+348]
	mov	DWORD PTR [esi+188], edx
	mov	eax, DWORD PTR [ecx+344]
	mov	DWORD PTR [esi+192], eax
	xor	eax, eax
	mov	DWORD PTR [esi+200], eax
	mov	DWORD PTR [esi+204], eax
	mov	eax, esi
	pop	ecx
	ret	0
??0PowerConfig@DRAMsimII@@QAE@ABVSettings@1@@Z ENDP	; DRAMsimII::PowerConfig::PowerConfig
PUBLIC	__real@3ee4f8b588e368f1
;	COMDAT __real@3ee4f8b588e368f1
; File c:\program files\microsoft visual studio 9.0\vc\include\math.h
CONST	SEGMENT
__real@3ee4f8b588e368f1 DQ 03ee4f8b588e368f1r	; 1e-005
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\globals.h
CONST	ENDS
;	COMDAT ??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z
_TEXT	SEGMENT
_nVal2$ = 8						; size = 8
??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z PROC		; DRAMsimII::AlmostEqual<double>, COMDAT
; _nVal1$ = xmm0l

; 114  : 		return std::abs(nVal1 - nVal2) <= EPSILON * std::fabs(nVal1);

	movsd	xmm1, QWORD PTR __mask@@AbsDouble@
	movapd	xmm2, xmm0
	subsd	xmm2, QWORD PTR _nVal2$[esp-4]
	andpd	xmm0, xmm1
	mulsd	xmm0, QWORD PTR __real@3ee4f8b588e368f1
	andpd	xmm2, xmm1
	comisd	xmm0, xmm2
	jb	SHORT $LN3@AlmostEqua
	mov	eax, 1

; 115  : 		// see Knuth section 4.2.2 pages 217-218
; 116  : 	}

	ret	0
$LN3@AlmostEqua:

; 114  : 		return std::abs(nVal1 - nVal2) <= EPSILON * std::fabs(nVal1);

	xor	eax, eax

; 115  : 		// see Knuth section 4.2.2 pages 217-218
; 116  : 	}

	ret	0
??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z ENDP		; DRAMsimII::AlmostEqual<double>
PUBLIC	??8PowerConfig@DRAMsimII@@QBE_NABV01@@Z		; DRAMsimII::PowerConfig::operator==
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\powerconfig.cpp
_TEXT	SEGMENT
??8PowerConfig@DRAMsimII@@QBE_NABV01@@Z PROC		; DRAMsimII::PowerConfig::operator==
; _this$ = edi
; _rhs$ = esi

; 108  : 	return (AlmostEqual<float>(VDD, rhs.VDD) && AlmostEqual<float>(VDDmax, rhs.VDDmax) && IDD0 == rhs.IDD0 &&
; 109  : 		IDD2P == rhs.IDD2P && IDD2N == rhs.IDD2N && IDD3P == rhs.IDD3P && IDD1 == rhs.IDD1 &&
; 110  : 		IDD3N == rhs.IDD3N && IDD4R == rhs.IDD4R && IDD4W == rhs.IDD4W &&
; 111  : 		IDD5 == rhs.IDD5 && AlmostEqual<float>(PdsACT, rhs.PdsACT) && AlmostEqual<float>(PdsACT_STBY, rhs.PdsACT_STBY) &&
; 112  : 		AlmostEqual<double>(PdsRD, rhs.PdsRD) &&
; 113  : 		AlmostEqual<double>(PdsACT_PDN, rhs.PdsACT_PDN) &&
; 114  : 		AlmostEqual<double>(PdsPRE_STBY, rhs.PdsPRE_STBY) &&
; 115  : 		AlmostEqual<double>(PdsPRE_PDN, rhs.PdsPRE_PDN) &&
; 116  : 		AlmostEqual<double>(PdsWR, rhs.PdsWR) &&
; 117  : 		AlmostEqual<double>(PdstermW, rhs.PdstermW) &&
; 118  : 		AlmostEqual<double>(PdqRD, rhs.PdqRD) &&
; 119  : 		AlmostEqual<double>(PdqWR, rhs.PdqWR) &&
; 120  : 		AlmostEqual<double>(PdqRDoth, rhs.PdqRDoth) &&
; 121  : 		AlmostEqual<double>(PdqWRoth, rhs.PdqWRoth) && DQperDRAM == rhs.DQperDRAM && DQSperDRAM == rhs.DQSperDRAM &&
; 122  : 		DMperDRAM == rhs.DMperDRAM && DQperRank == rhs.DQperRank && frequency == rhs.frequency &&
; 123  : 		specFrequency == rhs.specFrequency && tBurst == rhs.tBurst && tRC == rhs.tRC &&
; 124  : 		tRAS == rhs.tRAS && lastCalculation==rhs.lastCalculation &&
; 125  : 		AlmostEqual<double>(voltageScaleFactor, rhs.voltageScaleFactor) &&
; 126  : 		AlmostEqual<double>(frequencyScaleFactor, rhs.frequencyScaleFactor));

	movss	xmm1, DWORD PTR [edi]
	movss	xmm2, DWORD PTR [esi]
	cvtps2pd xmm0, xmm1
	cvtps2pd xmm2, xmm2
	subsd	xmm0, xmm2
	cvtsd2ss xmm0, xmm0
	cvtss2sd xmm2, xmm0
	movsd	xmm0, QWORD PTR __mask@@AbsDouble@
	cvtps2pd xmm1, xmm1
	andpd	xmm2, xmm0
	andpd	xmm1, xmm0
	cvtsd2ss xmm2, xmm2
	cvtss2sd xmm3, xmm2
	movsd	xmm2, QWORD PTR __real@3ee4f8b588e368f1
	cvtsd2ss xmm1, xmm1
	cvtss2sd xmm1, xmm1
	mulsd	xmm1, xmm2
	comisd	xmm1, xmm3
	jb	$LN3@operator@329
	movss	xmm1, DWORD PTR [edi+4]
	movss	xmm4, DWORD PTR [esi+4]
	cvtps2pd xmm3, xmm1
	cvtps2pd xmm4, xmm4
	subsd	xmm3, xmm4
	cvtsd2ss xmm3, xmm3
	cvtss2sd xmm3, xmm3
	andpd	xmm3, xmm0
	cvtps2pd xmm1, xmm1
	andpd	xmm1, xmm0
	cvtsd2ss xmm0, xmm1
	cvtsd2ss xmm3, xmm3
	cvtss2sd xmm0, xmm0
	cvtss2sd xmm3, xmm3
	mulsd	xmm0, xmm2
	comisd	xmm0, xmm3
	jb	$LN3@operator@329
	mov	eax, DWORD PTR [edi+8]
	cmp	eax, DWORD PTR [esi+8]
	jne	$LN3@operator@329
	mov	ecx, DWORD PTR [edi+16]
	cmp	ecx, DWORD PTR [esi+16]
	jne	$LN3@operator@329
	mov	edx, DWORD PTR [edi+20]
	cmp	edx, DWORD PTR [esi+20]
	jne	$LN3@operator@329
	mov	eax, DWORD PTR [edi+24]
	cmp	eax, DWORD PTR [esi+24]
	jne	$LN3@operator@329
	mov	ecx, DWORD PTR [edi+12]
	cmp	ecx, DWORD PTR [esi+12]
	jne	$LN3@operator@329
	mov	edx, DWORD PTR [edi+28]
	cmp	edx, DWORD PTR [esi+28]
	jne	$LN3@operator@329
	mov	eax, DWORD PTR [edi+32]
	cmp	eax, DWORD PTR [esi+32]
	jne	$LN3@operator@329
	mov	ecx, DWORD PTR [edi+36]
	cmp	ecx, DWORD PTR [esi+36]
	jne	$LN3@operator@329
	mov	edx, DWORD PTR [edi+40]
	cmp	edx, DWORD PTR [esi+40]
	jne	$LN3@operator@329
	movsd	xmm0, QWORD PTR [esi+48]
	cvtpd2ps xmm2, xmm0
	movsd	xmm0, QWORD PTR [edi+48]
	cvtpd2ps xmm0, xmm0
	call	??$AlmostEqual@M@DRAMsimII@@YA_NMM@Z	; DRAMsimII::AlmostEqual<float>
	test	al, al
	je	$LN3@operator@329
	movsd	xmm0, QWORD PTR [esi+56]
	cvtpd2ps xmm2, xmm0
	movsd	xmm0, QWORD PTR [edi+56]
	cvtpd2ps xmm0, xmm0
	call	??$AlmostEqual@M@DRAMsimII@@YA_NMM@Z	; DRAMsimII::AlmostEqual<float>
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+88]
	movsd	xmm0, QWORD PTR [edi+88]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+72]
	movsd	xmm0, QWORD PTR [edi+72]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+64]
	movsd	xmm0, QWORD PTR [edi+64]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+80]
	movsd	xmm0, QWORD PTR [edi+80]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+96]
	movsd	xmm0, QWORD PTR [edi+96]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+120]
	movsd	xmm0, QWORD PTR [edi+120]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+128]
	movsd	xmm0, QWORD PTR [edi+128]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+136]
	movsd	xmm0, QWORD PTR [edi+136]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+144]
	movsd	xmm0, QWORD PTR [edi+144]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	fld	QWORD PTR [esi+152]
	movsd	xmm0, QWORD PTR [edi+152]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	$LN3@operator@329
	mov	eax, DWORD PTR [edi+160]
	cmp	eax, DWORD PTR [esi+160]
	jne	$LN3@operator@329
	mov	ecx, DWORD PTR [edi+164]
	cmp	ecx, DWORD PTR [esi+164]
	jne	$LN3@operator@329
	mov	edx, DWORD PTR [edi+168]
	cmp	edx, DWORD PTR [esi+168]
	jne	$LN3@operator@329
	mov	eax, DWORD PTR [edi+172]
	cmp	eax, DWORD PTR [esi+172]
	jne	$LN3@operator@329
	mov	ecx, DWORD PTR [edi+176]
	cmp	ecx, DWORD PTR [esi+176]
	jne	$LN3@operator@329
	mov	edx, DWORD PTR [edi+180]
	cmp	edx, DWORD PTR [esi+180]
	jne	$LN3@operator@329
	mov	eax, DWORD PTR [edi+184]
	cmp	eax, DWORD PTR [esi+184]
	jne	SHORT $LN3@operator@329
	mov	ecx, DWORD PTR [edi+188]
	cmp	ecx, DWORD PTR [esi+188]
	jne	SHORT $LN3@operator@329
	mov	edx, DWORD PTR [edi+192]
	cmp	edx, DWORD PTR [esi+192]
	jne	SHORT $LN3@operator@329
	mov	eax, DWORD PTR [edi+200]
	cmp	eax, DWORD PTR [esi+200]
	jne	SHORT $LN3@operator@329
	mov	ecx, DWORD PTR [edi+204]
	cmp	ecx, DWORD PTR [esi+204]
	jne	SHORT $LN3@operator@329
	fld	QWORD PTR [esi+104]
	movsd	xmm0, QWORD PTR [edi+104]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	SHORT $LN3@operator@329
	fld	QWORD PTR [esi+112]
	movsd	xmm0, QWORD PTR [edi+112]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	??$AlmostEqual@N@DRAMsimII@@YA_NNN@Z	; DRAMsimII::AlmostEqual<double>
	add	esp, 8
	test	al, al
	je	SHORT $LN3@operator@329
	mov	eax, 1

; 127  : }

	ret	0
$LN3@operator@329:

; 108  : 	return (AlmostEqual<float>(VDD, rhs.VDD) && AlmostEqual<float>(VDDmax, rhs.VDDmax) && IDD0 == rhs.IDD0 &&
; 109  : 		IDD2P == rhs.IDD2P && IDD2N == rhs.IDD2N && IDD3P == rhs.IDD3P && IDD1 == rhs.IDD1 &&
; 110  : 		IDD3N == rhs.IDD3N && IDD4R == rhs.IDD4R && IDD4W == rhs.IDD4W &&
; 111  : 		IDD5 == rhs.IDD5 && AlmostEqual<float>(PdsACT, rhs.PdsACT) && AlmostEqual<float>(PdsACT_STBY, rhs.PdsACT_STBY) &&
; 112  : 		AlmostEqual<double>(PdsRD, rhs.PdsRD) &&
; 113  : 		AlmostEqual<double>(PdsACT_PDN, rhs.PdsACT_PDN) &&
; 114  : 		AlmostEqual<double>(PdsPRE_STBY, rhs.PdsPRE_STBY) &&
; 115  : 		AlmostEqual<double>(PdsPRE_PDN, rhs.PdsPRE_PDN) &&
; 116  : 		AlmostEqual<double>(PdsWR, rhs.PdsWR) &&
; 117  : 		AlmostEqual<double>(PdstermW, rhs.PdstermW) &&
; 118  : 		AlmostEqual<double>(PdqRD, rhs.PdqRD) &&
; 119  : 		AlmostEqual<double>(PdqWR, rhs.PdqWR) &&
; 120  : 		AlmostEqual<double>(PdqRDoth, rhs.PdqRDoth) &&
; 121  : 		AlmostEqual<double>(PdqWRoth, rhs.PdqWRoth) && DQperDRAM == rhs.DQperDRAM && DQSperDRAM == rhs.DQSperDRAM &&
; 122  : 		DMperDRAM == rhs.DMperDRAM && DQperRank == rhs.DQperRank && frequency == rhs.frequency &&
; 123  : 		specFrequency == rhs.specFrequency && tBurst == rhs.tBurst && tRC == rhs.tRC &&
; 124  : 		tRAS == rhs.tRAS && lastCalculation==rhs.lastCalculation &&
; 125  : 		AlmostEqual<double>(voltageScaleFactor, rhs.voltageScaleFactor) &&
; 126  : 		AlmostEqual<double>(frequencyScaleFactor, rhs.frequencyScaleFactor));

	xor	eax, eax

; 127  : }

	ret	0
??8PowerConfig@DRAMsimII@@QBE_NABV01@@Z ENDP		; DRAMsimII::PowerConfig::operator==
;	COMDAT xdata$x
; File c:\program files\microsoft visual studio 9.0\vc\include\ios
xdata$x	SEGMENT
__unwindtable$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z DD 0ffffffffH
	DD	FLAT:__unwindfunclet$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$2
	DD	00H
	DD	FLAT:__unwindfunclet$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$3
	DD	00H
	DD	00H
	DD	00H
	DD	00H
__catchsym$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$11 DD 040H
	DD	00H
	DD	00H
	DD	FLAT:__catch$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$0
__tryblocktable$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z DD 02H
	DD	02H
	DD	03H
	DD	01H
	DD	FLAT:__catchsym$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$11
__ehfuncinfo$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z DD 019930522H
	DD	04H
	DD	FLAT:__unwindtable$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z
	DD	01H
	DD	FLAT:__tryblocktable$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z
	DD	2 DUP(00H)
	DD	00H
	DD	01H
; Function compile flags: /Ogtpy
; File c:\program files\microsoft visual studio 9.0\vc\include\ostream
xdata$x	ENDS
;	COMDAT ??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z
_TEXT	SEGMENT
$T617327 = -48						; size = 8
$T617326 = -48						; size = 8
__Ok$ = -40						; size = 8
__Nput_fac$118844 = -32					; size = 4
$T617372 = -28						; size = 1
__Lock$617361 = -28					; size = 4
$T617321 = -24						; size = 4
__State$ = -20						; size = 4
__$EHRec$ = -16						; size = 16
_this$ = 8						; size = 4
__Val$ = 12						; size = 4
??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z PROC ; std::basic_ostream<char,std::char_traits<char> >::operator<<, COMDAT

; 264  : 		{	// insert an int

	push	ebp
	mov	ebp, esp
	push	-1
	push	__ehhandler$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z
	mov	eax, DWORD PTR fs:0
	push	eax
	sub	esp, 36					; 00000024H
	push	ebx
	push	esi
	push	edi
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	push	eax
	lea	eax, DWORD PTR __$EHRec$[ebp+4]
	mov	DWORD PTR fs:0, eax
	mov	DWORD PTR __$EHRec$[ebp], esp
	mov	ebx, DWORD PTR _this$[ebp]

; 265  : 		ios_base::iostate _State = ios_base::goodbit;

	xor	esi, esi

; 266  : 		const sentry _Ok(*this);

	push	ebx
	lea	ecx, DWORD PTR __Ok$[ebp]
	mov	DWORD PTR __State$[ebp], esi
	call	??0sentry@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAE@AAV12@@Z ; std::basic_ostream<char,std::char_traits<char> >::sentry::sentry
	mov	DWORD PTR __$EHRec$[ebp+12], esi

; 267  : 
; 268  : 		if (_Ok)

	cmp	BYTE PTR __Ok$[ebp+4], 0
	je	$LN15@operator@396

; 269  : 			{	// state okay, use facet to insert
; 270  : 			const _Nput& _Nput_fac = _USE(ios_base::getloc(), _Nput);

	mov	ecx, DWORD PTR [ebx]
	mov	ecx, DWORD PTR [ecx+4]
	lea	eax, DWORD PTR $T617321[ebp]
	push	eax
	add	ecx, ebx
	call	?getloc@ios_base@std@@QBE?AVlocale@2@XZ	; std::ios_base::getloc
	mov	edi, eax
	mov	BYTE PTR __$EHRec$[ebp+12], 1
	call	??$use_facet@V?$num_put@DV?$ostreambuf_iterator@DU?$char_traits@D@std@@@std@@@std@@@std@@YAABV?$num_put@DV?$ostreambuf_iterator@DU?$char_traits@D@std@@@std@@@0@ABVlocale@0@@Z ; std::use_facet<std::num_put<char,std::ostreambuf_iterator<char,std::char_traits<char> > > >
	mov	BYTE PTR __$EHRec$[ebp+12], 0
	mov	edi, DWORD PTR $T617321[ebp]
	mov	DWORD PTR __Nput_fac$118844[ebp], eax
	cmp	edi, esi
	je	SHORT $LN21@operator@396
	push	esi
	lea	ecx, DWORD PTR __Lock$617361[ebp]
	call	??0_Lockit@std@@QAE@H@Z			; std::_Lockit::_Lockit
	mov	eax, DWORD PTR [edi+4]
	cmp	eax, esi
	jbe	SHORT $LN23@operator@396
	cmp	eax, -1
	jae	SHORT $LN23@operator@396
	dec	eax
	mov	DWORD PTR [edi+4], eax
$LN23@operator@396:
	mov	esi, DWORD PTR [edi+4]
	neg	esi
	sbb	esi, esi
	not	esi
	lea	ecx, DWORD PTR __Lock$617361[ebp]
	and	esi, edi
	call	??1_Lockit@std@@QAE@XZ			; std::_Lockit::~_Lockit
	test	esi, esi
	je	SHORT $LN21@operator@396
	mov	edx, DWORD PTR [esi]
	mov	eax, DWORD PTR [edx]
	push	1
	mov	ecx, esi
	call	eax
$LN21@operator@396:

; 271  : 			ios_base::fmtflags _Bfl =
; 272  : 				ios_base::flags() & ios_base::basefield;

	mov	ecx, DWORD PTR [ebx]

; 273  : 			long _Tmp = (_Bfl == ios_base::oct
; 274  : 				|| _Bfl == ios_base::hex)
; 275  : 				? (long)(unsigned int)_Val : (long)_Val;
; 276  : 
; 277  : 			_TRY_IO_BEGIN
; 278  : 			if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this,
; 279  : 				_Myios::fill(), _Tmp).failed())

	mov	edi, DWORD PTR __Val$[ebp]
	mov	BYTE PTR __$EHRec$[ebp+12], 2
	mov	eax, DWORD PTR [ecx+4]
	mov	ecx, DWORD PTR [ecx+4]
	mov	dl, BYTE PTR [ecx+ebx+48]
	lea	esi, DWORD PTR [ecx+ebx]
	mov	esi, DWORD PTR [esi+40]
	mov	ecx, DWORD PTR __Nput_fac$118844[ebp]
	push	edi
	mov	BYTE PTR $T617372[ebp], dl
	mov	edi, DWORD PTR $T617372[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	edx, DWORD PTR [edx+28]
	push	edi
	add	eax, ebx
	push	eax
	mov	BYTE PTR $T617326[ebp], 0
	mov	eax, DWORD PTR $T617326[ebp]
	push	esi
	push	eax
	lea	eax, DWORD PTR $T617327[ebp]
	push	eax
	call	edx
	cmp	BYTE PTR $T617327[ebp], 0
	je	SHORT $LN1@operator@396

; 280  : 				_State |= ios_base::badbit;

	mov	DWORD PTR __State$[ebp], 4
$LN1@operator@396:
	mov	DWORD PTR __$EHRec$[ebp+12], 0
	jmp	SHORT $LN15@operator@396
__catch$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$0:

; 281  : 			_CATCH_IO_END

	mov	eax, DWORD PTR _this$[ebp]
	mov	ecx, DWORD PTR [eax]
	mov	ecx, DWORD PTR [ecx+4]
	add	ecx, eax
	mov	eax, DWORD PTR [ecx+8]
	or	eax, 4
	cmp	DWORD PTR [ecx+40], 0
	jne	SHORT $LN46@operator@396
	or	eax, 4
$LN46@operator@396:
	push	1
	push	eax
	call	?clear@ios_base@std@@QAEXH_N@Z		; std::ios_base::clear
	mov	DWORD PTR __$EHRec$[ebp+12], 0
	mov	eax, $LN83@operator@396
	ret	0
$LN83@operator@396:
	mov	ebx, DWORD PTR _this$[ebp]
$LN15@operator@396:

; 282  : 			}
; 283  : 
; 284  : 		_Myios::setstate(_State);

	mov	edx, DWORD PTR [ebx]
	mov	ecx, DWORD PTR [edx+4]
	mov	edx, DWORD PTR __State$[ebp]
	add	ecx, ebx
	test	edx, edx
	je	SHORT $LN75@operator@396
	mov	eax, DWORD PTR [ecx+8]
	or	eax, edx
	cmp	DWORD PTR [ecx+40], 0
	jne	SHORT $LN77@operator@396
	or	eax, 4
$LN77@operator@396:
	push	0
	push	eax
	call	?clear@ios_base@std@@QAEXH_N@Z		; std::ios_base::clear
$LN75@operator@396:

; 285  : 		return (*this);

	lea	ecx, DWORD PTR __Ok$[ebp]
	mov	DWORD PTR __$EHRec$[ebp+12], -1
	call	??1sentry@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAE@XZ ; std::basic_ostream<char,std::char_traits<char> >::sentry::~sentry
	mov	eax, ebx

; 286  : 		}

	mov	ecx, DWORD PTR __$EHRec$[ebp+4]
	mov	DWORD PTR fs:0, ecx
	pop	ecx
	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	8
_TEXT	ENDS
;	COMDAT text$x
text$x	SEGMENT
__unwindfunclet$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$2:
	lea	ecx, DWORD PTR __Ok$[ebp]
	jmp	??1sentry@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAE@XZ ; std::basic_ostream<char,std::char_traits<char> >::sentry::~sentry
__unwindfunclet$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z$3:
	lea	ecx, DWORD PTR $T617321[ebp]
	jmp	??1locale@std@@QAE@XZ			; std::locale::~locale
__ehhandler$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z:
	mov	edx, DWORD PTR [esp+8]
	lea	eax, DWORD PTR [edx+12]
	mov	ecx, DWORD PTR [edx-52]
	xor	ecx, eax
	call	@__security_check_cookie@4
	mov	eax, OFFSET __ehfuncinfo$??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z
	jmp	___CxxFrameHandler3
text$x	ENDS
??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z ENDP ; std::basic_ostream<char,std::char_traits<char> >::operator<<
PUBLIC	??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVPowerConfig@0@@Z ; DRAMsimII::operator<<
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\powerconfig.cpp
_TEXT	SEGMENT
??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVPowerConfig@0@@Z PROC ; DRAMsimII::operator<<
; _in$ = esi
; _pc$ = eax

; 130  : {

	push	ebx
	push	edi

; 131  : 	using std::endl;
; 132  : 
; 133  : 	in << "PowerConfig" << endl;

	push	OFFSET ??_C@_0M@EONNHDNC@PowerConfig?$AA@
	push	esi
	mov	edi, eax
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	mov	ebx, eax
	push	10					; 0000000aH
	mov	ecx, ebx
	call	?put@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@D@Z ; std::basic_ostream<char,std::char_traits<char> >::put
	mov	ecx, ebx
	call	?flush@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@XZ ; std::basic_ostream<char,std::char_traits<char> >::flush

; 134  : 	in << "VDD" << pc.VDD << endl;

	fld	DWORD PTR [edi]
	push	ecx
	fstp	DWORD PTR [esp]
	push	OFFSET ??_C@_03NDFLNPIA@VDD?$AA@
	push	esi
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	push	eax
	call	??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@M@Z ; std::basic_ostream<char,std::char_traits<char> >::operator<<
	mov	ebx, eax
	push	10					; 0000000aH
	mov	ecx, ebx
	call	?put@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@D@Z ; std::basic_ostream<char,std::char_traits<char> >::put
	mov	ecx, ebx
	call	?flush@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@XZ ; std::basic_ostream<char,std::char_traits<char> >::flush

; 135  : 	in << "VDDmax" << pc.VDDmax << endl;

	fld	DWORD PTR [edi+4]
	push	ecx
	fstp	DWORD PTR [esp]
	push	OFFSET ??_C@_06HMJDDHMI@VDDmax?$AA@
	push	esi
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	push	eax
	call	??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@M@Z ; std::basic_ostream<char,std::char_traits<char> >::operator<<
	mov	ebx, eax
	push	10					; 0000000aH
	mov	ecx, ebx
	call	?put@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@D@Z ; std::basic_ostream<char,std::char_traits<char> >::put
	mov	ecx, ebx
	call	?flush@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@XZ ; std::basic_ostream<char,std::char_traits<char> >::flush

; 136  : 	in << "IDD0" << pc.IDD0 << endl;

	mov	eax, DWORD PTR [edi+8]
	push	eax
	push	OFFSET ??_C@_04NAJNOOFP@IDD0?$AA@
	push	esi
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	push	eax
	call	??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z ; std::basic_ostream<char,std::char_traits<char> >::operator<<
	mov	ebx, eax
	push	10					; 0000000aH
	mov	ecx, ebx
	call	?put@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@D@Z ; std::basic_ostream<char,std::char_traits<char> >::put
	mov	ecx, ebx
	call	?flush@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@XZ ; std::basic_ostream<char,std::char_traits<char> >::flush

; 137  : 	in << "IDD2p" << pc.IDD2P << endl;

	mov	ecx, DWORD PTR [edi+16]
	push	ecx
	push	OFFSET ??_C@_05NHLPHMBD@IDD2p?$AA@
	push	esi
	call	??$?6U?$char_traits@D@std@@@std@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@0@AAV10@PBD@Z ; std::operator<<<std::char_traits<char> >
	add	esp, 8
	push	eax
	call	??6?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV01@H@Z ; std::basic_ostream<char,std::char_traits<char> >::operator<<
	mov	edi, eax
	push	10					; 0000000aH
	mov	ecx, edi
	call	?put@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@D@Z ; std::basic_ostream<char,std::char_traits<char> >::put
	mov	ecx, edi
	call	?flush@?$basic_ostream@DU?$char_traits@D@std@@@std@@QAEAAV12@XZ ; std::basic_ostream<char,std::char_traits<char> >::flush
	pop	edi

; 138  : 	return in;

	mov	eax, esi
	pop	ebx

; 139  : }

	ret	0
??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVPowerConfig@0@@Z ENDP ; DRAMsimII::operator<<
END
