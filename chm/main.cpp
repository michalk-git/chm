

#include "bsp.h"
#include "watchdog.h"
#include <iostream>
#include "test.h"
#define TEST_NUM 4;
using namespace Core_Health;





//............................................................................
int main(int argc, char* argv[]) {
	static QP::QEvt const* chmQueueSto[N_MEMBER];
	static QP::QEvt const* memberQueueSto[N_MEMBER][N_MEMBER];
	static QP::QSubscrList subscrSto[MAX_PUB_SIG];
	static QF_MPOOL_EL(MemberEvt) smlPoolSto[2 * N_MEMBER];

	QP::QF::init();  // initialize the framework and the underlying RT kernel

	Core_Health::BSP::init(argc, argv); // initialize the BSP

	QP::QF::psInit(subscrSto, Q_DIM(subscrSto)); // init publish-subscribe

	// initialize event pools...
	QP::QF::poolInit(smlPoolSto,
		sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
	/*
	SubscribeCmd subscription_matrix[N_MEMBER][4] = {
		{SubscribeCmd(SUBSCRIBE,30), SubscribeCmd(SUBSCRIBE,35), SubscribeCmd(UNSUBSCRIBE,40),  SubscribeCmd(UNSUBSCRIBE,40) },
		{SubscribeCmd(SUBSCRIBE,30), SubscribeCmd(SUBSCRIBE,35), SubscribeCmd(UNSUBSCRIBE,40),  SubscribeCmd(UNSUBSCRIBE,40) },
		{SubscribeCmd(SUBSCRIBE,30), SubscribeCmd(SUBSCRIBE,35), SubscribeCmd(UNSUBSCRIBE,40),  SubscribeCmd(UNSUBSCRIBE,40) },
		{SubscribeCmd(SUBSCRIBE,30), SubscribeCmd(SUBSCRIBE,35), SubscribeCmd(UNSUBSCRIBE,40),  SubscribeCmd(UNSUBSCRIBE,40) },
		{SubscribeCmd(SUBSCRIBE,30), SubscribeCmd(SUBSCRIBE,35), SubscribeCmd(UNSUBSCRIBE,40),  SubscribeCmd(UNSUBSCRIBE,40) },

	};

	*/
	// start the active objects...
	for (uint8_t n = 0U; n < N_MEMBER; ++n) {
		InitializationEvt* init_evt = Q_NEW(InitializationEvt, INIT_SIG);
		//init_evt->cmd_or_wait = SubscriptionCmdOrWait(subscription_matrix[n], 4);
		AO_Member[n]->start((uint8_t)(n + 1U),
			memberQueueSto[n], Q_DIM(memberQueueSto[n]),
			(void*)0, 0U, init_evt);
	}

	AO_HealthMonitor->start((uint8_t)(N_MEMBER + 1U),
		chmQueueSto, Q_DIM(chmQueueSto),
		(void*)0, 0U);





	return QP::QF::run(); // run the QF application
}
