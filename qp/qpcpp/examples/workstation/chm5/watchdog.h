#pragma once
#ifndef WD_CPP_
#define WD_CPP_



#include <thread>
#include "system.h"
#include "singleton.h"
#include <mutex>
#include <chrono>


class WatchDog :public singleton<WatchDog>{
	std::chrono::duration<int> counter;
	std::thread watchdog;
	static void WatchDogFunction();
	std::mutex mtx;
	WatchDog() : counter(Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC),running(true) {};
	friend singleton<WatchDog>;
	bool running;
public:
	
	void start() {
		running = true;
		counter = std::chrono::duration<int>(Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC);
		watchdog = std::thread(&WatchDogFunction);
	}
	
	void stop() {
		running = false;
		watchdog.join();
	}
	void kick() {
		mtx.lock();
		counter = std::chrono::duration<int>(Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC);
		mtx.unlock();
	}
	void SetResetInterval(unsigned int interval) {
		Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC = interval;
	}
	void Decrement(std::chrono::duration<int> amount) {
		mtx.lock();
		counter -= (amount);
		mtx.unlock();
	}
	std::chrono::duration<int> GetCounter()const {
		return counter;
	}


};

#endif