#pragma once

#include "system.h"
using namespace Core_Health;

typedef enum CMD {
	SUBSCRIBE,
	UNSUBSCRIBE,
	DEACTIVATE
}Command_t;

class Command {
public:
	int time;
	Command_t cmd;
	Command(Command_t subscription_cmd, int cmd_time) : cmd(subscription_cmd), time(cmd_time) {};
	virtual void ExecuteCmd() = 0;
};

class UnSubscribeCommand : public Command {
public:
	UnSubscribeCommand(Command_t subscription_cmd, int cmd_time, int new_user_id) : Command(subscription_cmd, cmd_time) {};
	virtual void ExecuteCmd() {
		MemberEvt* user_evt = Q_NEW(MemberEvt, UNSUBSCRIBE_SIG);                                                                   //deleted index assignment without changing interfaces
		AO_HealthMonitor->postFIFO(user_evt);

	}
};

class SubscribeCommand : public Command {
public:
	int user_id;
	SubscribeCommand(Command_t subscription_cmd, int cmd_time, int new_user_id) : Command(subscription_cmd, cmd_time), user_id(new_user_id) {};
	virtual void ExecuteCmd() {
		//create an event with the new user's id and post it to the CHM system
		RegisterNewUserEvt* user_evt = Q_NEW(RegisterNewUserEvt, SUBSCRIBE_SIG);
		user_evt->id = user_id;
		AO_HealthMonitor->postFIFO(user_evt);
	}
};

class DeactivateCommand : public Command {
public:
	int cycle_num;
	int index;

	DeactivateCommand(Command_t subscription_cmd, int cmd_time, int deactivation_cycles, int member_index) : Command(subscription_cmd, cmd_time), cycle_num(deactivation_cycles), index(member_index) {};
	virtual void ExecuteCmd() {
		// create new event to notify the appropriate member and post it
		DeactivationEvt* evt = Q_NEW(DeactivationEvt, DEACTIVATE_SIG);
		evt->period_num = cycle_num;
		AO_Member[index]->postFIFO(evt);

		printf("Member %d is deactivated for %d cycles\n ", index, cycle_num);
	}
};

class SubscriptionCmdOrWait {
	Command** subscribe_cmd_vec;
	int subscribe_cmd_vec_len;
	int curr_index;
	int curr_time_in_ticks;

	void CallNextCmd();

public:
	SubscriptionCmdOrWait() : subscribe_cmd_vec(NULL), subscribe_cmd_vec_len(0), curr_index(0), curr_time_in_ticks(0) {};
	SubscriptionCmdOrWait(Command** command_vec, int length) {
		subscribe_cmd_vec = command_vec;
		curr_index = 0;
		curr_time_in_ticks = 0;
		subscribe_cmd_vec_len = length;
	}
	void operator () () {
		// Convert ticks to seconds
		int seconds = (curr_time_in_ticks / (Core_Health::BSP::TICKS_PER_SEC));
		// check if we reached end of tests
		if (curr_index == (subscribe_cmd_vec_len - 1)) return;
		// check if we need to run a new command yet
		else if (seconds >= subscribe_cmd_vec[curr_index]->time) {
			CallNextCmd();
		}

	}
};




class InitializationEvt : public QP::QEvt {
public:
	SubscriptionCmdOrWait cmd_or_wait;
};

