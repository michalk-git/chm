#pragma once
#include <chrono>
#include "system.h"
using namespace std::chrono;
using namespace Core_Health;


class Command {
public:
	int time;
	Command(int cmd_time) : time(cmd_time) {};
	virtual ~Command() = default;
	virtual void ExecuteCmd() {};
};

class UnSubscribe : public Command {
	int sys_id;
public:
	UnSubscribe(int cmd_time, int member_index) : Command(cmd_time),sys_id(member_index) {};
	virtual void ExecuteCmd() override{
		UnSubscribeUserEvt* user_evt = Q_NEW(UnSubscribeUserEvt, MEMBER_UNSUBSCRIBE_SIG);    //deleted index assignment without changing interfaces
		user_evt->sender_id = sys_id;
		AO_Member[sys_id]->postFIFO(user_evt);

	}
};

class Subscribe : public Command {
	int sys_id;
	int user_id;
public:
	Subscribe(int cmd_time, int new_user_id, int member_index) : Command(cmd_time), user_id(new_user_id), sys_id(member_index) {};
	virtual void ExecuteCmd() override {
		//create an event with the new user's id and post it to the appropriate Member Ao
		SubscribeUserEvt* user_evt = Q_NEW(SubscribeUserEvt, MEMBER_SUBSCRIBE_SIG);
		user_evt->id = user_id;
		user_evt->sender_id = sys_id;
		AO_Member[sys_id]->postFIFO(user_evt);
	}
};

class Deactivate : public Command {
public:
	int cycle_num;
	int index;

	Deactivate( int cmd_time, int deactivation_cycles, int member_index) : Command(cmd_time), cycle_num(deactivation_cycles), index(member_index) {};
	virtual void ExecuteCmd() override {
		// create new event to notify the appropriate member and post it
		DeactivationEvt* evt = Q_NEW(DeactivationEvt, DEACTIVATE_SIG);
		evt->period_num = cycle_num;
		AO_Member[index]->postFIFO(evt);
		PRINT_LOG("Member %d is deactivated for %d cycles\n ", index, cycle_num);
		
	}
};

class SubscriptionCmdOrWait {
	Command** subscribe_cmd_vec;
	int subscribe_cmd_vec_len;
	int curr_index;
	system_clock::time_point initial_time;

	void CallNextCmd() {
		subscribe_cmd_vec[curr_index++]->ExecuteCmd();
	}

public:
	SubscriptionCmdOrWait() : subscribe_cmd_vec(NULL), subscribe_cmd_vec_len(0), curr_index(0), initial_time(system_clock::now()) {};
	SubscriptionCmdOrWait(Command** command_vec, int length) {
		subscribe_cmd_vec = command_vec;
		curr_index = 0;
		initial_time = system_clock::now();
		subscribe_cmd_vec_len = length;
	}
	void operator () () {
		
		system_clock::time_point curr_time = system_clock::now();
		
		// find the duration of time since initialization of class in duration
		duration<int> seconds = duration_cast<std::chrono::seconds>(curr_time - initial_time);
		// check if we reached end of tests
		if (curr_index == (subscribe_cmd_vec_len )) {  return; }
		// check if we need to run a new command yet
		else if (seconds.count() >= (subscribe_cmd_vec[curr_index]->time)) {
			CallNextCmd();
			
			
			
		}
		
		
	}
};




class InitializationEvt : public QP::QEvt {
public:
	SubscriptionCmdOrWait cmd_or_wait;
};

