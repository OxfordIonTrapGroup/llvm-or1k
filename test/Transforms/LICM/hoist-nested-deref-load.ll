; RUN: opt -S -licm < %s | FileCheck %s

target datalayout = "E-m:e-p:32:32-i8:8:8-i16:16:16-i64:32:32-f64:32:32-v64:32:32-v128:32:32-a0:0:32-n32"

%a = type { %b* }
%b = type { i32 }

; This test represents the following function:
; class B:
;   __immutable_fields__ = {"x"}
;   def __init__(self):
;     self.x = 10
; class A:
;   __immutable_fields__ = {"f"}
;   def __init__(self):
;     self.f = A()
;   def foo(self):
;     for _ in range(10):
;       consume(self.f.x)
; in a memory-safe language where every pointer that is ever computed
; is fully dereferenceable.
;
; We want to check that loads of immutable  fields are hoisted out of
; loops even when nested; if they were marked just !dereferenceable instead
; of !unconditionally_dereferenceable, LICM would strip that metadata, as it
; cannot prove that the loaded pointer is still dereferenceable in the loop
; preheader.

; CHECK-LABEL: @test
; CHECK: entry:
; CHECK: %val.f = load %b*, %b** %ptr.f, !unconditionally_dereferenceable !0
; CHECK: %val.x = load i32, i32* %ptr.x
; CHECK: for.head:

define void @test(%a* dereferenceable(4) %arg) {
entry:
  br label %for.head

for.head:
  %IND = phi i32 [ 0, %entry ], [ %IND.new, %for.body ]
  %CMP = icmp slt i32 %IND, 10
  br i1 %CMP, label %for.body, label %exit

for.body:
  %ptr.f = getelementptr inbounds %a, %a* %arg, i32 0, i32 0
  %val.f = load %b*, %b** %ptr.f, !invariant.load !0, !unconditionally_dereferenceable !1
  %ptr.x = getelementptr inbounds %b, %b* %val.f, i32 0, i32 0
  %val.x = load i32, i32* %ptr.x, !invariant.load !0
  call void @consume(i32 %val.x)
  %IND.new = add i32 %IND, 1
  br label %for.head

exit:
  ret void
}

declare void @consume(i32)

!0 = !{}
!1 = !{ i64 4 }
