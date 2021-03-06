# RUN: llvm-mc -arch=or1k -show-encoding %s | FileCheck %s

    lf.add.s r1, r2, r3
# CHECK: # encoding: [0xc8,0x22,0x18,0x00]

    lf.div.s r1, r2, r3
# CHECK: # encoding: [0xc8,0x22,0x18,0x03]

    lf.ftoi.s r1, r2
# CHECK: # encoding: [0xc8,0x22,0x00,0x05]

    lf.itof.s r1, r2
# CHECK: # encoding: [0xc8,0x22,0x00,0x04]

    lf.mul.s r1, r2, r3
# CHECK: # encoding: [0xc8,0x22,0x18,0x02]

    lf.rem.s r1, r2, r3
# CHECK: # encoding: [0xc8,0x22,0x18,0x06]

    lf.sub.s r1, r2, r3
# CHECK: # encoding: [0xc8,0x22,0x18,0x01]
