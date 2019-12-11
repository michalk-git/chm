

#include "bsp.h"
#include "watchdog.h"
#include <iostream>
#include "test.h"
#define COMMAND_NUM 4
using namespace Core_Health;





//............................................................................
int main(int argc, char* argv[]) {
	static QP::QEvt const* chmQueueSto[N_MEMBER];
	static QP::QEvt const* memberQueueSto[N_MEMBER][N_MEMBER];
	static QP::QSubscrList subscrSto[MAX_PUB_SIG];
	static QF_MPOOL_EL(MemberEvt) smlPoolSto[2 * N_MEMBER];
	static InitializationEvt medPoolSto[1];

	QP::QF::init();  // initialize the framework and the underlying RT kernel

	Core_Health::BSP::init(argc, argv); // initialize the BSP

	QP::QF::psInit(subscrSto, Q_DIM(subscrSto)); // init publish-subscribe

	// initialize event pools...
	QP::QF::poolInit(smlPoolSto,
		sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

	QP::QF::poolInit(medPoolSto,
		sizeof(medPoolSto), sizeof(medPoolSto[0]));
	
	// initialize command array (specifying the commands to be executed during program run)
	Command* subscription_array[COMMAND_NUM] = {
		&Subscribe(0, 0),&Subscribe(1, 1),&Subscribe(2, 2),&UnSubscribe(10, 0)
	};

	
	// start the active objects...
	for (uint8_t n = 0U; n < N_MEMBER; ++n) {

		AO_Member[n]->start((uint8_t)(n + 1U),
			memberQueueSto[n], Q_DIM(memberQueueSto[n]),
			(void*)0, 0U);
	}

	// create initialization event for AO_HealthMonitor active object
	InitializationEvt* init_evt = Q_NEW(InitializationEvt, INIT_SIG);
	init_evt->cmd_or_wait = SubscriptionCmdOrWait(subscription_array, COMMAND_NUM);
	AO_HealthMonitor->start((uint8_t)(N_MEMBER + 1U),
		chmQueueSto, Q_DIM(chmQueueSto),
		(void*)0, 0U, init_evt);





	return QP::QF::run(); // run the QF application
}
