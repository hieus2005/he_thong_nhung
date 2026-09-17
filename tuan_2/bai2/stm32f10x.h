typedef unsigned short uint16_t;

// ==== RCC ====
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

// ==== GPIO ====
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

// ==== SysTick ====
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

// ==== USART1 ====
typedef struct {
    volatile unsigned int SR;
    volatile unsigned int DR;
    volatile unsigned int BRR;
    volatile unsigned int CR1;
    volatile unsigned int CR2;
    volatile unsigned int CR3;
    volatile unsigned int GTPR;
} USART_TypeDef;

#define USART1 ((USART_TypeDef *)0x40013800)

// ==== TIM2 ====
typedef struct {
    volatile unsigned int CR1;
    volatile unsigned int CR2;
    volatile unsigned int SMCR;
    volatile unsigned int DIER;
    volatile unsigned int SR;
    volatile unsigned int EGR;
    volatile unsigned int CCMR1;
    volatile unsigned int CCMR2;
    volatile unsigned int CCER;
    volatile unsigned int CNT;
    volatile unsigned int PSC;
    volatile unsigned int ARR;
    unsigned int RESERVED0;
    volatile unsigned int CCR1;
    volatile unsigned int CCR2;
    volatile unsigned int CCR3;
    volatile unsigned int CCR4;
} TIM_TypeDef;

#define TIM2 ((TIM_TypeDef *)0x40000000)

// ==== GPIO Library-style (STD) ====
typedef struct {
    unsigned int GPIO_Pin;
    unsigned int GPIO_Speed;
    unsigned int GPIO_Mode;
} GPIO_InitTypeDef;

#define GPIO_Mode_AF_PP       0xB
#define GPIO_Mode_Out_PP      0x3
#define GPIO_Mode_IN_FLOATING 0x4
#define GPIO_Speed_50MHz      3

static inline void GPIO_Init(GPIO_TypeDef *port, GPIO_InitTypeDef *cfg) {
    for (int pin = 0; pin < 8; pin++) {
        if (cfg->GPIO_Pin & (1 << pin)) {
            port->CRL &= ~(0xF << (pin * 4));
            port->CRL |=  (cfg->GPIO_Mode << (pin * 4));
        }
    }
    for (int pin = 8; pin < 16; pin++) {
        if (cfg->GPIO_Pin & (1 << pin)) {
            port->CRH &= ~(0xF << ((pin - 8) * 4));
            port->CRH |=  (cfg->GPIO_Mode << ((pin - 8) * 4));
        }
    }
}

// ==== TIM Library-style (STD) ====
typedef struct {
    unsigned int TIM_Prescaler;
    unsigned int TIM_Period;
} TIM_TimeBaseInitTypeDef;

static inline void TIM_TimeBaseInit(TIM_TypeDef *tim, TIM_TimeBaseInitTypeDef *cfg) {
    tim->PSC = cfg->TIM_Prescaler;
    tim->ARR = cfg->TIM_Period;
    tim->EGR = 1;
}

typedef struct {
    unsigned int TIM_Pulse;
} TIM_OCInitTypeDef;

static inline void TIM_OC1Init(TIM_TypeDef *tim, TIM_OCInitTypeDef *cfg) {
    tim->CCMR1 &= ~(0xFF << 0);
    tim->CCMR1 |=  (6 << 4) | (1 << 3);
    tim->CCR1 = cfg->TIM_Pulse;
    tim->CCER |= (1 << 0);
}
static inline void TIM_OC2Init(TIM_TypeDef *tim, TIM_OCInitTypeDef *cfg) {
    tim->CCMR1 &= ~(0xFF << 8);
    tim->CCMR1 |=  (6 << 12) | (1 << 11);
    tim->CCR2 = cfg->TIM_Pulse;
    tim->CCER |= (1 << 4);
}
static inline void TIM_OC3Init(TIM_TypeDef *tim, TIM_OCInitTypeDef *cfg) {
    tim->CCMR2 &= ~(0xFF << 0);
    tim->CCMR2 |=  (6 << 4) | (1 << 3);
    tim->CCR3 = cfg->TIM_Pulse;
    tim->CCER |= (1 << 8);
}
static inline void TIM_OC4Init(TIM_TypeDef *tim, TIM_OCInitTypeDef *cfg) {
    tim->CCMR2 &= ~(0xFF << 8);
    tim->CCMR2 |=  (6 << 12) | (1 << 11);
    tim->CCR4 = cfg->TIM_Pulse;
    tim->CCER |= (1 << 12);
}
static inline void TIM_Cmd(TIM_TypeDef *tim, int enable) {
    if (enable) tim->CR1 |= (1 << 0) | (1 << 7);
    else        tim->CR1 &= ~(1 << 0);
}