/*
 GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project submission
 FILE: circularbuffer Program (Circular Buffer Library)
 Implements the circular buffer program that runs in the client side.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
/*Structure Definition*/
typedef struct {
    int size;   //size of the buffer
    int start; // Start of the buffer can also be called as "head"
    int end;   // End of the buffer can also be called as "tail"
    data_packet *packets[64];// Circular buffer holds 64 -packets
} CircularBuffer;

/*Method for  Buffer Initialization.*/
void CircularBufferInitialization(CircularBuffer *cb){
    int i;
    cb->size = 64;
    cb->start = 0;
    cb->end = 0;
    for(i = 0 ; i < 64 ; i++){
        cb->packets[i]= (data_packet *)calloc(1, sizeof(data_packet));
        cb->packets[i]->sequence_number = -1 ;
    }
}

/*Method for freeing packet at "index" location.*/
void CircularBufferFree(CircularBuffer *cb, int index) {
    if(cb->packets[index] != NULL){
        cb->packets[index] = NULL;
    }
    else{
        return;
    }
}

/*Method for checking wether the circular buffer is full.*/
int CircularBufferFull(CircularBuffer *cb) {
    return (cb->end + 1) % cb->size == cb->start;
}

/*Method for checking wether the circular buffer is empty.*/
int CircularBufferEmpty(CircularBuffer *cb) {
    return cb->end == cb->start;
}


/*Method for writing the data to the cyclic buffer.*/
void CircularBufferWrite(CircularBuffer *cb, data_packet *packet) {
    if(cb->packets[(cb->end%cb->size)] == NULL){
        cb->packets[(cb->end%cb->size)] = (data_packet *)calloc(1, sizeof(data_packet));
    }
    
    memcpy(cb->packets[(cb->end%cb->size)]->payload,packet->payload,packet->size);
    cb->packets[(cb->end%cb->size)]->sequence_number = packet->sequence_number;
    cb->packets[(cb->end%cb->size)]->FYN = packet->FYN;
    cb->packets[(cb->end%cb->size)]->checksum = packet->checksum;
    cb->packets[(cb->end%cb->size)]->size = packet->size;
    
    cb->end = ((cb->end) + 1)%cb->size; //% (cb->size - 1)
    printf("CIRCULAR BUFFER  is writing and the state is [Start: %d, End: %d] \t |\n", cb->start, cb->end);
}

/*Fetching the packet at the index location*/
data_packet * CircularBufferFetch(CircularBuffer *cb, int index) {
    
    if(CircularBufferEmpty(cb)) return;
    //If there is no packet at the location
    if(cb->packets[index %(cb->size)] == NULL) {
        printf("Warning from Circular buffer: No Packets at the location : %d \t|\n",index);
        return;
    }
    printf("Fetching the packet from Circular Buffer with index: %d \n ",index %(cb->size));
    return cb->packets[(index %(cb->size))];
}

/*On removing a packet from the circular buffer this method will adjust the start of the cicular buffer*/
void CircularBufferAdjustStart(CircularBuffer *cb){
    int old=cb->start;
    /*if(old!=cb->size - 1){
     while(cb->packets[cb->start] == NULL){
     int old=cb->start;
     cb->start = (cb->start+1) % (cb->size);
     int new=cb->start;
     printf("Start is adjusted from %d to %d",old,new);
     }
     }else{*/
    if(cb->packets[cb->start] == NULL){
        int old=cb->start;
        cb->start = (cb->start+1) % (cb->size);
        int new=cb->start;
        //printf("Start is adjusted from %d to %d",old,new);
    }
}

/*Method for deleting the element from the circular buffer*/
void CircularBufferDelete(CircularBuffer *cb, int index){
    int loc;
    if(CircularBufferEmpty(cb))
    return;
    loc = (index) % (cb->size);
    //printf("INDEX is %d and LOC is %d \n", index, loc);
    if(cb->packets[loc] != NULL){
        //printf("State Before REMOVE: [Start: %d, End: %d]\n", cb->start, cb->end);
        CircularBufferFree(cb, loc);
        CircularBufferAdjustStart(cb);
        printf("CIRCULAR BUFFER removed the packet and the state is [Start: %d, End: %d]\t|\n", cb->start, cb->end);
        fflush(stdout);
    }
    else{
        return;
    }
}
