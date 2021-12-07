#pragma once
#include <bitset>
#include <sys/endian.h>

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


  public:
  /*
   * 需要根据寻址模式获取值并且递增当前
   */
  uint8_t GetValue(){

  }
public:
  CPU();
  void test() {}
};

/*
 * 状态寄存器
*/
class StatusRegister {
private:
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

public:

};
