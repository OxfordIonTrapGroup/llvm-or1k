//===-- OR1KInstrInfo.cpp - OR1K Instruction Information --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the OR1K implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "OR1KInstrInfo.h"
#include "OR1K.h"
#include "OR1KMachineFunctionInfo.h"
#include "OR1KSubtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"

#define GET_INSTRINFO_CTOR
#include "OR1KGenInstrInfo.inc"

using namespace llvm;

OR1KInstrInfo::OR1KInstrInfo()
  : OR1KGenInstrInfo(OR1K::ADJCALLSTACKDOWN, OR1K::ADJCALLSTACKUP),
    RI(*this) {
}

#if 0

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned OR1KInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
                                             int &FrameIndex) const {
  if (MI->getOpcode() == OR1K::LWZ) {
    if (MI->getOperand(1).isFI() && MI->getOperand(2).isImm() &&
        MI->getOperand(2).getImm() == 0) {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned OR1KInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                            int &FrameIndex) const {
  if (MI->getOpcode() == OR1K::SW) {
    if (MI->getOperand(0).isFI() && MI->getOperand(1).isImm() &&
        MI->getOperand(1).getImm() == 0) {
      FrameIndex = MI->getOperand(0).getIndex();
      return MI->getOperand(2).getReg();
    }
  }
  return 0;
}

unsigned
OR1KInstrInfo::InsertBranch(MachineBasicBlock &MBB,MachineBasicBlock *TBB,
                             MachineBasicBlock *FBB,
                             const SmallVectorImpl<MachineOperand> &Cond,
                             DebugLoc DL)const{
  // Can only insert uncond branches so far.
  assert(Cond.empty() && !FBB && TBB && "Can only handle uncond branches!");
  BuildMI(&MBB, DL, get(SP::BA)).addMBB(TBB);
  return 1;
}
#endif

void OR1KInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I, DebugLoc DL,
                                unsigned DestReg, unsigned SrcReg,
                                bool KillSrc) const {
  if (OR1K::GPRRegClass.contains(DestReg, SrcReg))
    BuildMI(MBB, I, DL, get(OR1K::ORI), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc))
      .addImm(0);
  else
    llvm_unreachable("Impossible reg-to-reg copy");
}

void OR1KInstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC,
                    const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == OR1K::GPRRegisterClass)
    BuildMI(MBB, I, DL, get(OR1K::SW)).addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FI).addImm(0);
  else
    llvm_unreachable("Can't store this register to stack slot");
}

void OR1KInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC,
                     const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == OR1K::GPRRegisterClass)
    BuildMI(MBB, I, DL, get(OR1K::LWZ), DestReg).addFrameIndex(FI).addImm(0);
  else
    llvm_unreachable("Can't load this register from stack slot");
}

#if 0
unsigned OR1KInstrInfo::getGlobalBaseReg(MachineFunction *MF) const
{
  OR1KMachineFunctionInfo *OR1KFI = MF->getInfo<OR1KMachineFunctionInfo>();
  unsigned GlobalBaseReg = OR1KFI->getGlobalBaseReg();
  if (GlobalBaseReg != 0)
    return GlobalBaseReg;

  // Insert the set of GlobalBaseReg into the first MBB of the function
  MachineBasicBlock &FirstMBB = MF->front();
  MachineBasicBlock::iterator MBBI = FirstMBB.begin();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();

  GlobalBaseReg = RegInfo.createVirtualRegister(&OR1K::GPRRegClass);


  DebugLoc dl;

  BuildMI(FirstMBB, MBBI, dl, get(SP::GETPCX), GlobalBaseReg);
  OR1KFI->setGlobalBaseReg(GlobalBaseReg);
  return GlobalBaseReg;
}
#endif
