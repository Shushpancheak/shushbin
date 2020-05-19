.586
.model flat, stdcall
.xmm
option casemap: none

include C:\masm32\include\masm32.inc
include C:\masm32\include\msvcrt.inc
include C:\masm32\include\kernel32.inc
include C:\masm32\include\user32.inc

includelib C:\masm32\lib\masm32.lib
includelib C:\masm32\lib\msvcrt.lib
includelib C:\masm32\lib\kernel32.lib
includelib C:\masm32\lib\user32.lib

.data
  print_fmt_str db "%lf", 13, 10, 0
  scanf_fmt_str db "%lf", 0
  val           dq 0

.code
start:
  
  
  push 0
  call ExitProcess
end start