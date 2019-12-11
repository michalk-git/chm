

#include "bsp.h"
#include "watchdog.h"
#include <iostream>
#include "test.h"
#define COMMAND_NUM 2
using namespace Core_Health;





//............................................................................
int main(int argc, char* argv[]) {
	static QP::QEvt const* chmQueueSto[N_MEMBER];
	static QP::QEvt const* memberQueueSto[N_MEMBER][100*N_MEMBER];
	static QP::QSubscrList subscrSto[MAX_PUB_SIG];
	static QF_MPOOL_EL(MemberEvt) smlPoolSto[2 * N_MEMBER];
	static QF_MPOOL_EL(InitializationEvt) medPoolSto[N_MEMBER];
	QP::QF::init();  // initialize the framework and the underlying RT kernel

	Core_Health::BSP::init(argc, argv); // initialize the BSP

	QP::QF::psInit(subscrSto, Q_DIM(subscrSto)); // init publish-subscribe

	// initialize event pools...
	QP::QF::poolInit(smlPoolSto,
		sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
	QP::QF::poolInit(medPoolSto,
		sizeof(medPoolSto), sizeof(medPoolSto[0]));
	Subscribe sub = Subscribe(0, 3,0);
	Subscribe sub2 = Subscribe(1, 3,0);
	Subscribe sub3 = Subscribe(5, 1,1);
	UnSubscribe unsub = UnSubscribe(10, 1);
	Subscribe c1 = Subscribe(0, 0,2);
	Deactivate c2 = Deactivate(21, 4, 2);
	Command c3 = Subscribe(5, 1,3);
	Command c4 = Subscribe(5, 1, 3);
	Command c5 = Subscribe(5, 1, 3);
	Command c6 = Subscribe(5, 1, 3);
	// initialize command array (specifying the commands to be executed during program run)
	Command* subscription_array[N_MEMBER][COMMAND_NUM] = {
		&sub,&sub2,
		&sub3,&unsub,
		& c1,& c2,
		& c3,& c4,
		&c5, &c6
	
	};

	
	// start the active objects...
	for (uint8_t n = 0U; n < N_MEMBER; ++n) {

		// create initialization event for AO_HealthMonitor active object associated with INIT_SIG signal
		InitializationEvt* init_evt = Q_NEW(InitializationEvt, INIT_SIG);
		init_evt->cmd_or_wait = SubscriptionCmdOrWait(subscription_array[n], COMMAND_NUM);
		AO_Member[n]->start((uint8_t)(n + 1U),
			memberQueueSto[n], Q_DIM(memberQueueSto[n]),
			(void*)0, 0U, init_evt);
	}


	AO_HealthMonitor->start((uint8_t)(N_MEMBER + 1U),
		chmQueueSto, Q_DIM(chmQueueSto),
		(void*)0, 0U);





	return QP::QF::run(); // run the QF application
}
