// RUN: foo-opt %s | foo-opt | FileCheck %s

module {
    // CHECK-LABEL: func @bar()
    func @bar() {
        // CHECK: %c1_i32 = constant 1 : i32
        %0 = constant 1 : i32
        // CHECK: %c0_i32 = constant 0 : i32
        %1 = constant 0 : i32
        // CHECK: %0 = or %c1_i32, %c0_i32 : i32
        %res =  or %0, %1 : i32
        // CHECK: return
        return
    }
}
