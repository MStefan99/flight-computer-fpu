#include "nvm.hpp"

const nvm::_internal::Rows nvm::_internal::rows __attribute__((aligned(FLASH_ROW_SIZE), keep, space(prog))) {};
const nvm::Options*        nvm::options {&nvm::_internal::rows.options};

uint8_t        nvm::_internal::rowCopy[FLASH_ROW_SIZE] {};
const uint8_t* nvm::_internal::modifiedRow {nullptr};

static void nvmWaitUntilReady();
static void nvmRowErase(uint32_t address);
static void nvmPageWrite();

static void nvmWaitUntilReady() {
	while ((NVMCTRL_REGS->NVMCTRL_INTFLAG & NVMCTRL_INTFLAG_READY_Msk) != NVMCTRL_INTFLAG_READY_Msk);
}

static void nvmRowErase(const uint8_t* address) {
	NVMCTRL_REGS->NVMCTRL_ADDR = reinterpret_cast<uint32_t>(address) >> 1u;
	NVMCTRL_REGS->NVMCTRL_CTRLA = NVMCTRL_CTRLA_CMD_ER_Val | NVMCTRL_CTRLA_CMDEX_KEY;

	nvmWaitUntilReady();
}

static void nvmPageWrite(const uint8_t* address) {
	NVMCTRL_REGS->NVMCTRL_ADDR = reinterpret_cast<uint32_t>(address) >> 1u;
	NVMCTRL_REGS->NVMCTRL_CTRLA = NVMCTRL_CTRLA_CMD_WP_Val | NVMCTRL_CTRLA_CMDEX_KEY;

	nvmWaitUntilReady();
}

void nvm::load() {
	for (uint8_t i {0}; i < data::mixesNumber; ++i) {
		data::usbMixesResponse.mixes[i] = nvm::options->mixes[i];
	}

	for (uint8_t i {0}; i < data::outputChannelNumber; ++i) {
		data::usbTrimsResponse.trims[i] = nvm::options->trims[i];
	}

	for (uint8_t i {0}; i < data::outputChannelNumber * 2; ++i) {
		data::usbLimitsResponse.limits[i] = nvm::options->limits[i];
	}

	for (uint8_t i {0}; i < data::pidNumber; ++i) {
		data::usbPIDsResponse.coefficients[i] = nvm::options->pidCoefficients[i];
	}
}

void nvm::write() {
	if (_internal::modifiedRow == nullptr) {
		return;  // No rows modified, nothing to do
	}

	nvmRowErase(_internal::modifiedRow);  // Erasing modified row

	for (uint8_t i {0}; i < 4; ++i) {  // Writing all 4 pages
		auto* page {_internal::modifiedRow + i * FLASH_PAGE_SIZE};
		util::copy(                                                                       // Copying a page
		    (uint32_t*)(page),                                                            // Original page
		    reinterpret_cast<const uint32_t*>(_internal::rowCopy + i * FLASH_PAGE_SIZE),  // Page copy
		    FLASH_PAGE_SIZE / sizeof(uint32_t)  // Copying the entire page in 32-bit operations
		);
		nvmPageWrite(page);
	}

	_internal::modifiedRow = nullptr;
}
