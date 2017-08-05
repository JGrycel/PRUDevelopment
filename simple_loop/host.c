/*
 * Simple PRU0 host program
 * Receives 10 interrupts, and then exits
 */

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define	LOOP_COUNT		10			   // Number of interrupts to wait for	 
#define TARGET_PRU 		0			   // Run program on PRU0
#define TARGET_PROGRAM "./pru_loop.p"  // Run loop PRU program

int configPRU(void)
{
	// Create reference to PRU initialization data
	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	
	// Initialize PRU
	if(prussdrv_init()){
		printf("Error initializing PRU\n");
		return 1;
	}
	printf("PRU memory initialized\n");
	
	// Open interrupts 0 and 1
	int result = prussdrv_open(PRU_EVTOUT_0);
	result |= prussdrv_open(PRU_EVTOUT_1);
	if(result){
		printf("Error opening PRU interrupts\n");
		return 1;
	}
	printf("PRU event interrupts 0 and 1 opened\n");
	
	
	// Map PRU interrupt controller
	if(prussdrv_pruintc_init(&pruss_intc_initdata)){
		printf("Error mapping interrupt controller\n");
		return 1;
	}
	printf("PRU interrupt controller mapped\n");

	return 0;
}

int closePRU()
{
	// Disable PRU0
	if(prussdrv_pru_disable(TARGET_PRU)){
		printf("Error disabling PRU0\n");
		return 1;
	}
	printf("Disabled PRU0\n");

	// Close PRU memory maps
	if(prussdrv_exit()){
		printf("Error closing PRU memory maps\n");
		return 1;
	}
	printf("Closed PRU memory maps\n");
	
	return 0;
}

int main(int argc, char *argv[])
{
	// Configure PRU subsystem for operation
	printf("Configuring PRU subsystem for operation...\n");
	if(configPRU()){
		printf("Exiting application due to errors...\n");
		return 1;
	}

	// Load loop-interrupt application into PRU0
	printf("Starting PRU program...\n\n");
	if(prussdrv_exec_program(TARGET_PRU, TARGET_PROGRAM)){
		printf("Error loading program into PRU0\n");
		printf("Exiting application due to errors...\n");
		return 1;
	}
	
	// Wait for all PRU interrupts to be received
	int i;
	for(i = 0; i < LOOP_COUNT; i++){
		// Wait for interrupt
		prussdrv_pru_wait_event(PRU_EVTOUT_1);
		// Clear interrupt
		prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU0_ARM_INTERRUPT);
		printf("Received PRU interrupt\n");
	}
	printf("\nPRU program successfully run\n\n");

	// Disable PRU0 and close mapped memory
	if(closePRU()){
		printf("Exiting application due to errors...\n\n");
		return 1;
	}
	printf("Disabled PRU and closed memory maps\n");
	
	return 0;
}
