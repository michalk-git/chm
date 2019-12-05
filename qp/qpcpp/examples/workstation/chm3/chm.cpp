
#include "qpcpp.h"
#include "system.h"
#include "bsp.h"
#include <iostream>
#include <thread>
#include "watchdog.h"
#include <chrono>

Q_DEFINE_THIS_FILE
 QP::QEQueue msg_queue;

void WatchDogFunction(QP::QEQueue* msg_queue) {
	printf("in start of thread\n");
	WatchDog wd;
	const Core_Health::WdEvt* event;
	std::chrono::system_clock::time_point last_time = std::chrono::system_clock::now();
	while (1) {
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::duration<float, std::milli> dur = now - last_time;
		int secs_in_dur = (dur.count()) / 1000;
		last_time += std::chrono::duration<int>( secs_in_dur );
		wd.Decrement(secs_in_dur);
		while (msg_queue->isEmpty() != true) {
			event = (const Core_Health::WdEvt*)msg_queue->get();
			wd.kick();
		}
		 std::this_thread::sleep_for(std::chrono::milliseconds(500));

		
		
	}
}


// Active object class -------------------------------------------------------
//$declare${AOs::CHM} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace Core_Health {

	//${AOs::CHM} ..............................................................
	class CHM : public QP::QActive {
	public:
		static CHM inst;

	private:
		bool subscribers[N_MEMBER];
		bool subscribers_alive[N_MEMBER+1];
		QP::QTimeEvt timeEvt_update;
		QP::QTimeEvt timeEvt_timer;
		QP::QTimeEvt timeEvt_kick;

	public:
		CHM();


	protected:
		Q_STATE_DECL(initial);
		Q_STATE_DECL(active);

	};


}
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 650U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.5.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::AO_CHM} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace Core_Health {

//${AOs::AO_CHM} ...........................................................
QP::QActive * const AO_CHM = &CHM::inst; // "opaque" pointer to Table AO

} // namespace Core_Health
//$enddef${AOs::AO_CHM} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::CHM} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace Core_Health {

	//${AOs::Table} ..............................................................
	CHM CHM::inst;
	//${AOs::CHM::CHM} .......................................................
	CHM::CHM()
		: QActive(&initial), timeEvt_update(this,UPDATE_SIG,0U),timeEvt_timer(this,TIMEOUT_SIG,0U),timeEvt_kick(this,KICK_SIG,0U)
	{

		//index = N_MEMBER is subscribers_alive array is for the CHM system to signal activity
		subscribers_alive[N_MEMBER] = false;
		//initialize subscribers_alive array to false and subscribers array to true (the default is that all users are subscribers and havn't sent an ALIVE signal yet
		for (int i = 0; i < N_MEMBER ; i++) {
			subscribers_alive[i] = false;
			subscribers[i] = true;
		}

	};

	//${AOs::CHM::SM} ..........................................................
	Q_STATE_DEF(CHM, initial) {
		//${AOs::CHM::SM::initial}
		(void)e; // suppress the compiler warning about unused parameter

		static QP::QEvt const* wdQueueSto[N_MEMBER];
		
		msg_queue.init(wdQueueSto, Q_DIM(wdQueueSto));
		std::thread watchdog_thread(WatchDogFunction, &msg_queue);

		std::cout << "in system: initial" << std::endl;
		
		//subscribe to REQUEST_SIG in order to check CHM AO is still active
		subscribe(REQUEST_UPDATE_SIG);
		//arm time event that fires the signal UPDATE_SIG every T_AO_ALIVE_SEC 
		timeEvt_update.armX(BSP::TICKS_PER_SEC*(Core_Health::CHMConfig_t::T_AO_ALIVE_SEC), BSP::TICKS_PER_SEC * (Core_Health::CHMConfig_t::T_AO_ALIVE_SEC));
		//arm time event that fires the signal TIMEOUT_SIG every second 
		timeEvt_timer.armX(BSP::TICKS_PER_SEC, BSP::TICKS_PER_SEC);
		//arm time event that fires  the signal KICK_SIG every T_UPDATE_WATCHDOG_SEC
		timeEvt_kick.armX(BSP::TICKS_PER_SEC*(Core_Health::CHMConfig_t::T_UPDATE_WATCHDOG_SEC), BSP::TICKS_PER_SEC*(Core_Health::CHMConfig_t::T_UPDATE_WATCHDOG_SEC));
		std::cout << "in system: initial end" << std::endl;
		return tran(&active);
	}
	//${AOs::CHM::SM::active} ..................................................
	Q_STATE_DEF(CHM, active) {

		
		QP::QState status_;
		std::cout << "in system: active" << std::endl;
		switch (e->sig) {

		case UPDATE_SIG: {
			std::cout << "in system: update sig" << std::endl;
			//publish a time event with signal REQUEST_SIG
			QP::QEvt* e = Q_NEW(QP::QEvt, REQUEST_UPDATE_SIG);
			QP::QF::PUBLISH(e, this);
			status_ = Q_RET_HANDLED;
			break;
		}
		case REQUEST_UPDATE_SIG: {
			std::cout << "in system: request sig" << std::endl;
			//if CHM recevied REQUEST_SIG we need to update subscribers_alive array in index = N_MEMBER to signal the CHM AO is active
			subscribers_alive[N_MEMBER] = true;
			status_ = Q_RET_HANDLED;
			break;
		}
		case ALIVE_SIG: {
			std::cout << "in system: alive sig" << std::endl;
			//if received an ALIVE_SIG CHm needs to update the subscribers_alive array in the appropriate index 
			std::cout << " in alive\n" << std::endl;
			subscribers_alive[(Q_EVT_CAST(MemberEvt)->memberNum)] = true;
			status_ = Q_RET_HANDLED;
			break;
		}
		case Q_EXIT_SIG: {
			std::cout << "in system: exit sig" << std::endl;
			timeEvt_update.disarm();
			timeEvt_kick.disarm();
			timeEvt_timer.disarm();
			status_ = Q_RET_HANDLED;
			break;
		}
		/*
		case TIMEOUT_SIG: {
			watchdog--;
			printf("watchdog : %d\n", watchdog);
			status_ = Q_RET_HANDLED;
			break;
		}
		*/
		case NOT_MEMBER_SIG: {
			std::cout << "in system: not member sig" << std::endl;
			//update subscribers array to show that a user has unsubscribed
			subscribers[(Q_EVT_CAST(MemberEvt)->memberNum)] = false;
			status_ = Q_RET_HANDLED;
			break;
		}
		case MEMBER_SIG: {
			std::cout << "in system: member sig" << std::endl;
			//update subscribers array to show a user has subscribed
			subscribers[(Q_EVT_CAST(MemberEvt)->memberNum)] = true;
			//update subscribers_alive array- if a user has subscribed it counts as an ALIVE signal
			subscribers_alive[(Q_EVT_CAST(MemberEvt)->memberNum)] = true;
			status_ = Q_RET_HANDLED;
			break;
		}
		case KICK_SIG: {
			std::cout << "in system: kick sig" << std::endl;
			bool kick = true;
			//pass through subscribers_alive array and check whether any subscribed members aren't responsive.
			//if so print error log and refrain from kicking watchdog
			for (int i = 0; i < N_MEMBER ; i++) {
                //check for each user if he is both subscribed and non-active: if so update kick to false (to make sure the system doesn't kick watchdog) and print to error log
				if (subscribers_alive[i] == false && subscribers[i] == true) {
					kick = false;
					if(i == N_MEMBER) printf("CHM system didn't send ALIVE signal\n");
					else printf("Member %d didn't send ALIVE signal\n", i);
				}
			}
			//check if the chm sys is unresponsive
			if (subscribers_alive[N_MEMBER] == false) kick = false;
			//if all subscribers are alive, kick watchdog
			Core_Health::WdEvt* ev;
			if (kick == true) msg_queue.post(ev,QP::QF_NO_MARGIN);
			//set the subscribers_alive array back to default (false) for next cycle
			for (int i = 0; i < N_MEMBER + 1; i++) subscribers_alive[i] = false;
			status_ = Q_RET_HANDLED;
			break;
		}
		default: {
			std::cout << "in system: default sig" << std::endl;
			status_ = super(&top);
			break;
		}
		}
		return status_;
	}
}