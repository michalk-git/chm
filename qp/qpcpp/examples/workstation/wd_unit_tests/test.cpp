#include "pch.h"
#include "watchdog.h"
#include <chrono>


#define TIME_INTERVAL 10

void wait(int seconds) {
	std::chrono::system_clock::time_point last = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point curr = last;
	std::chrono::milliseconds wait_time = std::chrono::seconds(seconds) + std::chrono::milliseconds(100);
	//std::cout << "wait " << wait_time.count() << std::endl;
	while ((curr - last) <= wait_time) {
		curr = std::chrono::system_clock::now();
	}

}

// test if the watchdog is resetted correctly upon initialization to T_WATCHDOG_RESET_SEC
TEST(WatchDogTests,initialization) {
	WatchDog& wd = singleton<WatchDog>::getInstance();
	wd.start();
    EXPECT_EQ(wd.GetCounter().count(), Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC);                
    wd.stop();
 
}

// check if the watchdog counter reaches correct value after TIME_INTERVAL secs
TEST(WatchDogTests, CountDownByTIME_INTERVAL) {
	WatchDog& wd = singleton<WatchDog>::getInstance();
	wd.start();
	wait(TIME_INTERVAL);
	EXPECT_EQ(wd.GetCounter().count(), (Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC - TIME_INTERVAL));
	wd.stop();
}

// check if the watchdog's counter reaches zero after T_WATCHDOG_RESET_SEC secs
TEST(WatchDogTests, CountDownByT_WATCHDOG_RESET_SEC) {
	WatchDog& wd = singleton<WatchDog>::getInstance();
	wd.start();
	wait(Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC);
	EXPECT_EQ(wd.GetCounter().count(), 0);
	wd.stop();
}

// check if a kick restores the watchdog's counter to T_WATCHDOG_RESET_SEC
TEST(WatchDogTests, Kick) {
	WatchDog& wd = singleton<WatchDog>::getInstance();
	wd.start();
	wait(TIME_INTERVAL);
	wd.kick();
	EXPECT_EQ(wd.GetCounter().count(), Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC);
	wd.stop();
}

// check if setting a new reset interval is succecfull
TEST(WatchDogTests, ResetCounter) {
	
	WatchDog& wd = singleton<WatchDog>::getInstance();
	wd.SetResetInterval(TIME_INTERVAL);
	wd.start();
	EXPECT_EQ(wd.GetCounter().count(), TIME_INTERVAL);
	wd.stop();
}

// check if decrementing the counter by TIME_INTERVAL secs is succecfull
TEST(WatchDogTests, Decrement) {

	WatchDog& wd = singleton<WatchDog>::getInstance();
	wd.start();
	wd.Decrement(std::chrono::seconds(TIME_INTERVAL));
	EXPECT_EQ(wd.GetCounter().count(), (Core_Health::CHMConfig_t::T_WATCHDOG_RESET_SEC - TIME_INTERVAL));
	wd.stop();
}