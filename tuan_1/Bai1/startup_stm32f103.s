.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global main
 
.section .isr_vector,"a",%progbits
.word 0x20005000
.word Reset_Handler

.section .text
Reset_Handler:
    bl main

loop:
    b loop
