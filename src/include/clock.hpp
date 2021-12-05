#pragma once
#include <chrono>

using namespace std::chrono;
/*
 * 需要根据cycle的数量自行推进当前时间
 * 如果一条指令的时间实际执行的时间超过cycle数量的时间，立即返回，
 * 否则，消耗当前时间使总时间达到指令cycle的时间。返回，也就是只是用来
 * 同步一个函数的时钟。
 *
 */
class Clock {
private:
  // NTSC NES CPU Cycle
  using NESCPUCycle = std::ratio<1, 1789773>;
  using CPUCycle = std::chrono::duration<int, NESCPUCycle>;

  using CPUCycleTimePoint =
      std::chrono::time_point<std::chrono::steady_clock, CPUCycle>;

  CPUCycleTimePoint m_startTimePoint;

  /*
   * 返回当前时间点
   */
  CPUCycleTimePoint Now() {
    return time_point_cast<CPUCycle>(steady_clock::now());
  }

public:
  Clock(){

  };

  /*
   * 开始计时
   */
  void Start() { m_startTimePoint = this->Now(); }

  /*
   * 传参cyclecount
   * 从开始到结束，时间走了对应的cycle数量的时间就返回
   */
  void End(int cylceCount) {
    int count = 0;
    do {
      auto now = this->Now();
      count = (now - m_startTimePoint).count();
    } while (count < cylceCount);
  }
};
