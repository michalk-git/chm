#include "SubscriptionHandler.h"
#include <stdio.h>



int SubscriptionHandler::FindFreeSpace(int id, int* return_index)const {
	int found_free_cell = SYSTEM_FULL;
	*return_index = SYSTEM_FULL;
	bool found = false;
	//pass through the members array and check if there is an empty cell; ie the ID = 'UNIDENTIFIED_ID'. If found, return the corresponding index. Else, return 'SYSTEM_FULL' ( = -1)
	for (int i = 0; i < N_MEMBER; ++i) {
		// if we found an empty cell we update 'return_index' to hold the associated index
		if ((members[i].id == UNIDENTIFIED_ID) && (!found)) {
			*return_index = i;
			found_free_cell = SUCCESS;
			found = true;
		}
		// if we found the id already in the members array, we return the associated index and return false;
		if (members[i].id == id) {
			*return_index = i;
			return USER_IN_SYS;
		}
	}
	return found_free_cell;
}




int SubscriptionHandler::SubscribeUser(int user_id) {
	int empty_cell_index = SYSTEM_FULL;
	int status = FindFreeSpace(user_id, &empty_cell_index);
	if (status == USER_IN_SYS) PRINT_LOG("User %d is already in the system\n", user_id);
	else if(status == SYSTEM_FULL) PRINT_LOG("System full\n");  
	else {	
		//if we found an index in the members array not associated with another user, register the new subscriber and increase members count
		members[empty_cell_index].id = user_id;
		members_num++;
		//the subscription is an ALIVE signal
		members[empty_cell_index].keep_alive_received = true;
		}
	return empty_cell_index;
}


bool SubscriptionHandler::UnSubscribeUser(int index) {
	if (IndexInRange(index) == false) return false;
	// update subscribers array to show that a user has unsubscribed and decrease members count
	if (members[index].id != UNIDENTIFIED_ID) {
		members[index].id = UNIDENTIFIED_ID;
		members_num--;
		// set 'keep_alive_received' to false
		members[index].keep_alive_received = false;
		return true;
	}
	return false;
}



void SubscriptionHandler::LogUnResponsiveUsersAndReset() {
	bool subscriber_is_unresponsive = false;
	//for each user set the keep_alive_received to false for next cycle 
	for (int i = 0; i < N_MEMBER; ++i) {
		// A subscriber is unresponsive if he's registered and hasn't sent an ALIVE signal
		subscriber_is_unresponsive = ((members[i].id != UNIDENTIFIED_ID) && (members[i].keep_alive_received == false));
		if (subscriber_is_unresponsive) PRINT_LOG("Watchdog wasn't kicked because member id %d didn't send ALIVE signal\n", members[i].id); 
		// reset the 'keep_alive_received' parameter for all users 
		members[i].keep_alive_received = false;
	}
}

bool SubscriptionHandler::AreAllMembersResponsive() {
	bool subscriber_is_unresponsive = false;
	// For each member check if there has been an ALIVE signal in the current cycle
	for (int i = 0; i < N_MEMBER; ++i) {
		subscriber_is_unresponsive = ((members[i].id != UNIDENTIFIED_ID) && (members[i].keep_alive_received == false));
		if (subscriber_is_unresponsive) return false;
	}
	return true;
}

void SubscriptionHandler::UpdateAliveStatus(int sys_id) {
	if(IndexInRange(sys_id) == false) return;
	// update the members array in the appropriate index
	members[sys_id].keep_alive_received = true;
	
}

bool SubscriptionHandler::IndexInRange(int index)const {
	if ((index < 0) || (index >= N_MEMBER)) return false;
	return true;
}

int SubscriptionHandler::GetMembersNum() const{
	return members_num;
}