#pragma once
#include "system.h"

using namespace std;
#define INVALID_INPUT (-1)
#define SYSTEM_FULL (-1)
#define INVALID_ID (-1)
#define SUCCESS 1
#define USER_IN_SYS (-2)

class SubscriptionHandler {
	User  members[N_MEMBER];
	int members_num;



	// returns index of first empty cell in members array unless the members array is full or the id is already in the system
	int FindFreeSpace(int id, int* return_index)const;

	bool IndexInRange(int index)const;
public:
	SubscriptionHandler() = default;

	// checks if the id = 'user_id' is new to the system and if there is free space in the system; If so, registers the new user and returns the index of the 
	// members array associated to the new user. If the user was already in the system or the system was full, returns -1
	int SubscribeUser(int user_id);

	// updates the members array by removing the user associated with the system id = 'sys_id'. 
	bool UnSubscribeUser(int sys_id);

	// check if the users were responsive in the past cycle; If not, print to error log. In any case, reset the ALIVE signals sent to the system for next cycle
	void LogUnResponsiveUsersAndReset();

	// check if all the users were responsive during the past cycle; If so, return true
	bool AreAllMembersResponsive();

	// sets the 'keep_alive_received' parameter to true for the user associated with system id = 'sys_id'
	void UpdateAliveStatus(int sys_id);

	int GetMembersNum()const;

};


