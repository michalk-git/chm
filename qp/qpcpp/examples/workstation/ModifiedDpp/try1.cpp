#include "qpcpp.h"
#include "dpp.h"
#include "bsp.h"

//${AOs::AO_new} ..............................................................
class NewAO : public QP::QActive {
public:
	NewAO() :QP::QActive(&initial) {};
protected:
	Q_STATE_DECL(initial);
	Q_STATE_DECL(hi);

};

namespace DPP {

	//${AOs::AO_newAO} ...........................................................
	QP::QActive* const AO_newAO = &newAO::inst; // "opaque" pointer to newAO AO

} // namespace DPP




Q_STATE_DEF(NewAO, initial) {
	QP::QState status_;
	switch (e->sig) {
		//${AOs::NewAO::SM::entry}
	case Q_ENTRY_SIG: {
		QS_FUN_DICTIONARY(&NewAO::initial);
		QS_FUN_DICTIONARY(&initial);
		subscribe(HI_SIG);
        status_ = Q_RET_HANDLED;
		break;
	}
	 //${AOs::newAO::SM::hi}
	case HI_SIG: {
		TableEvt* pe = Q_NEW(TableEvt, HI_SIG);
		pe->philoNum = 100;
		//AO_Table->POST(pe, this);
		uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
		BSP::displayPhilStat(n, HI);
		status_ = Q_RET_HANDLED;
		
		break;
	}
	 return status_;
					 

}


