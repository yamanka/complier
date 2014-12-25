.386
.Model Flat, StdCall
Option Casemap :None
Include \masm32\include\msvcrt.inc
Include \masm32\include\windows.inc
Include \masm32\include\masm32rt.inc
IncludeLib \masm32\lib\msvcrt.lib
.data
var_var1 dword ?
var_var2 dword ?
var_var3 dword ?
var_var4 dword ?
.code
_max proc
push ebp
mov ebp,esp
sub esp,4
sub esp,8
mov eax, dword ptr[ebp+16]
mov dword ptr [ebp-4-4],eax
mov eax, dword ptr[ebp+12]
mov dword ptr [ebp-4-8],eax
mov eax,dword ptr[ebp-4-4]
mov ecx,dword ptr[ebp-8-4]
cmp eax,ecx
jl sig_1
mov eax, dword ptr[ebp+16]
mov dword ptr [ebp-4-4],eax
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-4-8],eax
mov eax,dword ptr[ebp-4-4]
mov ecx,dword ptr[ebp-8-4]
cmp eax,ecx
jl sig_3
mov eax, dword ptr[ebp+16]
mov dword ptr [ebp-4-4],eax
mov  eax, dword ptr[ebp - 4-4]
mov dword ptr[ebp-4],eax
jmp sig_4
sig_3:
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-4-4],eax
mov  eax, dword ptr[ebp - 4-4]
mov dword ptr[ebp-4],eax
sig_4:
jmp sig_2
sig_1:
mov eax, dword ptr[ebp+12]
mov dword ptr [ebp-4-4],eax
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-4-8],eax
mov eax,dword ptr[ebp-4-4]
mov ecx,dword ptr[ebp-8-4]
cmp eax,ecx
jl sig_5
mov eax, dword ptr[ebp+12]
mov dword ptr [ebp-4-4],eax
mov  eax, dword ptr[ebp - 4-4]
mov dword ptr[ebp-4],eax
jmp sig_6
sig_5:
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-4-4],eax
mov  eax, dword ptr[ebp - 4-4]
mov dword ptr[ebp-4],eax
sig_6:
sig_2:
mov eax,dword ptr[ebp- 4]
mov dword ptr [ebp-4-4],eax
mov esp,ebp
pop ebp
ret 12
_max endp
_add1 proc
push ebp
mov ebp,esp
sub esp,0
sub esp,8
sig_7:
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-0-4],eax
mov eax,0
mov dword ptr [ebp-0-8],eax
mov eax,dword ptr[ebp-4-0]
mov ecx,dword ptr[ebp-8-0]
cmp eax,ecx
jle sig_8
mov eax, dword ptr[ebp+16]
mov dword ptr [ebp-0-4],eax
mov eax, dword ptr[ebp+12]
mov dword ptr [ebp-0-8],eax
mov  eax, dword ptr[ebp - 0-4]
mov  ecx, dword ptr[ebp - 0-8]
add eax,ecx
mov dword ptr [ebp-0-4],eax
mov  eax, dword ptr[ebp - 0-4]
mov dword ptr [ebp+16],eax
mov eax, dword ptr[ebp+8]
mov dword ptr [ebp-0-4],eax
mov eax,1
mov dword ptr [ebp-0-8],eax
mov  eax, dword ptr[ebp - 0-4]
mov  ecx, dword ptr[ebp - 0-8]
sub eax,ecx
mov dword ptr [ebp-0-4],eax
mov  eax, dword ptr[ebp - 0-4]
mov dword ptr [ebp+8],eax
jmp sig_7
sig_8:
mov eax, dword ptr[ebp+16]
mov dword ptr [ebp-0-4],eax
mov esp,ebp
pop ebp
ret 12
_add1 endp
_main proc
push ebp
mov ebp,esp
sub esp,32
sub esp,12
mov eax,123
mov dword ptr [ebp-32-4],eax
mov  eax, dword ptr[ebp - 32-4]
mov var_var3,eax
mov eax,43
mov dword ptr [ebp-32-4],eax
mov  eax, dword ptr[ebp - 32-4]
mov var_var4,eax
invoke crt_printf,CTXT("input three numbers: ")
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-4]
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-8]
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-12]
mov eax,dword ptr[ebp- 4]
mov dword ptr [ebp-32-4],eax
push dword ptr [ebp-32-4]
mov eax,dword ptr[ebp- 8]
mov dword ptr [ebp-32-4],eax
push dword ptr [ebp-32-4]
mov eax,dword ptr[ebp- 12]
mov dword ptr [ebp-32-4],eax
push dword ptr [ebp-32-4]
call _max
mov dword ptr [ebp-32-4],eax
mov  eax, dword ptr[ebp - 32-4]
mov var_var1,eax
invoke crt_printf,CTXT("the max number:")
mov eax,var_var1
mov dword ptr [ebp-32-4],eax
invoke crt_printf, CTXT("%d"),dword ptr [ebp-4-32]
invoke crt_printf,CTXT("input the base: ")
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-16]
invoke crt_printf,CTXT("input the step: ")
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-20]
invoke crt_printf,CTXT("input the number:")
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-24]
mov eax,dword ptr[ebp- 16]
mov dword ptr [ebp-32-4],eax
push dword ptr [ebp-32-4]
mov eax,dword ptr[ebp- 20]
mov dword ptr [ebp-32-4],eax
push dword ptr [ebp-32-4]
mov eax,dword ptr[ebp- 24]
mov dword ptr [ebp-32-4],eax
push dword ptr [ebp-32-4]
call _add1
mov dword ptr [ebp-32-4],eax
mov  eax, dword ptr[ebp - 32-4]
mov dword ptr[ebp-28],eax
invoke crt_printf,CTXT("the result: ")
mov eax,dword ptr[ebp- 28]
mov dword ptr [ebp-32-4],eax
invoke crt_printf, CTXT("%d"),dword ptr [ebp-4-32]
mov eax,var_var4
mov dword ptr [ebp-32-4],eax
invoke crt_printf, CTXT("%d"),dword ptr [ebp-4-32]
invoke crt_printf,CTXT("input the choice: 1 2 3 , 0 for return ")
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-32]
sig_9:
mov eax,dword ptr[ebp- 32]
mov dword ptr [ebp-32-4],eax
mov eax,0
mov dword ptr [ebp-32-8],eax
mov eax,dword ptr[ebp-4-32]
mov ecx,dword ptr[ebp-8-32]
cmp eax,ecx
je sig_10
mov eax,dword ptr[ebp- 32]
mov dword ptr [ebp-32-4],eax
mov eax,1
mov dword ptr [ebp-32-8],eax
mov eax,dword ptr[ebp-4-32]
mov ecx,dword ptr[ebp-8-32]
cmp eax,ecx
jne sig_12
invoke crt_printf,CTXT("good")
jmp sig_11
sig_12:
mov eax,2
mov dword ptr [ebp-32-8],eax
mov eax,dword ptr[ebp-4-32]
mov ecx,dword ptr[ebp-8-32]
cmp eax,ecx
jne sig_13
invoke crt_printf,CTXT("better")
jmp sig_11
sig_13:
mov eax,3
mov dword ptr [ebp-32-8],eax
mov eax,dword ptr[ebp-4-32]
mov ecx,dword ptr[ebp-8-32]
cmp eax,ecx
jne sig_14
invoke crt_printf,CTXT("best")
jmp sig_11
sig_14:
sig_11:
invoke crt_scanf,CTXT("%d"),addr dword ptr [ebp-32]
jmp sig_9
sig_10:
mov esp,ebp
pop ebp
ret 0
_main endp
end _main
