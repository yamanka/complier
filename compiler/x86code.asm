.386
.Model Flat, StdCall
Option Casemap :None
Include \masm32\include\msvcrt.inc
Include \masm32\include\windows.inc
Include \masm32\include\masm32rt.inc
IncludeLib \masm32\lib\msvcrt.lib
.data
n dword 10 DUP(?)
.code
_search proc
push ebp
mov ebp,esp
sub esp,0
sub esp,4
mov eax,1
mov dword ptr [ebp+0+4],eax
mov esp,ebp
pop ebp
ret 12
_search endp
_main proc
push ebp
mov ebp,esp
sub esp,12
sub esp,12
mov eax,1
mov dword ptr [ebp+12+4],eax
push dword ptr [ebp+12+4]
mov eax,1
mov dword ptr [ebp+12+4],eax
push dword ptr [ebp+12+4]
mov eax,2
mov dword ptr [ebp+12+4],eax
push dword ptr [ebp+12+4]
mov eax,3
mov dword ptr [ebp+12+4],eax
push dword ptr [ebp+12+4]
call _search
mov dword ptr [ebp+12+4],eax
push dword ptr [ebp+12+4]
mov eax,3
mov dword ptr [ebp+12+4],eax
push dword ptr [ebp+12+4]
call _search
mov dword ptr [ebp+12+4],eax
invoke crt_printf, CTXT("%d"),dword ptr [ebp+4+12]
mov esp,ebp
pop ebp
ret 0
_main endp
end _main
