; Listing generated by Microsoft (R) Optimizing Compiler Version 15.00.30729.01 

	TITLE	c:\Users\CriuS\Documents\Visual Studio 2008\Projects\DRAMsimIIhg\src\fbdFrame.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB OLDNAMES

PUBLIC	?acquireItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEPAVfbdFrame@2@XZ ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::acquireItem
PUBLIC	?releaseItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEXPAVfbdFrame@2@@Z ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::releaseItem
__bad_alloc_Message DD FLAT:??_C@_0P@GHFPNOJB@bad?5allocation?$AA@
PUBLIC	??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVfbdFrame@0@@Z ; DRAMsimII::operator<<
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\fbdframe.cpp
_TEXT	SEGMENT
??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVfbdFrame@0@@Z PROC ; DRAMsimII::operator<<
; _os$ = eax

; 48   : 	return os;
; 49   : }

	ret	0
??6DRAMsimII@@YAAAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AAV12@ABVfbdFrame@0@@Z ENDP ; DRAMsimII::operator<<
_TEXT	ENDS
PUBLIC	??1fbdFrame@DRAMsimII@@QAE@XZ			; DRAMsimII::fbdFrame::~fbdFrame
; Function compile flags: /Ogtpy
_TEXT	SEGMENT
??1fbdFrame@DRAMsimII@@QAE@XZ PROC			; DRAMsimII::fbdFrame::~fbdFrame

; 32   : {}

	ret	0
??1fbdFrame@DRAMsimII@@QAE@XZ ENDP			; DRAMsimII::fbdFrame::~fbdFrame
_TEXT	ENDS
PUBLIC	??0fbdFrame@DRAMsimII@@QAE@_J@Z			; DRAMsimII::fbdFrame::fbdFrame
; Function compile flags: /Ogtpy
_TEXT	SEGMENT
_time$ = 8						; size = 8
??0fbdFrame@DRAMsimII@@QAE@_J@Z PROC			; DRAMsimII::fbdFrame::fbdFrame
; _this$ = eax

; 27   : {

	mov	ecx, DWORD PTR _time$[esp-4]
	mov	edx, DWORD PTR _time$[esp]
	mov	DWORD PTR [eax+40], ecx

; 28   : 	commandSlotType[0] = commandSlotType[1] = commandSlotType[2] = Command::INVALID_COMMAND;

	mov	ecx, 16					; 00000010H
	mov	DWORD PTR [eax+44], edx
	mov	DWORD PTR [eax+24], ecx
	mov	DWORD PTR [eax+20], ecx
	mov	DWORD PTR [eax+16], ecx

; 29   : }

	ret	8
??0fbdFrame@DRAMsimII@@QAE@_J@Z ENDP			; DRAMsimII::fbdFrame::fbdFrame
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\queue.h
;	COMDAT ?acquireItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEPAVfbdFrame@2@XZ
_TEXT	SEGMENT
?acquireItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEPAVfbdFrame@2@XZ PROC ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::acquireItem, COMDAT

; 291  : 		{

	push	esi

; 292  : 			assert(pool);
; 293  : 			T* newItem;
; 294  : 			//#pragma omp critical
; 295  : 			{
; 296  : 				if (count == 0)

	xor	esi, esi
	cmp	DWORD PTR ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A, esi
	jne	SHORT $LN2@acquireIte

; 297  : 				{
; 298  : 					newItem = ::new T;

	push	48					; 00000030H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	cmp	eax, esi
	je	SHORT $LN5@acquireIte
	mov	ecx, 16					; 00000010H
	mov	DWORD PTR [eax+40], esi
	mov	DWORD PTR [eax+44], esi
	mov	DWORD PTR [eax+24], ecx
	mov	DWORD PTR [eax+20], ecx
	mov	DWORD PTR [eax+16], ecx
	pop	esi

; 303  : 			}
; 304  : 			return newItem;
; 305  : 		}

	ret	0
$LN5@acquireIte:

; 297  : 				{
; 298  : 					newItem = ::new T;

	xor	eax, eax
	pop	esi

; 303  : 			}
; 304  : 			return newItem;
; 305  : 		}

	ret	0
$LN2@acquireIte:

; 299  : 					assert(newItem != NULL);
; 300  : 				}
; 301  : 				else
; 302  : 					newItem = pop();

	mov	esi, OFFSET ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A ; DRAMsimII::fbdFrame::freeFramePool
	call	?pop@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEPAVfbdFrame@2@XZ ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::pop
	pop	esi

; 303  : 			}
; 304  : 			return newItem;
; 305  : 		}

	ret	0
?acquireItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEPAVfbdFrame@2@XZ ENDP ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::acquireItem
_TEXT	ENDS
PUBLIC	??2fbdFrame@DRAMsimII@@SAPAXI@Z			; DRAMsimII::fbdFrame::operator new
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\fbdframe.cpp
_TEXT	SEGMENT
??2fbdFrame@DRAMsimII@@SAPAXI@Z PROC			; DRAMsimII::fbdFrame::operator new

; 35   : {

	push	esi

; 36   : 	assert(size == sizeof(fbdFrame));
; 37   : 	return freeFramePool.acquireItem();

	xor	esi, esi
	cmp	DWORD PTR ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A, esi
	jne	SHORT $LN4@operator@244
	push	48					; 00000030H
	call	??2@YAPAXI@Z				; operator new
	add	esp, 4
	cmp	eax, esi
	je	SHORT $LN7@operator@244
	mov	ecx, 16					; 00000010H
	mov	DWORD PTR [eax+40], esi
	mov	DWORD PTR [eax+44], esi
	mov	DWORD PTR [eax+24], ecx
	mov	DWORD PTR [eax+20], ecx
	mov	DWORD PTR [eax+16], ecx
	pop	esi

; 38   : }

	ret	0

; 36   : 	assert(size == sizeof(fbdFrame));
; 37   : 	return freeFramePool.acquireItem();

$LN7@operator@244:
	xor	eax, eax
	pop	esi

; 38   : }

	ret	0

; 36   : 	assert(size == sizeof(fbdFrame));
; 37   : 	return freeFramePool.acquireItem();

$LN4@operator@244:
	mov	esi, OFFSET ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A ; DRAMsimII::fbdFrame::freeFramePool
	call	?pop@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEPAVfbdFrame@2@XZ ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::pop
	pop	esi

; 38   : }

	ret	0
??2fbdFrame@DRAMsimII@@SAPAXI@Z ENDP			; DRAMsimII::fbdFrame::operator new
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\queue.h
;	COMDAT ?releaseItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEXPAVfbdFrame@2@@Z
_TEXT	SEGMENT
_item$ = 8						; size = 4
?releaseItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEXPAVfbdFrame@2@@Z PROC ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::releaseItem, COMDAT
; _this$ = ecx

; 266  : 		{

	push	ebx
	push	ebp
	push	esi
	push	edi
	mov	edi, ecx

; 267  : 			assert(pool);
; 268  : 			//#pragma omp critical
; 269  : 			{
; 270  : 
; 271  : #if 1
; 272  : 				// look around to see if this was already in there, slows things down a lot, so use only when this might be a problem
; 273  : 				for (typename std::vector<T *>::iterator i = entry.begin(); i != entry.end(); i++)

	mov	ebx, DWORD PTR [edi+24]
	cmp	ebx, DWORD PTR [edi+28]
	jbe	SHORT $LN14@releaseIte@2
	call	__invalid_parameter_noinfo
$LN14@releaseIte@2:
	mov	esi, DWORD PTR [edi+12]
$LL31@releaseIte@2:
	mov	ebp, DWORD PTR [edi+28]
	cmp	DWORD PTR [edi+24], ebp
	jbe	SHORT $LN46@releaseIte@2
	call	__invalid_parameter_noinfo
$LN46@releaseIte@2:
	mov	eax, DWORD PTR [edi+12]
	test	esi, esi
	je	SHORT $LN59@releaseIte@2
	cmp	esi, eax
	je	SHORT $LN60@releaseIte@2
$LN59@releaseIte@2:
	call	__invalid_parameter_noinfo
$LN60@releaseIte@2:
	cmp	ebx, ebp
	je	SHORT $LN2@releaseIte@2
	test	esi, esi
	jne	SHORT $LN75@releaseIte@2
	call	__invalid_parameter_noinfo
	xor	eax, eax
$LN38@releaseIte@2:
	cmp	ebx, DWORD PTR [eax+16]
	jb	SHORT $LN29@releaseIte@2
	call	__invalid_parameter_noinfo
$LN29@releaseIte@2:
	add	ebx, 4
	jmp	SHORT $LL31@releaseIte@2
$LN75@releaseIte@2:
	mov	eax, DWORD PTR [esi]
	jmp	SHORT $LN38@releaseIte@2
$LN2@releaseIte@2:

; 274  : 				{
; 275  : 					assert(item != *i);
; 276  : 				}
; 277  : #endif
; 278  : 				assert(pool);
; 279  : 				if (!push(item))

	mov	esi, DWORD PTR _item$[esp+12]
	push	esi
	mov	eax, edi
	call	?push@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAE_NPAVfbdFrame@2@@Z ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::push
	test	al, al
	jne	SHORT $LN1@releaseIte@2

; 280  : 				{
; 281  : 					::delete item;

	push	esi
	call	??3@YAXPAX@Z				; operator delete
	add	esp, 4
$LN1@releaseIte@2:
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx

; 282  : 					item = NULL;
; 283  : 				}
; 284  : 			}
; 285  : 		}

	ret	4
?releaseItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEXPAVfbdFrame@2@@Z ENDP ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::releaseItem
_TEXT	ENDS
PUBLIC	??3fbdFrame@DRAMsimII@@SAXPAX@Z			; DRAMsimII::fbdFrame::operator delete
; Function compile flags: /Ogtpy
; File c:\users\crius\documents\visual studio 2008\projects\dramsimiihg\src\fbdframe.cpp
_TEXT	SEGMENT
??3fbdFrame@DRAMsimII@@SAXPAX@Z PROC			; DRAMsimII::fbdFrame::operator delete
; _mem$ = eax

; 42   : 	fbdFrame *frame(static_cast<fbdFrame*>(mem));
; 43   : 	freeFramePool.releaseItem(frame);

	push	eax
	mov	ecx, OFFSET ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A ; DRAMsimII::fbdFrame::freeFramePool
	call	?releaseItem@?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAEXPAVfbdFrame@2@@Z ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::releaseItem

; 44   : }

	ret	0
??3fbdFrame@DRAMsimII@@SAXPAX@Z ENDP			; DRAMsimII::fbdFrame::operator delete
; Function compile flags: /Ogtpy
;	COMDAT ??__E?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ
text$yc	SEGMENT
??__E?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ PROC ; `dynamic initializer for 'DRAMsimII::fbdFrame::freeFramePool'', COMDAT

; 23   : Queue<fbdFrame> fbdFrame::freeFramePool(4*POOL_SIZE,true);

	push	1
	push	OFFSET ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A ; DRAMsimII::fbdFrame::freeFramePool
	mov	ecx, 256				; 00000100H
	call	??0?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAE@I_N@Z ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::Queue<DRAMsimII::fbdFrame>
	push	OFFSET ??__F?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ ; `dynamic atexit destructor for 'DRAMsimII::fbdFrame::freeFramePool''
	call	_atexit
	pop	ecx
	ret	0
??__E?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ ENDP ; `dynamic initializer for 'DRAMsimII::fbdFrame::freeFramePool''
; Function compile flags: /Ogtpy
;	COMDAT ??__F?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ
text$yd	SEGMENT
??__F?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ PROC ; `dynamic atexit destructor for 'DRAMsimII::fbdFrame::freeFramePool'', COMDAT
	push	OFFSET ?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A ; DRAMsimII::fbdFrame::freeFramePool
	call	??1?$Queue@VfbdFrame@DRAMsimII@@@DRAMsimII@@QAE@XZ ; DRAMsimII::Queue<DRAMsimII::fbdFrame>::~Queue<DRAMsimII::fbdFrame>
	ret	0
??__F?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ ENDP ; `dynamic atexit destructor for 'DRAMsimII::fbdFrame::freeFramePool''
PUBLIC	?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A ; DRAMsimII::fbdFrame::freeFramePool
?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A DB 028H DUP (?) ; DRAMsimII::fbdFrame::freeFramePool
?freeFramePool$initializer$@fbdFrame@DRAMsimII@@0P6AXXZA DD FLAT:??__E?freeFramePool@fbdFrame@DRAMsimII@@0V?$Queue@VfbdFrame@DRAMsimII@@@2@A@@YAXXZ ; DRAMsimII::fbdFrame::freeFramePool$initializer$
END
