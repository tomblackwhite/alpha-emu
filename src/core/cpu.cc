#include "../include/cpu.hh"
#include <sys/endian.h>

void CPU::InitInstructionSet() {

  // carry flag 只在进位借位时设置
  auto adc = [this](uint8_t memoryValue) {
    uint16_t result = this->m_AC + memoryValue + this->CarryFlag();

    //设置carray flag
    this->CarryFlag() = result & 0x0100;

    /*
      当结果超过+127 小于-128时溢出
      只有同时为正数或者为负数时才会溢出
      符号位 符号位 结果符号位 overflow flag
      1 1 0 --> 1
      0 0 1 --> 1
      可以利用异或来做，前两个符号相同，最后一个符号和前两个不同，
      这种方式的公式和利用真值表得到的公式是等价的，
      前两个同或。也就是前两个符号相同。也就是前两个等价。这样两个等价的符号，在后面的逻辑运算中，可以
      互相替换。
    */
    auto ac = this->m_AC;
    bool overFlowFlag = (~(ac ^ memoryValue)) & (memoryValue ^ result) & 0x80;
    this->OverflowFlag() = overFlowFlag;

    this->NegativeFlag() = result & 0x80;
    this->CarryFlag() = result & 0x0100;
    this->ZeroFlag() = !(result & 0xFF);

    this->m_AC = result & 0xFF;
  };
  m_instructionMap.insert({0x69,{2,0x69,AddressMode::immediate,adc}});
}
