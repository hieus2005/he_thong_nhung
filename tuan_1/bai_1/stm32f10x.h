typedef unsigned short uint16_t;

typedef struct {
    volatile unsigned int CR;
    volatile unsigned int CFGR;
    volatile unsigned int CIR;
    volatile unsigned int APB2RSTR;
    volatile unsigned int APB1RSTR;
    volatile unsigned int AHBENR;
    volatile unsigned int APB2ENR;
    volatile unsigned int APB1ENR;
} RCC_TypeDef;

typedef struct {
    volatile unsigned int CRL;
    volatile unsigned int CRH;
    volatile unsigned int IDR;
    volatile unsigned int ODR;
    volatile unsigned int BSRR;
    volatile unsigned int BRR;
    volatile unsigned int LCKR;
} GPIO_TypeDef;

#define RCC   ((RCC_TypeDef *)0x40021000)
#define GPIOA ((GPIO_TypeDef *)0x40010800)
#define GPIOC ((GPIO_TypeDef *)0x40011000)

#define STK_CTRL (*(volatile unsigned int*)0xE000E010)
#define STK_LOAD (*(volatile unsigned int*)0xE000E014)
#define STK_VAL  (*(volatile unsigned int*)0xE000E018)

static inline void delay_ms(uint16_t t) {
    STK_LOAD = 999;
    STK_VAL  = 0;
    STK_CTRL = 0x1;
    for (uint16_t i = 0; i < t; i++) {
        while ((STK_CTRL & (1 << 16)) == 0);
    }
    STK_CTRL = 0;
}