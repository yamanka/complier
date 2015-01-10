.386
.Model Flat, StdCall
Option Casemap :None
Include \masm32\include\msvcrt.inc
Include \masm32\include\windows.inc
Include \masm32\include\masm32rt.inc
IncludeLib \masm32\lib\msvcrt.lib
.data
var_data dword 10 DUP(?)
.code
_main proc
push ebx
push esi
push edi
push ebp
mov ebp,esp
sub esp,20
sub esp,16
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-4]
mov eax,0
mov dword ptr [ebp-20-4],eax
mov eax,0
mov dword ptr[ebp-8],eax
sig_1:
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-4],eax
mov eax,dword ptr[ebp- 4]
mov dword ptr [ebp-20-8],eax
mov eax,dword ptr[ebp-4-20]
mov ecx,dword ptr[ebp-8-20]
cmp eax,ecx
jge sig_2
invoke crt_scanf,CTXT("%d"),addr [esp-4]
mov ESI,[esp-4]
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-12],eax
mov  eax,ESI
mov edx,dword ptr[ebp-20-12]
sal edx,2
mov var_data[edx],eax
mov eax,1
mov dword ptr [ebp-20-8],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr [ebp-20-4],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr[ebp-8],eax
jmp sig_1
sig_2:
mov eax,0
mov dword ptr [ebp-20-4],eax
mov eax,0
mov dword ptr[ebp-8],eax
sig_3:
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-4],eax
mov eax,dword ptr[ebp- 4]
mov dword ptr [ebp-20-8],eax
mov eax,dword ptr[ebp-4-20]
mov ecx,dword ptr[ebp-8-20]
cmp eax,ecx
jge sig_4
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-8],eax
mov eax,dword ptr[ebp- 8]
mov  EDI,eax
mov edx,dword ptr[ebp-20-8]
sal edx,2
mov eax,var_data[edx]
mov dword ptr [ebp-20-4],eax
mov edx,dword ptr[ebp-20-8]
sal edx,2
mov eax,var_data[edx]
mov  ESI,eax
sig_5:
mov  eax,EDI
mov dword ptr [ebp-20-4],eax
mov eax,dword ptr[ebp- 4]
mov dword ptr [ebp-20-8],eax
mov eax,dword ptr[ebp-4-20]
mov ecx,dword ptr[ebp-8-20]
cmp eax,ecx
jge sig_6
mov  eax,EDI
mov dword ptr [ebp-20-12],eax
mov edx,dword ptr[ebp-20-12]
sal edx,2
mov eax,var_data[edx]
mov dword ptr [ebp-20-4],eax
mov  eax,ESI
mov dword ptr [ebp-20-8],eax
mov eax,dword ptr[ebp-4-20]
mov ecx,dword ptr[ebp-8-20]
cmp eax,ecx
jg sig_7
mov  eax,EDI
mov dword ptr [ebp-20-4],eax
mov  eax,EDI
mov  EBX,eax
mov edx,dword ptr[ebp-20-4]
sal edx,2
mov eax,var_data[edx]
mov  ESI,eax
jmp sig_8
sig_7:
sig_8:
mov  eax,EDI
mov dword ptr [ebp-20-12],eax
mov eax,1
mov dword ptr [ebp-20-8],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr [ebp-20-4],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov  EDI,eax
jmp sig_5
sig_6:
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-12],eax
mov  eax,EBX
mov dword ptr [ebp-20-16],eax
mov edx,dword ptr[ebp-20-12]
sal edx,2
mov eax,var_data[edx]
mov  EDI,eax
mov edx,dword ptr[ebp-20-12]
sal edx,2
mov eax,var_data[edx]
mov edx,dword ptr[ebp-20-16]
sal edx,2
mov var_data[edx],eax
mov edx,dword ptr[ebp-20-16]
sal edx,2
mov eax,var_data[edx]
mov edx,dword ptr[ebp-20-12]
sal edx,2
mov var_data[edx],eax
mov eax,1
mov dword ptr [ebp-20-8],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr [ebp-20-4],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr[ebp-8],eax
jmp sig_3
sig_4:
mov eax,0
mov dword ptr [ebp-20-4],eax
mov eax,0
mov dword ptr[ebp-8],eax
sig_9:
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-4],eax
mov eax,dword ptr[ebp- 4]
mov dword ptr [ebp-20-8],eax
mov eax,dword ptr[ebp-4-20]
mov ecx,dword ptr[ebp-8-20]
cmp eax,ecx
jge sig_10
invoke crt_printf,addr literal(" ")
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-8],eax
mov edx,dword ptr[ebp-20-8]
sal edx,2
mov eax,var_data[edx]
mov dword ptr [ebp-20-4],eax
invoke crt_printf, addr literal("%d"),dword ptr [ebp-4-20]
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-20-12],eax
mov eax,1
mov dword ptr [ebp-20-8],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr [ebp-20-4],eax
mov  eax, dword ptr[ebp - 20-12]
mov  ecx, dword ptr[ebp - 20-8]
add eax,ecx
mov dword ptr[ebp-8],eax
jmp sig_9
sig_10:
mov esp,ebp
pop ebp
pop edi
pop esi
pop ebx
ret 0
_main endp
end _main
