//= OR1KInstPrinter.h - Convert OR1K MCInst to asm syntax ---------*- C++ -*--//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints a OR1K MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#ifndef OR1KINSTPRINTER_H
#define OR1KINSTPRINTER_H

#include "llvm/MC/MCInstPrinter.h"

namespace llvm {
class OR1KInstPrinter : public MCInstPrinter {
public:
  OR1KInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                  const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI) {}

  void printInst(const MCInst *MI, raw_ostream &O, StringRef Annot,
                 const MCSubtargetInfo &STI) override;
  void printOperand(const MCInst *MI, unsigned OpNo,
                    raw_ostream &O, const char *Modifier = 0);
  void printMemOperand(const MCInst *MI, int OpNo,raw_ostream &O,
                       const char *Modifier = 0);
  void printS16ImmOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O);
  void printRegName(raw_ostream &OS, unsigned RegNo) const override;

  // Autogenerated by tblgen.
  void printInstruction(const MCInst *MI, raw_ostream &O);
  static const char *getRegisterName(unsigned RegNo);
};
}

#endif
