#include <xinu.h>

syscall	i8255x_exec_cu(struct ether *ethptr, uint32 address){
	
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address);
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, I8255X_COMMAND_CU_START);

	return i8255x_cmd_accept(ethptr);
}

syscall	i8255x_exec_ru(struct ether *ethptr, uint32 address){
	
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address);
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, I8255X_COMMAND_RU_START);

	return i8255x_cmd_accept(ethptr);
}
syscall	i8255x_resume_ru(struct ether *ethptr){
	
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, I8255X_COMMAND_RU_RESUME);

	return i8255x_cmd_accept(ethptr);
}

byte i8255x_get_cu_status(struct ether *ethptr){
	byte sbyte; //status byte
	sbyte = inb(ethptr->iobase + I8255X_STATUS_LOW);
	return (sbyte & I8255X_STATUS_CU_MASK); 
}


byte i8255x_get_ru_status(struct ether *ethptr){
	byte sbyte; //status byte
	sbyte = inb(ethptr->iobase + I8255X_STATUS_LOW);
	return (sbyte & I8255X_STATUS_RU_MASK);
}



status i8255x_cmd_accept(
	struct ether *ethptr
	)
{
	int32 i;
	byte retval;
	
	for(i=0; i < I8255X_CMD_ACCEPT_RETRIES; i++){
		DELAY(10);	
		retval  = inb(ethptr->iobase + I8255X_SCB_COMMAND_LOW); // read the command word
		retval &= 0x00FF;
		
		if(retval == 0){
			break;
		}
	}

	if(i == I8255X_CMD_ACCEPT_RETRIES){
		//kprintf("i8255x_cmd_accept: retried %d times. FAILED!\n\r", i );
		return SYSERR;
	}
	
//	kprintf("i8255x_cmd_accept: retried %d times. SUCCEEDED!\n\r", i );
	return OK;
}


status i8255x_dump_stats(
	struct ether *ethptr
	)
{
	byte command;
	uint32 address;

	if((uint32)ethptr->stats == SYSERR){
		return SYSERR;
	}

	/*run the Dump Stats Counters command*/
	command = 0x05;
	command = command << I8255X_SCB_SHIFT_CUC;
	address  = 0; //This command dont cares what is in GENPTR
	/*Write the address to SCB General Register*/
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address);	
	/*Load CU-Base opcode  0101 (23:20) */
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, command);
	
	if(OK != i8255x_cmd_accept(ethptr)){
		//kprintf("dump stats command Failed\n\r");
		return SYSERR;
	}
	
	kprintf("Stats dumped\n\r");
	return OK;
}


status i8255x_dump_stats_reset(
	struct ether *ethptr
	)
{
	byte command;
	uint32 address;

	if((uint32)ethptr->stats == SYSERR){
		return SYSERR;
	}

	/*run the Dump Stats Counters command*/
	command = 0x07;
	command = command << I8255X_SCB_SHIFT_CUC;
	address  = 0; //This command dont cares what is in GENPTR
	/*Write the address to SCB General Register*/
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address);	
	/*Load CU-Base opcode  0111 (23:20) */
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, command);
	
	if(OK != i8255x_cmd_accept(ethptr)){
		//kprintf("dump and reset stats command Failed\n\r");
		return SYSERR;
	}
	
	kprintf("Stats dumped and reset\n\r");
	return OK;
}

uint32 i8255x_stats(struct ether *ethptr, uint32 index, bool8 force){
	uint32 *counters;
	
	if(index > I82559_DUMP_STATS_SIZE / 4){
		return SYSERR;
	}
	
	if(force){
		if(OK != i8255x_dump_stats(ethptr)){
			return SYSERR;
		}
	}
	counters = ethptr->stats;
	return counters[index];
}

status i8255x_print_stats(struct ether *ethptr, bool8 force){

	char names[21][30] = {
			"Tx Good frames",
			"Tx Max Col Er",
			"Tx Late Col Er",
			"Tx Underrun Er",
			"Tx Lost Carrier Sense",
			"Tx deferred",
			"Tx Single Collision",
			"Tx Multiple Collision",
			"Tx Total Collision",
			"Rx Good Frames",
			"Rx CRC Errors",
			"Rx Allignment Errors",
			"Rx Resource Errors",
			"Rx Overrun Errors",
			"Rx Collision Detect Errors",
			"Rx Short Frame Errors",
			"FC Tx Pause",
			"FC Rx Pause",
			"FC Rx Unsupported",
			"Tx/Rx TCO frames",
			"STATUS"
	};
	int32 i;

	if(force){
		if(OK != i8255x_dump_stats(ethptr)){
			return SYSERR;
		}
	}
	
	kprintf("--------------------STATS---------------------\n\r");
	kprintf("%30s - # \n\r","Counter Names");
	kprintf("--------------------STATS---------------------\n\r");
	for(i=0; i < I82559_DUMP_STATS_SIZE/4; i++){
		kprintf("%30s - %d \n\r", names[i],  (int32)i8255x_stats(ethptr, i, FALSE));
	}
	kprintf("--------------------STATS---------------------\n\r");
        
        return OK;

}

uint32 i8255x_rx_count(struct ether *ethptr, bool8 force){
	return i8255x_stats(ethptr, 9, force);
}

uint32 i8255x_tx_count(struct ether *ethptr, bool8 force){
	return i8255x_stats(ethptr, 0, force);
}
