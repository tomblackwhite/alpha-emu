#pragma once
#include <bitset>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

using std::uint16_t;
using std::uint8_t;
using std::int8_t;
using std::int16_t;

/* 寻址方式 6502为小端 */
enum class AddressMode {

  // accumulator impiled single byte instruction
  // operand is AC (implied single byte instruction)
  accumulator,

  // operand is address $HHLL
  absolute,

  // OPC $LLHH,X operand is address;
  // effective address is address incremented by X with carry
  absolute_x,

  // OPC $LLHH,Y	operand is address;
  // effective address is address incremented by Y with carry **
  absolute_y,

  // OPC #$BB	operand is byte BB
  immediate,

  // OPC	operand implied
  implied,

  // OPC ($LLHH)	operand is address;
  // effective address is contents of word at address: C.w($HHLL)
  indirect,

  // OPC ($LL,X)	operand is zeropage address;
  // effective address is word in (LL + X, LL + X + 1), inc. without carry:
  // C.w($00LL + X)
  // An 8-bit zero-page address and the X register are added, without carry
  //(if the addition overflows, the address wraps around within page 0).
  x_indirect,

  // OPC ($LL),Y	operand is zeropage address;
  // effective address is word in (LL, LL + 1) incremented by Y with carry:
  // C.w($00LL) + Y
  indirect_y,

  // OPC $BB	branch target is PC + signed offset BB ***
  relative,

  // OPC $LL	operand is zeropage address (hi-byte is zero, address = $00LL)
  zeropage,

  // OPC $LL,X	operand is zeropage address;
  // effective address is address incremented by X without carry **
  zeropage_x,

  // OPC $LL,Y	operand is zeropage address;
  // effective address is address incremented by Y without carry **
  zeropage_y

  /*
   * 所以不带进位的加法，不会影响到地址位的高位。
   * 通常来说, increments of 16-bit addresses include a carry,
   * increments of zeropage addresses don't.
   * 这个和累加器的carry bit 无关
   */
};

enum class InstructionType {
  ADC,AND,ASL,BCC,BCS,BEQ,BIT,BMI,BNE,BPL,BRK,BVC,BVS,CLC,CLD,CLI,CLV,CMP,
  CPX,CPY,DEC,DEX,DEY,EOR,INC,INX,INY,JMP,JSR,LDA,LDX,LDY,LSR,NOP,ORA,PHA,
  PHP,PLA,PLP,ROL,ROR,RTI,RTS,SBC,SEC,SED,SEI,STA,STX,STY,TAX,TAY,TSX,TXA,
  TXS,TYA
};
//指令
struct Instruction {
  //周期数
  int m_cycleCount;

  //指令的值
  uint8_t m_operatorCode;

  AddressMode m_addressMode;

  InstructionType m_instructionType;
  //执行实际的命令并设置相关寄存器
  //在初始化时自动捕获当前上下文变量
  std::function<void(uint16_t &memoryValue)> m_executor;
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

  // accumulator register
  uint8_t m_AC = 0;

  // X register
  uint8_t m_X = 0;

  // Y register
  uint8_t m_Y = 0;

  // stack pointer
  uint8_t m_statckPointer = 0;

  // program counter
  uint16_t m_PC = 0;

  /*
   * 内存
   */
  std::vector<uint8_t> m_memory;

  /*指令集*/
  std::unordered_map<uint8_t, Instruction> m_instructionMap;

public:
  //初始化指令集设置相关参数
  void InitInstructionSet();

  //开始执行内存中的代码
  void Run();
  /*
   * 需要根据寻址模式获取值并且递增当前的program counter
   * 首先获取opertor code 然后根据operator code 判断
   * 寻址方式，获取实际的值
   */
  uint8_t GetValue() { return 0; }

public:
  CPU() { InitInstructionSet(); };
  void test() {}

private:
  auto NegativeFlag() { return m_SR[7]; }
  auto OverflowFlag() { return m_SR[6]; }
  auto BreakFlag() { return m_SR[4]; }
  auto DecimalFlag() { return m_SR[3]; }
  auto InterruptFlag() { return m_SR[2]; }
  auto ZeroFlag() { return m_SR[1]; }
  auto CarryFlag() { return m_SR[0]; }

  /*寻址方式*/
  /*获取指令使用的有效地址或者能直接使用的值*/
  uint16_t GetAddressOrMemoryValue(AddressMode mode) {
    uint16_t result = 0;
    switch (mode) {
    case AddressMode::accumulator: {
      //取累加器中的值
      result = m_X;
      break;
    }
    case AddressMode::absolute: {
      //取有效地址
      result = m_memory[m_PC] + (m_memory[m_PC + 1] << 4);
      m_PC += 2;
      break;
    }
    case AddressMode::absolute_x: {
      //取有效地址
      result = m_memory[m_PC] + (m_memory[m_PC + 1] << 4) + m_X;
      m_PC += 2;
      break;
    }
    case AddressMode::absolute_y: {
      //取有效地址
      result = m_memory[m_PC] + (m_memory[m_PC + 1] << 4) + m_Y;
      m_PC += 2;
      break;
    }
    case AddressMode::immediate: {
      //直接取值
      result = m_memory[m_PC];
      m_PC += 1;
      break;
    }
    case AddressMode::implied: {
      //直接返回
      break;
    }
    case AddressMode::indirect: {
      //取有效地址
      uint16_t middleAddress = m_memory[m_PC] + (m_memory[m_PC + 1] << 4);
      result = m_memory[middleAddress] + (m_memory[middleAddress + 1] << 4);
      m_PC += 2;
      break;
    }
    case AddressMode::x_indirect: {
      //取有效地址
      uint16_t middleAddress = m_memory[m_PC] + m_X;
      middleAddress &= 0xFF;
      result = m_memory[middleAddress] + (m_memory[middleAddress + 1] << 4);
      m_PC += 1;
      break;
    }
    case AddressMode::indirect_y: {
      //取有效地址
      uint16_t middleAddress = m_memory[m_PC] + (m_memory[m_PC + 1] << 4);
      result = middleAddress + m_Y;
      m_PC += 1;
      break;
    }
    case AddressMode::relative: {
      //取偏移量
      result = m_memory[m_PC];
      m_PC += 1;
      break;
    }
    case AddressMode::zeropage: {
      //取地址
      result = m_memory[m_PC];
      m_PC += 1;
      break;
    }
    case AddressMode::zeropage_x: {
      //取地址
      result = m_memory[m_PC] + m_X;
      result &= 0xFF;
      m_PC += 1;
      break;
    }
    case AddressMode::zeropage_y: {
      //取地址
      result = m_memory[m_PC] + m_Y;
      result &= 0xFF;
      m_PC += 1;
      break;
    }
    default: {
      static_assert(true, "未知的寻址类型");
    }
    }
    return result;
  }

  /*获取根据寻址模式获取的值，获取指令执行时需要传的参数。*/
  uint16_t GetInstructionParameter(uint16_t addressModeValue) { return 0;}
};
