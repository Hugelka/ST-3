// Copyright 2024 Mironov I.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>  // NOLINT [build/c++11]
#include <cstdint>
#include <thread>  // NOLINT [build/c++11]

#include "TimedDoor.h"

// Мок для интерфейса TimerClient
class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Мок для класса Door
class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

// Тестовый класс для TimedDoor
class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  MockTimerClient* mockTimerClient;

  void SetUp() override {
    door = new TimedDoor(1);
    mockTimerClient = new MockTimerClient();
  }

  void TearDown() override {
    delete door;
    delete mockTimerClient;
  }
};

// Тесты на TimedDoor

TEST_F(TimedDoorTest, DoorShouldBeClosedInitially) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeUnlocked) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeLocked) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsExceptionIfDoorStillOpen) {
  door->unlock();
  EXPECT_THROW(
      {
        std::this_thread::sleep_for(std::chrono::seconds(2));
      },
      std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionIfDoorClosedBeforeTimeout) {
  door->unlock();
  std::this_thread::sleep_for(
      std::chrono::milliseconds(500));
  door->lock();
  EXPECT_NO_THROW({
    std::this_thread::sleep_for(
        std::chrono::seconds(1));
  });
}

// Тестовый класс для DoorTimerAdapter
class DoorTimerAdapterTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  DoorTimerAdapter* adapter;

  void SetUp() override {
    door = new TimedDoor(1);
    adapter = new DoorTimerAdapter(*door);
  }

  void TearDown() override {
    delete door;
    delete adapter;
  }
};

TEST_F(DoorTimerAdapterTest, AdapterCallsThrowStateIfDoorOpen) {
  door->unlock();
  EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, AdapterDoesNotThrowIfDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(adapter->Timeout());
}

// Тестовый класс для Timer
class TimerTest : public ::testing::Test {
 protected:
  Timer* timer;
  MockTimerClient* mockClient;

  void SetUp() override {
    timer = new Timer();
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete timer;
    delete mockClient;
  }
};

TEST_F(TimerTest, TimerCallsTimeoutAfterInterval) {
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer->tregister(1, mockClient);
  std::this_thread::sleep_for(
      std::chrono::seconds(2));
}

TEST_F(TimerTest, TimerDoesNotCallTimeoutImmediately) {
  EXPECT_CALL(*mockClient, Timeout()).Times(0);
  timer->tregister(1, mockClient);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(500));
}
