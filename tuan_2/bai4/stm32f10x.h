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
#define GPIOB ((GPIO_TypeDef *)0x40010C00)
#define GPIOC ((GPIO_TypeDef *)0x40011000)

typedef unsigned short uint16_t;