#include "../include/cpu.hh"

void CPU::InitInstructionSet() {

  // carry flag 只在进位借位时设置
  auto adc = [this](uint16_t &memoryValue) {
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
  m_instructionMap.insert(
      {0x69, {2, 0x69, AddressMode::immediate, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x65, {3, 0x65, AddressMode::zeropage, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x75, {4, 0x75, AddressMode::zeropage_x, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x6D, {4, 0x6D, AddressMode::absolute, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x7D, {4, 0x7D, AddressMode::absolute_x, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x79, {4, 0x79, AddressMode::absolute_y, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x61, {6, 0x61, AddressMode::x_indirect, InstructionType::ADC, adc}});
  m_instructionMap.insert(
      {0x71, {5, 0x71, AddressMode::indirect_y, InstructionType::ADC, adc}});

  // And memeory with accumulator
  auto andExecutor = [this](uint16_t &memoryValue) {
    m_AC = static_cast<uint8_t>(memoryValue & m_AC);
    this->NegativeFlag() = m_AC & 0x80;
    this->ZeroFlag() = !m_AC;
  };
  m_instructionMap.insert(
      {0x29,
       {2, 0x29, AddressMode::immediate, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x25,
       {3, 0x25, AddressMode::zeropage, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x35,
       {4, 0x35, AddressMode::zeropage_x, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x2D,
       {4, 0x2D, AddressMode::absolute, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x3D,
       {4, 0x3D, AddressMode::absolute_x, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x39,
       {4, 0x39, AddressMode::absolute_y, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x21,
       {6, 0x21, AddressMode::x_indirect, InstructionType::AND, andExecutor}});
  m_instructionMap.insert(
      {0x31,
       {5, 0x31, AddressMode::indirect_y, InstructionType::AND, andExecutor}});

  //左移一位
  auto aslExecutor = [this](uint16_t &memoryValue) {
    memoryValue <<= 1;
    this->CarryFlag() = memoryValue & 0x0100;
    this->NegativeFlag() = memoryValue & 0x80;
    this->ZeroFlag() = !(memoryValue & 0xFF);
  };
  m_instructionMap.insert(
      {0x0A,
       {2, 0x0A, AddressMode::accumulator, InstructionType::ASL, aslExecutor}});
  m_instructionMap.insert(
      {0x06,
       {5, 0x06, AddressMode::zeropage, InstructionType::ASL, aslExecutor}});
  m_instructionMap.insert(
      {0x16,
       {6, 0x16, AddressMode::zeropage_x, InstructionType::ASL, aslExecutor}});
  m_instructionMap.insert(
      {0x0E,
       {6, 0x0E, AddressMode::absolute, InstructionType::ASL, aslExecutor}});
  m_instructionMap.insert(
      {0x1E,
       {7, 0x1E, AddressMode::absolute_x, InstructionType::ASL, aslExecutor}});

  // Carry Clear branch
  auto bccOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x90, {2, 0x90, AddressMode::relative, InstructionType::BCC, bccOP}});

  // branch on Carry Set
  auto bcsOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xB0, {2, 0xB0, AddressMode::relative, InstructionType::BCS, bcsOP}});

  // branch on Result Zero
  auto beqOP = [this](uint16_t &memoryValue) {
    if (!this->ZeroFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };
  m_instructionMap.insert(
      {0xF0, {2, 0xF0, AddressMode::relative, InstructionType::BEQ, beqOP}});

  // Test Bits in Memory with Accumulator
  /*
   * bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V);
   the zero-flag is set to the result of operand AND accumulator.

   A AND M, M7 -> N, M6 -> V
   N	Z	C	I	D	V
   M7	+	-	-	-	M6
   */
  auto bitOP = [this](uint16_t &memoryValue) {
    uint16_t result = memoryValue & m_AC;
    NegativeFlag() = result & 0x80;
    OverflowFlag() = result & 0x40;
    this->ZeroFlag() = !(result & 0xFF);
  };
  m_instructionMap.insert(
      {0x24, {3, 0x24, AddressMode::zeropage, InstructionType::BIT, bitOP}});
  m_instructionMap.insert(
      {0x2C, {4, 0x2C, AddressMode::absolute, InstructionType::BIT, bitOP}});
}


void CPU::Run(){
  while (true) {
    auto op=m_memory[0];
  }
}
