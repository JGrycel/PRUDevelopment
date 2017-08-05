// Simple PRU source for generating a host interrupt
// 10 times a second. 100 iterations through the outer loop,
// with each inner loop counting to 1,000,000 instructions

.origin 0
.entrypoint START

#define ITERATIONS      10         // Number of outer loop iterations
#define INS_PER_IT      1000000    // Number of inner loop instructions

#define HOST_INTERRUPT  (1 << 5)   // Interrupt-signaliing it in R31
#define PRU_EVENT_0     0x00000003 // Bits to signify interrupt 0
#define PRU_EVENT_1     0x00000004 // Bits to signify interrupt 0

// REGISTER USAGE:
// r0: maximum outer iterations
// r1: current outer iteration count
// r2: maximum inner instructions
// r3: current inner instruction count

START:
    MOV r0, ITERATIONS      // Load maximum outer loop iterations into r0
    MOV r1, 0x00000000      // Zero-out current outer loop iteration count
    MOV r2, INS_PER_IT      // Load maximum inner loop instrutions into r2
    MOV r3, 0x00000000      // Zero-out current inner loop instruction count

INNER_LOOP:
    ADD r3, r3, 0x00000002   // Increase instruction count
    QBLT INNER_LOOP, r2, r3  // Loop again if under the maximum instructions

OUTER_LOOP:
    MOV r3, 0x00000000           // Zero-out instruction count
    ADD r1, r1, 0x00000001       // Increment iteration count
    MOV r31.b0, HOST_INTERRUPT | PRU_EVENT_1 // Interrupt host
    QBLT INNER_LOOP, r0, r1      // Loop again if under the maximum iterations

END:
    MOV r31.b0, HOST_INTERRUPT | PRU_EVENT_0 // Interrupt host with end of program
    HALT            // Stop PRU execution
