/* GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project Submission */
#include "crc.h"

/* Time packet definition */
typedef struct timer_packet {
    float time ;          //Associated Time
    int sequence_number ; //Sequence Number of Packet
    int type ;            // 0 - Insert; 1 - Delete
}timer_packet;

/* Data packet definition */
typedef struct data_packet {
    char payload[1000];     //Payload Carries the data
    int size;               //Size of the packet
    int sequence_number;    //Carries the sequence number of the packet
    char FYN;               // Carries the FYN status of the packet
    crc checksum;           // Carried the checksum of the packet
}data_packet;
