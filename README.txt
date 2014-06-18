Group No: 8 Automated Pesticide Dispenser
----------------------------------------------

Team
------
Akanksha Patidar       123050094 
Dixita Bothra          123059005 
Swadesh Jain           123050085

Hardware Requirements
------------------------
• Sharp IR Sensors
• Servo Motors
• Fire-bird
• Zig-bee Module
• Auxiliary Power Supply
• Sifter and Container with knob open/close mechanism using servo motor
• Lifter arm servo motor attachment

Software Requirements
------------------------
• AVR Studio
• WinAVR
• X-CTU

Instruction
--------------

STEP:1 Entire code is present in Team8/pesticide.c, first open this file with AVR Studio IDE. "lcd.c" should also be present in same directory. Now, set project configurations according to frequency of bot. After this, build(compile) the project.

STEP:2 Now, pesticide.hex will be generated in the default folder.

STEP:3 Burn this pesticide.hex file using AVR Boot Loader on the BOT.

STEP:4 Restart the BOT.

STEP:5 Now, we will give command through common interface(i.e. Android Phone) according to the format specified below, that command will be received by BOT through ZigBee Module.

The format of common interface is:

BOT-Id$funCode$par1$par2$par3#

In case of our bot, the format for will be:
8$2$1#
Here, 	botId=8;
	funCode=2;	// Function for dispencing is called.
	par1=1;		// This parameter tells about trough number on which function for dispencing is called.
