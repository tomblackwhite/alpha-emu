#include "../include/cpu.hh"

void CPU::InitInstructionSet() {

  // carry flag 只在进位借位时设置
  auto adc = [this](uint8_t memoryValue) {
    uint16_t result = this->m_AC + memoryValue + this->m_SR[0];

    //设置carray flag
    this->m_SR[0] = result & 0x0100;

    /*
      当结果超过+127 小于-128时溢出
    */
  };
}
