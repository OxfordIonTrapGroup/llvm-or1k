//===- OR1KDisassembler.cpp - Disassembler for OR1K -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the OR1K Disassembler.
//
//===----------------------------------------------------------------------===//

#include "OR1K.h"
#include "OR1KDisassembler.h"
#include "OR1KSubtarget.h"
#include "OR1KRegisterInfo.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/Support/MemoryObject.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace llvm {
  extern Target TheOR1KTarget;
}

static MCDisassembler *createOR1KDisassembler(const Target &T,
                                              const MCSubtargetInfo &STI) {
  return new OR1KDisassembler(STI, T.createMCRegInfo(""));
}

extern "C" void LLVMInitializeOR1KDisassembler() {
  // Register the disassembler
  TargetRegistry::RegisterMCDisassembler(TheOR1KTarget,
                                         createOR1KDisassembler);
}

// Forward declare because the autogenerated code will reference this.
// Definition is further down.
DecodeStatus DecodeGPRRegisterClass(MCInst &Inst,
                                                 unsigned RegNo,
                                                 uint64_t Address,
                                                 const void *Decoder);

static DecodeStatus DecodeMemoryValue(MCInst &Inst,
                                       unsigned Insn,
                                       uint64_t Address,
                                       const void *Decoder);

#include "OR1KGenDisassemblerTables.inc"

static DecodeStatus readInstruction32(const MemoryObject &region,
                                      uint64_t address,
                                      uint64_t &size,
                                      uint32_t &insn) {
  uint8_t Bytes[4];

  // We want to read exactly 4 bytes of data.
  if (region.readBytes(address, 4, (uint8_t*)Bytes) == -1) {
    size = 0;
    return MCDisassembler::Fail;
  }

  // Encoded as big-endian 32-bit word in the stream.
  insn = (Bytes[0] << 24) |
         (Bytes[1] << 16) |
         (Bytes[2] <<  8) |
         (Bytes[3] <<  0);

  return MCDisassembler::Success;
}

DecodeStatus
OR1KDisassembler::getInstruction(MCInst &instr,
                                 uint64_t &Size,
                                 const MemoryObject &Region,
                                 uint64_t Address,
                                 raw_ostream &vStream,
                                 raw_ostream &cStream) const {
  uint32_t Insn;

  DecodeStatus Result = readInstruction32(Region, Address, Size,
                                          Insn);

  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Call auto-generated decoder function
  Result = decodeInstruction(DecoderTableOR1K32, instr, Insn, Address,
                             this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  return MCDisassembler::Fail;
}

DecodeStatus DecodeGPRRegisterClass(MCInst &Inst,
                                    unsigned RegNo,
                                    uint64_t Address,
                                    const void *Decoder) {

  if (RegNo > 31)
    return MCDisassembler::Fail;

  // The internal representation of the registers counts r0: 2, r1: 3, etc.
  // FIXME: Use a more stable method of identifying registers
  Inst.addOperand(MCOperand::CreateReg(RegNo+2));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeMemoryValue(MCInst &Inst,
                                       unsigned Insn,
                                       uint64_t Address,
                                       const void *Decoder) {
  unsigned Register = (Insn >> 16) & 0x1f;
  Inst.addOperand(MCOperand::CreateReg(Register+2));
  unsigned Offset = (Insn & 0xffff);
  Inst.addOperand(MCOperand::CreateImm(SignExtend32<16>(Offset)));
  return MCDisassembler::Success;
}
