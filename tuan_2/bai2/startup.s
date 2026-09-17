.syntax unified
.cpu cortex-m3
.thumb

.section .isr_vector
.word 0x20005000
.word Reset_Handler

.section .text
.global Reset_Handler
Reset_Handler:
    BL main
    B .
