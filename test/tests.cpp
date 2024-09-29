// Copyright 2024 Mironov I.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>  // NOLINT [build/c++11]
#include <cstdint>
#include <thread>  // NOLINT [build/c++11]

#include "TimedDoor.h"

// Mock Timer Client
class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Test Fixture for TimedDoor
class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor door;
  MockTimerClient mockClient;
  Timer timer;

  TimedDoorTest() : door(1), timer() {}

  void SetUp() override { timer.tregister(door.getTimeOut(), &mockClient); }

  void TearDown() override {
    testing::Mock::VerifyAndClearExpectations(&mockClient);
  }
};

TEST_F(TimedDoorTest, UNL_D_TH_TIMEOUT) {
  door.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut()));
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, LOCK_D_TH_UNLOCK) {
  std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
  EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, LOCK_BF_TIMEOUT_TH_OPEN) {
  std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, UNL_MTH) {
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LOCK_MTH) {
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, FST_D_CLO) { EXPECT_FALSE(door.isDoorOpened()); }

TEST_F(TimedDoorTest, UNL_D_TW) {
  door.unlock();
  EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, OPEN_EX) {
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, CLO_BF) { EXPECT_NO_THROW(door.throwState()); }

TEST_F(TimedDoorTest, LOCK_D_TW) {
  EXPECT_THROW(door.lock(), std::logic_error);
}
