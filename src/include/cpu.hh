#pragma once
#include <bitset>
#include <functional>
#include <cstdint>
#include <vector>
#include <unordered_map>

using std::uint16_t;
using std::uint8_t;
//指令
struct Instruction{
  //周期数
  int m_cycleCount;

  //指令的值
  uint8_t m_operatorCode;

  //执行实际的命令并设置相关寄存器
  //在初始化时自动捕获当前上下文变量
  std::function<void(uint8_t memoryValue)> m_executor;

};


/*
** CPU负责读取内存中的命令
** 依次读取命令并执行命令并且设置相应
** 的寄存器，状态之类的。
** 需要根据命令读取内存中的数据，根据寻址方式，获取操作数，设置对应
** 的pc count
*/
class CPU {
private:
  // memory instructions

  /*
   * status register
   * N V - B D I Z C from bit 7 to bit 0
   * N ... Negative
   * V ... Overflow
   * - ...ignored
   * B ...Break
   * D ... Decimal (use BCD for arithmetics)
   * I ... Interrupt (IRQ disable)
   * Z ... Zero
   * C ... Carry
   */
  std::bitset<8> m_SR = 0;

  //accumulator register
  uint8_t m_AC=0;

  //X register
  uint8_t m_X=0;

  //Y register
  uint8_t m_Y=0;

  //stack pointer
  uint8_t m_statckPointer=0;

  //program counter
  uint16_t m_PC=0;

  /*
   * 内存
   */
  std::vector<uint8_t> memory;


  /*指令集*/
  std::unordered_map<uint8_t, Instruction> m_instructionSet;

public:
  //初始化指令集设置相关参数
  void InitInstructionSet();

  /*
   * 需要根据寻址模式获取值并且递增当前的program counter
   * 首先获取opertor code 然后根据operator code 判断
   * 寻址方式，获取实际的值
   */
  uint8_t GetValue() { return 0; }

public:
  CPU();
  void test() {}
};
