.386
.Model Flat, StdCall
Option Casemap :None
Include \masm32\include\msvcrt.inc
Include \masm32\include\windows.inc
Include \masm32\include\masm32rt.inc
IncludeLib \masm32\lib\msvcrt.lib
.data
a dword ?
d dword 10 DUP(?)
.code
_search proc
push ebp
mov ebp,esp
sub esp,4
sub esp,12
mov eax,1
mov dword ptr [ebp-4-8],eax
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-4-4],eax
mov eax,dword ptr[ebp-4-4]
mov ecx,dword ptr[ebp-8-4]
cmp eax,ecx
jne sig_1
mov eax,1
mov dword ptr [ebp-4-4],eax
mov esp,ebp
pop ebp
ret 4
jmp sig_2
sig_1:sig_2:mov eax,1
mov dword ptr [ebp-4-12],eax
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-4-4],eax
mov  eax, dword ptr[ebp - 4-4]
mov  ecx, dword ptr[ebp - 4-12]
sub eax,ecx
mov dword ptr [ebp-4-8],eax
push dword ptr [ebp-4-8]
call _search
mov dword ptr [ebp-4-8],eax
mov  eax, dword ptr[ebp - 4-4]
mov  ecx, dword ptr[ebp - 4-8]
imul ecx
mov dword ptr[ebp-4],eax
mov esp,ebp
pop ebp
ret 4
_search endp
_main proc
push ebp
mov ebp,esp
sub esp,4
sub esp,4
mov eax,10
mov dword ptr [ebp-4-4],eax
push dword ptr [ebp-4-4]
call _search
mov dword ptr [ebp-4-4],eax
mov  eax, dword ptr[ebp - 4-4]
mov dword ptr[ebp-4],eax
mov esp,ebp
pop ebp
ret 0
_main endp
end _main
