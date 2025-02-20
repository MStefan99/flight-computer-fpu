#include "util.hpp"


static uint32_t ticks {0};


extern "C" {
	void SysTick_Handler() {
		++ticks;
	}
}

void util::init() {
	uint32_t calibration = *((uint32_t*)0x00806020);

	// SUPC setup
	SUPC_REGS->SUPC_VREF = SUPC_VREF_TSEN(1)   // Enable temperature sensor
	                     | SUPC_VREF_SEL_1V0;  // Set 1.0V as a reference
	SUPC_REGS->SUPC_VREG = SUPC_VREG_SEL_BUCK | SUPC_VREG_ENABLE(1);
	while (!(SUPC_REGS->SUPC_STATUS & SUPC_INTFLAG_VREGRDY_Msk));

	// NVM setup
	NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_MANW(1)  // Use NVM in manual write mode
	                            | NVMCTRL_CTRLB_RWS(4);  // Use 4 wait states for NVM

	// PM setup
	PM_REGS->PM_PLCFG = PM_PLCFG_PLSEL_PL2;                 // Enter PL2
	while (!(PM_REGS->PM_INTFLAG & PM_INTFLAG_PLRDY_Msk));  // Wait for the transition to complete

	// OSCCTRL setup
	OSCCTRL_REGS->OSCCTRL_OSC16MCTRL = OSCCTRL_OSC16MCTRL_ENABLE(1)  // Enable OSC16M
	                                 | OSCCTRL_OSC16MCTRL_FSEL_16;   // Set frequency to 16MHz
	OSCCTRL_REGS->OSCCTRL_DFLLVAL =
	    OSCCTRL_DFLLVAL_COARSE((calibration >> 26u) & 0x3f) | OSCCTRL_DFLLVAL_FINE(512);  // Load calibration value
	OSCCTRL_REGS->OSCCTRL_DFLLCTRL = OSCCTRL_DFLLCTRL_ENABLE(1)                           // Enable DFLL48M
	                               | OSCCTRL_DFLLCTRL_MODE(0);                            // Run in open-loop mode

	// GLCK setup
	GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_GENEN(1)      // Enable GCLK 0
	                           | GCLK_GENCTRL_SRC_DFLL48M;  // Set DFLL48M as a source
	GCLK_REGS->GCLK_GENCTRL[1] = GCLK_GENCTRL_GENEN(1)      // Enable GCLK 1
	                           | GCLK_GENCTRL_SRC_OSC16M;   // Set OSC16M as a source
	GCLK_REGS->GCLK_GENCTRL[2] = GCLK_GENCTRL_GENEN(1)      // Enable GCLK 2
	                           | GCLK_GENCTRL_SRC_OSC16M    // Set OSC16M as a source
	                           | GCLK_GENCTRL_DIVSEL_DIV2   // Set division mode (2^(x+1))
	                           | GCLK_GENCTRL_DIV(5);       // Divide by 64 (2^(5+1))

	// SysTick setup
	SysTick_Config(48000);

	// NVIC setup
	__DMB();
	__enable_irq();
	NVIC_EnableIRQ(SysTick_IRQn);

	// ADC setup
	GCLK_REGS->GCLK_PCHCTRL[30] = GCLK_PCHCTRL_CHEN(1)     // Enable ADC clock
	                            | GCLK_PCHCTRL_GEN_GCLK2;  // Set GCLK2 as a clock source
	ADC_REGS->ADC_REFCTRL = ADC_REFCTRL_REFSEL_INTREF;     // Set ADC reference voltage
	ADC_REGS->ADC_INPUTCTRL = ADC_INPUTCTRL_MUXNEG_GND     // Set GND as negative input
	                        | ADC_INPUTCTRL_MUXPOS_TEMP;   // Set temperature sensor as positive input
	ADC_REGS->ADC_INTENSET = ADC_INTFLAG_RESRDY(1);        // Enable result ready interrupt
	ADC_REGS->ADC_CTRLA = ADC_CTRLA_ENABLE(1);             // Enable ADC
}

// Fast inverse square root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float util::invSqrt(float x) {
	float halfx = 0.5f * x;

	union {
		float f;
		long  i;
	} conv = {x};

	conv.i = 0x5f3759df - (conv.i >> 1);
	conv.f *= 1.5f - (halfx * conv.f * conv.f);
	conv.f *= 1.5f - (halfx * conv.f * conv.f);
	return conv.f;
}

uint32_t util::getTime() {
	return ticks;
}

void util::sleep(uint32_t ms) {
	uint32_t t {ticks + ms};

	while (ticks < t) {
		__WFI();
	}
}
