// Copyright 2024 Mironov I.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>  // NOLINT [build/c++11]
#include <cstdint>
#include <thread>  // NOLINT [build/c++11]

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

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

TEST_F(TimedDoorTest, unlockExpectThrow) {
  door.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut()));
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, lockExpectNoThrow) {
  std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
  EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, lockBeforeTimeout) {
  std::this_thread::sleep_for(std::chrono::seconds(door.getTimeOut() + 1));
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, unlockCheck) {
  door.unlock();
  EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, lockCheck) {
  door.unlock();
  door.lock();
  EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, isDoorCheck) { EXPECT_FALSE(door.isDoorOpened()); }

TEST_F(TimedDoorTest, unlockCheckThrow) {
  door.unlock();
  EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, checkThrow) {
  door.unlock();
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, checkNoThrow) { EXPECT_NO_THROW(door.throwState()); }

TEST_F(TimedDoorTest, lockCheckThrow) {
  EXPECT_THROW(door.lock(), std::logic_error);
}
