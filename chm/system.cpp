#include "system.h"



unsigned int Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC = 60;
unsigned int Core_Health::CHMConfig_t::T_AO_ALIVE_SEC = 10;
unsigned int Core_Health::CHMConfig_t::T_UPDATE_WATCHDOG_SEC = 20;



QP::QTimeEvtCtr Core_Health::ConvertSecondsToTicks(unsigned int seconds) {
	return (seconds * (Core_Health::BSP::TICKS_PER_SEC));
}
