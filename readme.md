# Emulator in C

## Introduction

It is an emulator that supports 7 instructions.

The emulator tries to imitate the working of an OS by switching between user-mode and kernel-mode to perform different
operations.

Loading programs, getting input and sending output are done in kernel mode while operations that access the internal
memory contents and registers only are done in user mode.

## Components

The OS utilizes some virtual components to emulate the behavior of underlying hardware.

It consists of the following components:

1. Data Register (R): Used to hold the data temporarily. It is of 4 bytes in size, same as a memory word.


2. Instruction Register (IR): Holds the currently executing instruction that was fetched from the memory before
   execution of the instruction. It is also 4 bytes in size because instructions are 4 bytes too.


3. Instruction Counter (IC): It is a counter that points to the address in the memory of the next instruction to be
   executed. It is incremented after the instruction from the memory is copied to IR. It can hold values from 00 to 99
   to be able to point the 100 memory addresses in the memory. It can also be changed by branch instruction to point to
   a non-consecutive address.


4. Toggle Register (T): It is the only condition register available in this basic hardware structure. The status of the
   toggle register is checked by the branch instruction as a condition before branching. The branching is done only if
   the toggle register holds a 1 value.


5. 100 4 Byte word Memory (MEM): The basic hardware structure also consists of a 100 Word Memory , where each word is 4
   bytes in size. This is a generic memory used to store both program and data.


6. Card Reader (Input Device): Represented by input.txt file. It is an input interface that allows the operating system
   to read programs and data from a card. A card is an Input file that consists of a block of memory. Data are read from
   the card as blocks. ie 10 words or 40 characters at a time and stored in memory, occupying whole memory block
   starting from a memory address such as 00, 10,20,30,..., 90. and occupying whole ten words after it.


7. Line Printer (Output Device): Represented by output.txt file. It is an output interface that allows the operating
   system to output the generated data to another card (output file). As in the case of input file, data are output in
   blocks too.

## Instruction Set

There are a total of 7 instructions. They are:

1. Load Register (LR): Loads the data register with the content in the memory pointed by the address in the address
   field (3rd and 4th byte) of the instruction.


2. Store Register (SR): Stores the content of the data register in the memory pointed by the address in the address
   field of the instruction.


3. Compare Register (CR): Compares the content of the data register with the content in the memory pointed by the
   address in the address field of the instruction. If the contents are equal then the toggle register is set to 1 else
   the toggle register is set to 0.


4. Branch if Toggle (BT): Checks the toggle register and if it is 1 then changes the program counter to the address
   specified in the address field of the instruction.


5. Get Data (GD): Instructs the machine to read a block (10 words) of data from the card reader (input file) and put it
   in the address in the memory pointed by the 3rd byte of the instruction.


6. Print Data (PD): Instructs the machine to print a block of memory pointed by the 3rd byte of the instruction by
   sending it to the line printer (output file).


7. Halt (H): Tells the machine that the current program has finished executing, after which it tries to load another
   program from the card reader (input file) if there are any left in the card, else the machine shuts down.

## Program Format

The format of a program of this basic operating system is:

    $AMJ
    [multiple Lines of program]
    ... ... ...
    $DTA
    [multiple lines of data]
    ... ... ...
    $END

$AMJ, $DTA and $END are called control cards. $AMJ signals the start of a new program until $DTA, $DTA means the start
of data block until $END. $END card is used to specify the end of the current program

Another $AMJ card follows after the $END card if there are more programs, else no other card is supplied which specifies
the end of all programs.

## Example Program

An example program would be:

    $AMJ
    GD40PD40GD50PD50GD60PD60GD70LR70SR60LR71
    SR61PD40PD50PD60GD80LR85SR86PD81GD90PD90
    H
    $DTA  
    MAY OUR FRIENDSHIP! TURN
    INTO
    SILVER
    GOLD
    THEN WE WILL SELL IT 50
    OK
    $END

The above generates the following output:

    MAY OUR FRIENDSHIP! TURN           	 
    INTO                               	 
    SILVER                             	 
    MAY OUR FRIENDSHIP! TURN           	 
    INTO                               	 
    GOLD                               	 
    THEN WE WILL SELL IT 50  50        	 
    OK

The system loads 4 bytes of program at a time i.e. GD40, PD40 and so on.

As we can see, the above code simply reads the data and outputs it but repeating the lines

MAY OUR FRIENDSHIP! TURN, INTO and 50.

## Implementation and Execution

* The program execution begins with a Termination Service Interrupt that tells the operating system that the previous
  program has finished executing so the next program needs to be loaded.

* The OS then switches to the kernel mode to respond to the interrupt and executes the termination routine.

* The termination routine sends two new lines to the line printer and then executes the load routine.

* The load routine reads the next card, in this case the first card from the card reader.

* The first card must be the $AMJ card to specify the beginning of a new program.

* After reading the $AMJ card the operating system starts reading following cards until $DTA card.

* Each read card is copied to a block of memory starting from 00.

* On receiving the $DTA card the load routine finishes. Thus exiting Kernel Mode and switching to user mode.

* Then the control shifts to the execution routine.

* The execution routine starts executing the program loaded in the memory by the load routine.

* Firstly, the Instruction counter is set to 0.

* Then the instruction in the memory pointed by the instruction counter is loaded to the IR.

* And the IC is incremented by 1.

* Then the instruction in the IR is decoded and executed.

* The execution continues until the execution of the HALT(H) instruction. After which the termination service interrupt
  is raised again and the cycle continues until all programs have been executed.

## Service Interrupt Routine

**READ**

Read a card from the card reader and copy it to the memory block in the address pointed by the address field of the
instruction.

**WRITE**

Write a block of memory pointed by the address field of the instruction to a card by sending it to the line printer.

**TERMINATE**

Send two new lines to the line printer. Then read next program from the Card Reader i.e. Read from $AMJ card to the $DTA
card or terminate if no more input cards remain.
