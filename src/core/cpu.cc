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

  auto bmiOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x30, {2, 0x30, AddressMode::relative, InstructionType::BMI, bmiOP}});

  auto bneOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xD0, {2, 0xD0, AddressMode::relative, InstructionType::BNE, bneOP}});

  auto bplOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x10, {2, 0x10, AddressMode::relative, InstructionType::BPL, bplOP}});

  auto brkOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x00, {7, 0x00, AddressMode::implied, InstructionType::BRK, brkOP}});

  auto bvcOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x50, {2, 0x50, AddressMode::relative, InstructionType::BVC, bvcOP}});

  auto bvsOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x70, {2, 0x70, AddressMode::relative, InstructionType::BVS, bvsOP}});

  auto clcOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x18, {2, 0x18, AddressMode::implied, InstructionType::CLC, clcOP}});

  auto cldOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xd8, {2, 0xd8, AddressMode::implied, InstructionType::CLD, cldOP}});

  auto cliOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x58, {2, 0x58, AddressMode::implied, InstructionType::CLI, cliOP}});

  auto clvOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xB8, {2, 0xB8, AddressMode::implied, InstructionType::CLV, clvOP}});

  auto cmpOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xC9, {2, 0xC9, AddressMode::immediate, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xC5, {3, 0xC5, AddressMode::zeropage, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xD5, {4, 0xD5, AddressMode::zeropage_x, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xCD, {4, 0xCD, AddressMode::absolute, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xDD, {4, 0xDD, AddressMode::absolute_x, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xD9, {4, 0xD9, AddressMode::absolute_y, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xC1, {6, 0xC1, AddressMode::x_indirect, InstructionType::CMP, cmpOP}});

  m_instructionMap.insert(
      {0xD1, {5, 0xD1, AddressMode::indirect_y, InstructionType::CMP, cmpOP}});

  auto cpxOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xe0, {2, 0xe0, AddressMode::immediate, InstructionType::CPX, cpxOP}});

  m_instructionMap.insert(
      {0xe4, {3, 0xe4, AddressMode::zeropage, InstructionType::CPX, cpxOP}});

  m_instructionMap.insert(
      {0xec, {4, 0xec, AddressMode::absolute, InstructionType::CPX, cpxOP}});

  auto cpyOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xc0, {2, 0xc0, AddressMode::immediate, InstructionType::CPY, cpyOP}});

  m_instructionMap.insert(
      {0xc4, {3, 0xc4, AddressMode::zeropage, InstructionType::CPY, cpyOP}});

  m_instructionMap.insert(
      {0xcc, {4, 0xcc, AddressMode::absolute, InstructionType::CPY, cpyOP}});

  auto decOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xC6, {5, 0xC6, AddressMode::zeropage, InstructionType::DEC, decOP}});

  m_instructionMap.insert(
      {0xD6, {6, 0xD6, AddressMode::zeropage_x, InstructionType::DEC, decOP}});

  m_instructionMap.insert(
      {0xCE, {6, 0xCE, AddressMode::absolute, InstructionType::DEC, decOP}});

  m_instructionMap.insert(
      {0xDE, {7, 0xDE, AddressMode::absolute_x, InstructionType::DEC, decOP}});

  auto dexOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xCA, {2, 0xCA, AddressMode::implied, InstructionType::DEX, dexOP}});

  auto deyOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x88, {2, 0x88, AddressMode::implied, InstructionType::DEY, deyOP}});

  auto eorOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x49, {2, 0x49, AddressMode::immediate, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x45, {3, 0x45, AddressMode::zeropage, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x55, {4, 0x55, AddressMode::zeropage_x, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x4D, {4, 0x4D, AddressMode::absolute, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x5D, {4, 0x5D, AddressMode::absolute_x, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x59, {4, 0x59, AddressMode::absolute_x, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x41, {6, 0x41, AddressMode::x_indirect, InstructionType::EOR, eorOP}});

  m_instructionMap.insert(
      {0x51, {5, 0x51, AddressMode::indirect_y, InstructionType::EOR, eorOP}});

  auto incOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xE6, {5, 0xE6, AddressMode::zeropage, InstructionType::INC, incOP}});

  m_instructionMap.insert(
      {0xF6, {6, 0xF6, AddressMode::zeropage_x, InstructionType::INC, incOP}});

  m_instructionMap.insert(
      {0xEE, {6, 0xEE, AddressMode::absolute, InstructionType::INC, incOP}});

  m_instructionMap.insert(
      {0xFE, {7, 0xFE, AddressMode::absolute_x, InstructionType::INC, incOP}});

  auto inxOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xE8, {2, 0xE8, AddressMode::implied, InstructionType::INX, inxOP}});

  auto inyOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xC8, {2, 0xC8, AddressMode::implied, InstructionType::INY, inyOP}});

  auto jmpOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x4C, {3, 0x4C, AddressMode::absolute, InstructionType::JMP, jmpOP}});

  m_instructionMap.insert(
      {0x6C, {5, 0x6C, AddressMode::indirect, InstructionType::JMP, jmpOP}});

  auto jsrOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x20, {6, 0x20, AddressMode::absolute, InstructionType::JSR, jsrOP}});

  auto ldaOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xA9, {2, 0xA9, AddressMode::immediate, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xA5, {3, 0xA5, AddressMode::zeropage, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xB5, {4, 0xB5, AddressMode::zeropage_x, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xAD, {4, 0xAD, AddressMode::absolute, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xBD, {4, 0xBD, AddressMode::absolute_x, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xB9, {4, 0xB9, AddressMode::absolute_y, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xA1, {6, 0xA1, AddressMode::x_indirect, InstructionType::LDA, ldaOP}});

  m_instructionMap.insert(
      {0xB1, {5, 0xB1, AddressMode::indirect_y, InstructionType::LDA, ldaOP}});

  auto ldxOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xA2, {2, 0xA2, AddressMode::immediate, InstructionType::LDX, ldxOP}});

  m_instructionMap.insert(
      {0xA6, {3, 0xA6, AddressMode::zeropage, InstructionType::LDX, ldxOP}});

  m_instructionMap.insert(
      {0xB6, {4, 0xB6, AddressMode::zeropage, InstructionType::LDX, ldxOP}});

  m_instructionMap.insert(
      {0xAE, {4, 0xAE, AddressMode::absolute, InstructionType::LDX, ldxOP}});

  m_instructionMap.insert(
      {0xBE, {4, 0xBE, AddressMode::absolute_y, InstructionType::LDX, ldxOP}});

  auto ldyOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xA0, {2, 0xA0, AddressMode::immediate, InstructionType::LDY, ldyOP}});

  m_instructionMap.insert(
      {0xA4, {3, 0xA4, AddressMode::zeropage, InstructionType::LDY, ldyOP}});

  m_instructionMap.insert(
      {0xB4, {4, 0xB4, AddressMode::zeropage_x, InstructionType::LDY, ldyOP}});

  m_instructionMap.insert(
      {0xAC, {4, 0xAC, AddressMode::absolute, InstructionType::LDY, ldyOP}});

  m_instructionMap.insert(
      {0xBC, {4, 0xBC, AddressMode::absolute_x, InstructionType::LDY, ldyOP}});

  auto lsrOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x4A, {2, 0x4A, AddressMode::accumulator, InstructionType::LSR, lsrOP}});

  m_instructionMap.insert(
      {0x46, {5, 0x46, AddressMode::zeropage, InstructionType::LSR, lsrOP}});

  m_instructionMap.insert(
      {0x56, {6, 0x56, AddressMode::zeropage_x, InstructionType::LSR, lsrOP}});

  m_instructionMap.insert(
      {0x4E, {6, 0x4E, AddressMode::absolute, InstructionType::LSR, lsrOP}});

  m_instructionMap.insert(
      {0x5E, {7, 0x5E, AddressMode::absolute_x, InstructionType::LSR, lsrOP}});

  auto nopOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xEA, {2, 0xEA, AddressMode::implied, InstructionType::NOP, nopOP}});

  auto oraOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x09, {2, 0x09, AddressMode::immediate, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x05, {3, 0x05, AddressMode::zeropage, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x15, {4, 0x15, AddressMode::zeropage_x, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x0D, {4, 0x0D, AddressMode::absolute, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x1D,
       {4 , 0x1D, AddressMode::absolute_x, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x19,
       {4 , 0x19, AddressMode::absolute_y, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x01, {6, 0x01, AddressMode::x_indirect, InstructionType::ORA, oraOP}});

  m_instructionMap.insert(
      {0x11,
       {5 , 0x11, AddressMode::indirect_y, InstructionType::ORA, oraOP}});

  auto phaOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x48, {3, 0x48, AddressMode::implied, InstructionType::PHA, phaOP}});

  auto phpOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x08, {3, 0x08, AddressMode::implied, InstructionType::PHP, phpOP}});

  auto plaOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x68, {4, 0x68, AddressMode::implied, InstructionType::PLA, plaOP}});

  auto plpOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x28, {4, 0x28, AddressMode::implied, InstructionType::PLP, plpOP}});

  auto rolOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x2A, {2, 0x2A, AddressMode::accumulator, InstructionType::ROL, rolOP}});

  m_instructionMap.insert(
      {0x26, {5, 0x26, AddressMode::zeropage, InstructionType::ROL, rolOP}});

  m_instructionMap.insert(
      {0x36, {6, 0x36, AddressMode::zeropage_x, InstructionType::ROL, rolOP}});

  m_instructionMap.insert(
      {0x2E, {6, 0x2E, AddressMode::absolute, InstructionType::ROL, rolOP}});

  m_instructionMap.insert(
      {0x3E, {7, 0x3E, AddressMode::absolute_x, InstructionType::ROL, rolOP}});

  auto rorOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x6A, {2, 0x6A, AddressMode::accumulator, InstructionType::ROR, rorOP}});

  m_instructionMap.insert(
      {0x66, {5, 0x66, AddressMode::zeropage, InstructionType::ROR, rorOP}});

  m_instructionMap.insert(
      {0x76, {6, 0x76, AddressMode::zeropage_x, InstructionType::ROR, rorOP}});

  m_instructionMap.insert(
      {0x6E, {6, 0x6E, AddressMode::absolute, InstructionType::ROR, rorOP}});

  m_instructionMap.insert(
      {0x7E, {7, 0x7E, AddressMode::absolute_x, InstructionType::ROR, rorOP}});

  auto rtiOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x40, {6, 0x40, AddressMode::implied, InstructionType::RTI, rtiOP}});

  auto rtsOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x60, {6, 0x60, AddressMode::implied, InstructionType::RTS, rtsOP}});

  auto sbcOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xE9, {2, 0xE9, AddressMode::immediate, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xE5, {3, 0xE5, AddressMode::zeropage, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xF5, {4, 0xF5, AddressMode::zeropage_x, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xED, {4, 0xED, AddressMode::absolute, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xFD,
       {4 , 0xFD, AddressMode::absolute_x, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xF9,
       {4 , 0xF9, AddressMode::absolute_y, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xE1, {6, 0xE1, AddressMode::x_indirect, InstructionType::SBC, sbcOP}});

  m_instructionMap.insert(
      {0xF1,
       {5 , 0xF1, AddressMode::indirect_y, InstructionType::SBC, sbcOP}});

  auto secOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x38, {2, 0x38, AddressMode::implied, InstructionType::SEC, secOP}});

  auto sedOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xF8, {2, 0xF8, AddressMode::implied, InstructionType::SED, sedOP}});

  auto seiOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x78, {2, 0x78, AddressMode::implied, InstructionType::SEI, seiOP}});

  auto staOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x85, {3, 0x85, AddressMode::zeropage, InstructionType::STA, staOP}});

  m_instructionMap.insert(
      {0x95, {4, 0x95, AddressMode::zeropage_x, InstructionType::STA, staOP}});

  m_instructionMap.insert(
      {0x8D, {4, 0x8D, AddressMode::absolute, InstructionType::STA, staOP}});

  m_instructionMap.insert(
      {0x9D, {5, 0x9D, AddressMode::absolute_x, InstructionType::STA, staOP}});

  m_instructionMap.insert(
      {0x99, {5, 0x99, AddressMode::absolute_y, InstructionType::STA, staOP}});

  m_instructionMap.insert(
      {0x81, {6, 0x81, AddressMode::x_indirect, InstructionType::STA, staOP}});

  m_instructionMap.insert(
      {0x91, {6, 0x91, AddressMode::indirect_y, InstructionType::STA, staOP}});

  auto stxOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x86, {3, 0x86, AddressMode::zeropage, InstructionType::STX, stxOP}});

  m_instructionMap.insert(
      {0x96, {4, 0x96, AddressMode::zeropage, InstructionType::STX, stxOP}});

  m_instructionMap.insert(
      {0x8E, {4, 0x8E, AddressMode::absolute, InstructionType::STX, stxOP}});

  auto styOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x84, {3, 0x84, AddressMode::zeropage, InstructionType::STY, styOP}});

  m_instructionMap.insert(
      {0x94, {4, 0x94, AddressMode::zeropage_x, InstructionType::STY, styOP}});

  m_instructionMap.insert(
      {0x8C, {4, 0x8C, AddressMode::absolute, InstructionType::STY, styOP}});

  auto taxOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xAA, {2, 0xAA, AddressMode::implied, InstructionType::TAX, taxOP}});

  auto tayOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xA8, {2, 0xA8, AddressMode::implied, InstructionType::TAY, tayOP}});

  auto tsxOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0xBA, {2, 0xBA, AddressMode::implied, InstructionType::TSX, tsxOP}});

  auto txaOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x8A, {2, 0x8A, AddressMode::implied, InstructionType::TXA, txaOP}});

  auto txsOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x9A, {2, 0x9A, AddressMode::implied, InstructionType::TXS, txsOP}});

  auto tyaOP = [this](uint16_t &memoryValue) {
    if (!this->CarryFlag()) {
      this->m_PC += static_cast<int16_t>(static_cast<int8_t>(memoryValue));
    }
  };

  m_instructionMap.insert(
      {0x98, {2, 0x98, AddressMode::implied, InstructionType::TYA, tyaOP}});
}

void CPU::Run() {
  while (true) {
    auto op = m_memory[0];
  }
}
