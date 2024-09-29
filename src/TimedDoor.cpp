// Copyright 2024 Mironov I.

#include "./TimedDoor.h"

#include <chrono>
#include <stdexcept>
#include <thread>

// Реализация DoorTimerAdapter
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    // Если дверь все еще открыта, выбрасываем исключение
    door.throwState();
  }
}

// Реализация TimedDoor
TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() { return isOpened; }

void TimedDoor::unlock() {
  isOpened = true;
  // Активируем таймер, когда дверь открыта
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() { isOpened = false; }

int TimedDoor::getTimeOut() const { return iTimeout; }

void TimedDoor::throwState() {
  throw std::runtime_error("Door is still open!");
}

// Реализация Timer
void Timer::sleep(int seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Timer::tregister(int timeout, TimerClient* client) {
  std::thread([timeout, client, this]() {
    sleep(timeout);
    client->Timeout();
  }).detach();
}
