; Listing generated by Microsoft (R) Optimizing Compiler Version 18.00.30501.0 

include listing.inc

INCLUDELIB MSVCRTD
INCLUDELIB OLDNAMES

PUBLIC	?g_strs@@3PAUTableString@@A			; g_strs
PUBLIC	?value@?$integral_constant@_N$0A@@std@@2_NB	; std::integral_constant<bool,0>::value
PUBLIC	?value@?$integral_constant@_N$00@std@@2_NB	; std::integral_constant<bool,1>::value
;	COMDAT ?value@?$integral_constant@_N$00@std@@2_NB
CONST	SEGMENT
?value@?$integral_constant@_N$00@std@@2_NB DB 01H	; std::integral_constant<bool,1>::value
CONST	ENDS
;	COMDAT ?value@?$integral_constant@_N$0A@@std@@2_NB
CONST	SEGMENT
?value@?$integral_constant@_N$0A@@std@@2_NB DB 00H	; std::integral_constant<bool,0>::value
CONST	ENDS
_DATA	SEGMENT
?g_strs@@3PAUTableString@@A DD 00H			; g_strs
	DB	00H
	ORG $+255
	DD	01H
	DB	'Moonface', 00H
	ORG $+247
	DD	02H
	DB	'hello', 0dH, 'Copyright 2014 syuilo.', 00H
	ORG $+227
	DD	08H
	DB	'Error loading suite.', 00H
	ORG $+235
	DD	09H
	DB	'Error releasing suite.', 00H
	ORG $+233
	ORG $+1820
_DATA	ENDS
PUBLIC	GetStringPtr
; Function compile flags: /Odtp
; File c:\users\syuilo\documents\visual studio 2013\projects\after_effects_cs5_sdk_win\adobe after effects cs5 win sdk\examples\template\moonface\skeleton_strings.cpp
_TEXT	SEGMENT
strNum$ = 8
GetStringPtr PROC

; 41   : {

	mov	DWORD PTR [rsp+8], ecx

; 42   : 	return g_strs[strNum].str;

	movsxd	rax, DWORD PTR strNum$[rsp]
	imul	rax, rax, 260				; 00000104H
	lea	rcx, OFFSET FLAT:?g_strs@@3PAUTableString@@A ; g_strs
	lea	rax, QWORD PTR [rcx+rax+4]

; 43   : }

	ret	0
GetStringPtr ENDP
_TEXT	ENDS
END
