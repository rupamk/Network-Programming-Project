# Network-Programming-Project


 In this Project we implemented and tested TCP-like reliable transportlayer protocol by using any inter-process communication mechanism such as UDP. Additionally, we also implemented test programs on top of this transport layer, to check the functionality of individual modules. Then the functionality of this transport layer protocol is contrasted against TCP, by designing simple file-transfer application. File transfers are monitored, tested using troll, a network emulator, which emulates real world network by using user adjustable parameters such as packet garbling, packet drop etc.


The folder contains following files

 circularBuffer.c
 crc.h
 crc.c
 ftps.c 
 ftpc.c
 Jacobson.c
 Makefile
 packet.h
 TCPD_M1.c
 TCPD_M2.c
 README.txt
 timerprocess.c
 troll
 NetworkProgrammingReport.pdf

______________________________________________________________________________________________________________

 circularBuffer.c

This file executes the necessary function for maintaining circular
buffer at client's TCP Demon (TCPD_M2). The file includes addition, removal and read functionalities from
circular buffer.
______________________________________________________________________________________________________________

 crc.h

This is a header file containg definitions for crc computation.
______________________________________________________________________________________________________________
 crc.c

This file computes checksum.

*** NOTE:  * Copyright (c) 2000 by Michael Barr.  This software is placed into the public domain and may be used for any purpose.  
    	   * However, this notice must not be changed or removed and no warranty is either expressed or implied by its publication 
	   * or distribution.
______________________________________________________________________________________________________________

 Jacobson.c

This file computes SRTT, RTTVAR and RTO required by 
TCPD_M2 for enabling reliable communication.
______________________________________________________________________________________________________________
 Makefile

This file is used for compiling all necessary c files. It also has functionality
to remove executables.

Execution Command:
make

For removing compiled files and unncessary clutter.

Execution Command:
make clean
______________________________________________________________________________________________________________
 packet.h

Header files for packets(both data and timer).
______________________________________________________________________________________________________________ 
ftps.c

This file contains server side application code. 

Execution Command:
ftps 8009
______________________________________________________________________________________________________________
 ftpc.c

This file contains client side application code. 

Execution Command:
ftpc 1.jpg
______________________________________________________________________________________________________________
 timerprocess.c

This file contains Delta_Timer code for executing timerprocess. It is implemented using doubly-linked-list (Template).
 It communicates with TCPD_M2(on client's side) to get and transmit notifications regarding new timer and timer expiry.

Execution Command:
tp
______________________________________________________________________________________________________________
 TCPD_M1.c

This file creates deamon process(in server side) to accept packets from troll. 

Execution Command:
TCPD_M1
______________________________________________________________________________________________________________
 TCPD_M2.c

This file creates deamon process(in client's side) to deliver packets to troll. 

Execution Command:
TCPD_M2
______________________________________________________________________________________________________________

11) troll
This helps to generate real life network simulations with drop, garble, duplication and delays. It works
with seven command line arguments viz. 1) Packet Drop Percentage (Specified with -x flag), 

- Packet Gabriel Percentage (Specified with -g flag), Packet Duplication Percentage ( Specified with -m ), Trace on (  Specified with -t ) 3) TCPD_M1 Host Name (Specified with -C flag), 

- TCPD_M2 Host Name (Specified with -S flag), 4) TCPD_M1 Listen PORT (Specified with -a flag),

- TCPD_M2_Send PORT (Specified with -b flag) and 6)TROLL Port 

Execution Command:
(For Communication Between TCPD_M2 to TCPD_M1) ***On beta.cse.ohio-state.edu
troll -x 10 -g 25 -m 25 -t -C gamma -S beta -a 8009 -b 4009 4680

(For Communication Between TCPD_M1 to TCPD_M2) ***On gamma.cse.ohio-state.edu
troll -x 0 -g 0 -se 1000 -C beta -S gamma -a 7009 -b 5009 6009

______________________________________________________________________________________________________________

STEPS TO EXECUTE :(Open a seperate terminal window for each of the following steps)

1. Execute troll on beta.cse.ohio-state.edu :

% troll -x 10 -g 25 -m 25 -t -C gamma -S beta -a 8009 -b 4009 4680

2. Execute troll on gamma.cse.ohio-state.edu:

% troll -x 0 -g 0 -se 1000 -C beta -S gamma -a 7009 -b 5009 6009

3. Execute timerprocess process on beta.cse.ohio-state.edu:

% tp

4. Execute TCPD_M2 on beta.cse.ohio-state.edu:

% TCPD_M2

5.  Execute TCPD_M1 on gamma.cse.ohio-state.edu:

% TCPD_M1

6. Execute ftps on gamma.cse.ohio-state.edu:

% ftps 8009

7. Execute ftpc on beta.cse.ohio-state.edu:

% ftpc <file-name>

______________________________________________________________________________________________________________









