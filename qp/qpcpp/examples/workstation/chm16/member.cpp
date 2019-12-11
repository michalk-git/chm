
#include "system.h"
#include <iostream>
#include "test.h"
using namespace std;
Q_DEFINE_THIS_FILE

// Active object class -------------------------------------------------------
//$declare${AOs::Member} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace Core_Health {

//${AOs::Member} ..............................................................
class Member : public QP::QActive {
public:
    static Member inst[N_MEMBER];

private:
	int num_deactivated_cycles;
	int system_id;

	// used for testing 
	SubscriptionCmdOrWait cmd_handler;
	QP::QTimeEvt        timeEvt_tick;
public:
	Member();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(active);

};

} // namespace Core_Health
//$enddecl${AOs::Member} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

namespace Core_Health {

// Local objects -------------------------------------------------------------
static Member l_Member[N_MEMBER];   // storage for all Members


} // namespace Core_Health

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 650U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.5.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::AO_Member[N_Member]} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace Core_Health {

//${AOs::AO_Member[N_Member]} ..................................................
QP::QActive * const AO_Member[N_MEMBER] = { // "opaque" pointers to Member AO
    & Member::inst[0],
    & Member::inst[1],
    & Member::inst[2],
    & Member::inst[3],
    & Member::inst[4]
};




} // namespace Core_Health
//$enddef${AOs::AO_Member[N_Member]} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Member} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace Core_Health {

	//${AOs::Member} ..............................................................
	Member Member::inst[N_MEMBER];
	//${AOs::Member::Member} .......................................................
	Member::Member()
		: QActive(&initial), num_deactivated_cycles(0), timeEvt_tick(this, TICK_SIG, 0U) {};
		
	

	//${AOs::Member::SM} ..........................................................
	Q_STATE_DEF(Member, initial) {
		//${AOs::Member::SM::initial}

		//arm time event that fires the signal TICK_SIG every second
		timeEvt_tick.armX(Core_Health::BSP::TICKS_PER_SEC, Core_Health::BSP::TICKS_PER_SEC);

		// handle initialization event 
		if (e->sig == INIT_SIG) {
			cmd_handler = (Q_EVT_CAST(InitializationEvt)->cmd_or_wait);
		}
		return tran(&active);
	}

	//${AOs::Member::SM::active} ................................................
	Q_STATE_DEF(Member, active) {


		QP::QState status_;
		switch (e->sig) {
		case TICK_SIG: {
			cmd_handler();
			status_ = Q_RET_HANDLED;
			break;
		}

		case MEMBER_SUBSCRIBE_SIG: {
			((QP::QEvt*)e)->sig = SUBSCRIBE_SIG;
			AO_HealthMonitor->postFIFO(e, this);
		}
		case MEMBER_UNSUBSCRIBE_SIG: {
			((QP::QEvt*)e)->sig = UNSUBSCRIBE_SIG;
			AO_HealthMonitor->postFIFO(e, this);
		}

		case REQUEST_UPDATE_SIG: {
			//if a member AO recevied a REQUEST_UPDATE_SIG it needs to post an ALIVE_SIG to the HealthMonitor active object unless it has been deactivated
			if (num_deactivated_cycles == 0) {
				MemberEvt* alive_evt = Q_NEW(MemberEvt, ALIVE_SIG);
				alive_evt->member_num = system_id;
				AO_HealthMonitor->postFIFO(alive_evt, this);
				cout << "member " << system_id << " has sent ALIVE signal" <<endl;
			}
			else --num_deactivated_cycles;
			status_ = Q_RET_HANDLED;
			break;
		}

        
		case SUBSCRIBE_ACKNOLEDGE_SIG: {
			//all users who wish to subscribe to health monitor system will subscribe to the REQUEST_UPDATE_SIG signal
			subscribe(REQUEST_UPDATE_SIG);
			system_id = (Q_EVT_CAST(MemberEvt)->member_num);
			cout << "member " << system_id << " has subscribed" << endl;
			status_ = Q_RET_HANDLED;
			break;
		}
		case UNSUBSCRIBE_ACKNOLEDGE_SIG: {
			//users who wish to unsubscribe will stop receiving REQUEST_UPDATE_SIG signal
			 unsubscribe(REQUEST_UPDATE_SIG);
			 system_id = -1;
			 cout << "member " << system_id << " has unsubscribed" << endl;
			 status_ = Q_RET_HANDLED;
			 break;
		}
		//used for testing purposes
		case DEACTIVATE_SIG: {
			num_deactivated_cycles = (Q_EVT_CAST(DeactivationEvt)->period_num);
			status_ = Q_RET_HANDLED;
			break;
		}
		//${AOs::Member::SM::active}
		case Q_EXIT_SIG: {
			timeEvt_tick.disarm();
			status_ = Q_RET_HANDLED;
			break;
		}

		default: {
			status_ = super(&top);
			break;
		}
		}
		return status_;
	}
}
