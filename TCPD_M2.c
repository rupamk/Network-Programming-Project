/*
 GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project submission
 FILE: TCPD_M2 Program (TCP Domain Application on Client Side)
 */

#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "packet.h"
#include "Jacobson.c"
#include "circularBuffer.c"

/***************************Communication Ports*************************************/

/*.................Communication with TROLL on Client side .................*/
#define TROLLC_TCDPM2_SEND_PORT 4009                       
// TROLL Address
#define TROLLC_ADDRESS  "164.107.113.18"
#define TROLLC_PORT_BUF 4680   

/*.................Communication Ports: ftpc TO M2.................*/
#define FTPC_TCDPM2_PORT "2468"				
#define TCDPM2_FTPC_PORT "8642"				

/*.................Communication Ports for Timers.................*/
#define TCPDM2_TIMER_PORT 4689                            
#define TIMER_TCPDM2_PORT 9864  

/*........ M2 Listening from TROLLS on LISTEN_PORT .................*/
#define TROLLS_M2_LISTEN_PORT 7009 

                              

                          

/***************************************************** Main Program *******************************/

main()
{
    /******* Initialization**********/
    char buf[1024];
    int ftpc_TCPDM2_sock, TCPDM2_ftpc_sock, TCPDM2_trollc_sock, trolls_TCPDM2_sock, TCPDM2_timer_sock, timer_TCPDM2_sock;
    struct sockaddr_in ftpc_M2_name;		 
    struct sockaddr_in M2_ftpc_name; 		 
    struct sockaddr_in M2_trollc_name;		 
    struct sockaddr_in M2_send_name;		
    struct sockaddr_in trolls_M2_name;      
    struct sockaddr_in M2_timer_name;       
    struct sockaddr_in timer_M2_name;       
    struct hostent *hp, *gethostbyname();
    char FileName[20];
    int dontcheckNum=-1;
    
    /********Packet Declarations******/
    struct data_packet *packet;		
    struct data_packet *ack_packet; 
    struct data_packet *re_packet;  
    
    packet = calloc(1,sizeof(struct data_packet));
    ack_packet = calloc(1,sizeof(struct data_packet));
    
    struct timer_packet *new_timer;
    struct timer_packet *cancel_timer;
    
    new_timer = calloc(1, sizeof(struct timer_packet));
    cancel_timer = calloc(1, sizeof(struct timer_packet));
    
    /********Variable Declarations******/
    CircularBuffer cB;										
    CircularBufferInitialization(&cB);						
    
    int namelen;
    namelen = sizeof(struct sockaddr_in);
    
    fd_set readset;
    struct timeval now ;
    
    /**** SRTT & RTTVAR Initialization****/
    initialize(25);
    
    
     /*................. ftpc to TCPD_M2 Communication .................*/
    ftpc_TCPDM2_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(ftpc_TCPDM2_sock < 0) {
        perror("Error in opening datagram socket ");
        exit(2);
    }else
    {
        printf("opened ftpc_TCPDM2_sock successfully \n");
    }
    ftpc_M2_name.sin_family = AF_INET;
    ftpc_M2_name.sin_port = htons(atoi(FTPC_TCDPM2_PORT));
    ftpc_M2_name.sin_addr.s_addr = INADDR_ANY;
    if(bind(ftpc_TCPDM2_sock, (struct sockaddr *)&ftpc_M2_name, sizeof(ftpc_M2_name)) < 0) {
        perror("ERROR IN BINDING ftpc_TCPDM2_sock to ftpc_M2_name");
        exit(2);
    }else
    {
        printf("BINDING ftpc_TCPDM2_sock to ftpc_M2_name successfully \n");
    }
    
    
    /*.................* TCPD_M2 to ftpc Communication .................*/
    TCPDM2_ftpc_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(TCPDM2_ftpc_sock < 0) {
        perror("opening datagram socket");
        exit(2);
    }else
    {
        printf("opened TCPDM2_ftpc_sock successfully \n");
    }
    M2_ftpc_name.sin_family = AF_INET;
    M2_ftpc_name.sin_port = htons(atoi(TCDPM2_FTPC_PORT));
    hp = gethostbyname("localhost");
    if(hp == 0) {
        fprintf(stderr, "%s:host no identified\n");
        exit(3);
    }
    bcopy((char *)hp->h_addr, (char *)&M2_ftpc_name.sin_addr, hp->h_length);
    
    
    /*................. TCPD_M2 to TROLLC Communication .................*/
    TCPDM2_trollc_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(TCPDM2_trollc_sock < 0) {
        perror("opening datagram socket TCPDM2_trollc_sock");
        exit(2);
    }else
    {
        printf("opened TCPDM2_trollc_sock successfully \n");
    }
    M2_send_name.sin_family = AF_INET;
    M2_send_name.sin_port = htons(TROLLC_TCDPM2_SEND_PORT);
    M2_send_name.sin_addr.s_addr = INADDR_ANY;
    if(bind(TCPDM2_trollc_sock, (struct sockaddr *)&M2_send_name, sizeof(M2_send_name)) < 0) {
        perror("BINDING ERROR");
        exit(2);
    }else
    {
        printf("Binding TCPDM2_trollc_socket successfully \n");
    }
    
    
    /*................. TCPD_M2 to TROLLC Communication .................**/
    M2_trollc_name.sin_family = AF_INET;
    M2_trollc_name.sin_port = htons(TROLLC_PORT_BUF);
    M2_trollc_name.sin_addr.s_addr = inet_addr(TROLLC_ADDRESS);
    
    
    /*................. TROLLS to TCPD_M2 Communication .................*/
    trolls_TCPDM2_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(trolls_TCPDM2_sock < 0) {
        perror("opening datagram socket trolls_to_m2");
        exit(2);
    }else
    {
        printf("opened trolls_to_m2 successfully \n");
    }
    trolls_M2_name.sin_family = AF_INET;
    trolls_M2_name.sin_port = htons(TROLLS_M2_LISTEN_PORT);
    trolls_M2_name.sin_addr.s_addr = INADDR_ANY;
    if(bind(trolls_TCPDM2_sock, (struct sockaddr *)&trolls_M2_name, sizeof(trolls_M2_name)) < 0) {
        perror("Unable to bind TROLLS_to_M2 Sock to trolls_M2_name");
        exit(2);
    }else
    {
        printf("bind TROLLS_to_M2 Sock to trolls_M2_name successfully \n");
    }
    
    /*................. Timmer to M2 Communication .................*/
    timer_TCPDM2_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(timer_TCPDM2_sock < 0) {
        perror("opening datagram socket m2_to_timer");
        exit(2);
    }else
    {
        printf("opened datagram socket m2_to_timer successfully \n");
    }
    timer_M2_name.sin_family = AF_INET;
    timer_M2_name.sin_port = htons(TIMER_TCPDM2_PORT);
    timer_M2_name.sin_addr.s_addr = INADDR_ANY;
    if(bind(timer_TCPDM2_sock, (struct sockaddr *)&timer_M2_name, sizeof(timer_M2_name)) < 0) {
        perror("Unable to bind (TROLLS_to_M2 Sock)");
        exit(2);
    }else
    {
        printf("bind (TROLLS_to_M2 Sock) successfully \n");
    }
    
    /*................. M2 to TIMER Communication .................*/
    TCPDM2_timer_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(TCPDM2_timer_sock < 0) {
        perror("opening datagram socket");
        exit(2);
    }
    M2_timer_name.sin_family = AF_INET;
    M2_timer_name.sin_port = htons(TCPDM2_TIMER_PORT);
    hp = gethostbyname("localhost");
    if(hp == 0) {
        fprintf(stderr, "%s:unknown host\n");
        exit(3);
    }
    bcopy((char *)hp->h_addr, (char *)&M2_timer_name.sin_addr, hp->h_length);
    
    /*................. CRC Initialization  .................*/
    crcInit();
    
    while(1){
        float rto = 0;
        FD_ZERO(&readset);
        FD_SET(ftpc_TCPDM2_sock, &readset);
        FD_SET(trolls_TCPDM2_sock, &readset);
        FD_SET(timer_TCPDM2_sock, &readset);
        
        if(select(FD_SETSIZE, &readset, NULL, NULL, NULL) < 0 ) {
            perror("ERROR ON SELECT WAITING");
            exit (1);
        }
        
        /*................. Packet from ftpc tp TCPD_M2 .................*/
        if(FD_ISSET(ftpc_TCPDM2_sock, &readset)){
            bzero(packet,sizeof(packet));
            if(recvfrom(ftpc_TCPDM2_sock, packet, sizeof(struct data_packet), 0, (struct sockaddr *)&ftpc_M2_name, &namelen) < 0) {
                perror("Error in receiving message from FTPC");
                exit(4);
            }
            printf("\n\n");
            printf("|----------------------------------------------------\t \t \t| \n");
            printf("|[FTPC to TCDP-M2] received the pakcet with sequence_number :  %d \t |\n", packet->sequence_number);
            
            if(!CircularBufferFull(&cB)){
                CircularBufferWrite(&cB, packet);
                
                /*................. Computing RTO using JACOBSON Algorithm .................*/
                set_time(packet->sequence_number);
                if(packet->sequence_number == 1){
                    rto = 250;
                }
                else{
                    rto = CalibrateRTO();
                }
                
                /***** Creating and sending TIMER PACKET to TIMER PROCESS *****/
                
                new_timer->time = (rto > 1000 ? ((int)rto/1000)+3 : (int)3);
                new_timer->sequence_number = packet->sequence_number ;
                new_timer->type = 0 ;
                
                if (! new_timer) {
                    fprintf (stderr, "Calloc failed for creating new timer in TCPD_M2 \t |\n");
                    exit (EXIT_FAILURE);
                }
                if(sendto(TCPDM2_timer_sock, new_timer, sizeof(struct timer_packet), 0, (struct sockaddr *)&M2_timer_name, sizeof(M2_timer_name)) < 0) {
                    perror("Error while sending PACKET TO TIMER");
                    exit(4);
                }
                
                /***** Sending Packet to TROLLC *****/
                if(packet->FYN == '1' ){
                    if(cB.end - cB.start == 1){
                        if(sendto(TCPDM2_trollc_sock, packet, sizeof(struct data_packet), 0, (struct sockaddr *)&M2_trollc_name, sizeof(M2_trollc_name)) < 0) {
                            perror("Error while sending PACKET TO TROLLC");
                            exit(4);
                        }
                    }
                }
                else{
                    if(sendto(TCPDM2_trollc_sock, packet, sizeof(struct data_packet), 0, (struct sockaddr *)&M2_trollc_name, sizeof(M2_trollc_name)) < 0) {
                        perror("Error while sending PACKET TO TROLLC");
                        exit(4);
                    }
                }
            }
        }
        
        /********* ACK_PACKET from TROLLS to TCPD_M2 *********/
        if(FD_ISSET(trolls_TCPDM2_sock, &readset)) {
            if(recvfrom(trolls_TCPDM2_sock, ack_packet, sizeof(struct data_packet), 0, (struct sockaddr *)&trolls_M2_name, &namelen) < 0) {
                perror("Error  receiving ACK from TROLLS");
                exit(4);
            }
            printf("[TCPD-M1 to TCPD_M2]  Received the ACK with sequence_number : %d \t |\n",ack_packet->sequence_number);
            
            gettimeofday(&now,NULL);
            calc_rtt(now,timehash[ack_packet->sequence_number]);
            
            printf("UPDATE!!! the RTT on receiving the Ack packet with sequence_number : %d\t |\n",ack_packet->sequence_number);
            fflush(stdout);
            
            CircularBufferDelete(&cB, ack_packet->sequence_number);
            if(!CircularBufferFull(&cB)){
                char buff[20];
                bzero(buff, 20);
                memcpy(buff, "empty", 20);
                if(sendto(TCPDM2_ftpc_sock, buff, sizeof(buff), 0, (struct sockaddr *)&M2_ftpc_name, sizeof(M2_ftpc_name)) < 0) {
                    perror("Error while sending BUFFER FULL MSG TO FTPC");
                    exit(4);
                }
            }
            
            /***** TIMER CANCELATION REQUEST for ACK_PACKET from TCPD_M2 to TIMER ********/
            printf("[TCDP-M2 to TIMER] Sending Cancel time packet with sequence_number: %d\t |\n", ack_packet->sequence_number);
            cancel_timer->sequence_number = ack_packet->sequence_number;
            cancel_timer->type = 1;
            dontcheckNum=cancel_timer->sequence_number;
            if(sendto(TCPDM2_timer_sock, cancel_timer, sizeof(struct timer_packet), 0, (struct sockaddr *)&M2_timer_name, sizeof(M2_timer_name)) < 0) {
                perror("Error while sending PACKET TO TIMER");
                exit(4);
            }
            
            /*****Connection Shutdown****///&&CircularBufferEmpty(&cB)
            if((ack_packet->FYN == '1')){
                char buff[20];
                bzero(buff, 20);
                memcpy(buff, "acked", 20);
                
                printf("\nFile Transfer Completed and connection is about ot shutdown...\t|\n ");
                ack_packet->sequence_number = 1;
                bzero(ack_packet, 1000);
                ack_packet->checksum = crcFast(ack_packet->payload,1000);
                
                /****Sending ACKED MSG to ftpc*****/
                if(sendto(TCPDM2_ftpc_sock, buff, sizeof(buff), 0, (struct sockaddr *)&M2_ftpc_name, sizeof(M2_ftpc_name)) < 0) {
                    perror("Error while sending BUFFER FULL MSG TO FTPC");
                    exit(4);
                }
                
                /*****Sending FYN ACK to M1*****/
                if(sendto(TCPDM2_trollc_sock, ack_packet, sizeof(struct data_packet), 0, (struct sockaddr *)&M2_trollc_name, sizeof(M2_trollc_name)) < 0) {
                    perror("Error while sending PACKET TO TROLLC");
                    exit(4);
                }
                //sleep(1);
                break;
            }
        }
        
        /*................. TIMER packet (RTO EXPIRATIONS) from TIMER to TCPD_M2 .................*******/
        if(FD_ISSET(timer_TCPDM2_sock, &readset)) {
            int exp_seq_num ;
            if(recvfrom(timer_TCPDM2_sock, &exp_seq_num, sizeof(int), 0, (struct sockaddr *)&timer_M2_name, &namelen) < 0) {
                perror("ERROR receiving TIMER EXPIRE message from TIMERPROCESS");
                exit(4);
            }
            printf("\n| \n| [TIMER to TCDP-M2]  TIMER-EXPIRED!!! for packet with sequence_number: %d\t |\n|\n|", exp_seq_num);
            fflush(stdout);
            
            if(exp_seq_num!=dontcheckNum){
                /**** Retransmission of Packet *****/
                re_packet = CircularBufferFetch(&cB, exp_seq_num);
                //packet = CircularBufferFetch(&cB, exp_seq_num);
                printf("[TIMER to TCDP-M2] RETRANSMIT the packet with sequence_number: %d \t |\n",re_packet->sequence_number);
                //printf("[TIMER to TCDP-M2] RETRANSMIT the packet with sequence_number: %d \t |\n",packet->sequence_number);
                fflush(stdout);
                
                /***** CALCULATING RTO USING JACOBSON *****/
                set_time(re_packet->sequence_number);
                rto = CalibrateRTO();
                
                /***** CREATING AND SENDING TIMER PACKET TO TIMER PROCESS *****/
                new_timer->time = (rto > 1000 ? ((int)rto/1000)+3 : (int)3);
                new_timer->sequence_number = re_packet->sequence_number ;
                new_timer->type = 0 ;
                if (! new_timer) {
                    fprintf (stderr, "Calloc failed for creating new timer in TCPD_M2 \n");
                    exit (EXIT_FAILURE);
                }
                if(sendto(TCPDM2_timer_sock, new_timer, sizeof(struct timer_packet), 0, (struct sockaddr *)&M2_timer_name, sizeof(M2_timer_name)) < 0) {
                    perror("Error while sending PACKET TO TIMER");
                    exit(4);
                } /*.................***/
                //memcpy(buf,re_packet->payload,1000);
                //re_packet->checksum = crcFast(buf,1000);
                /*.................***/
                /*****Sending packet to TROLL*****/
                if(sendto(TCPDM2_trollc_sock, re_packet, sizeof(struct data_packet), 0, (struct sockaddr *)&M2_trollc_name, sizeof(M2_trollc_name)) < 0) {
                    perror("Error while sending PACKET TO TROLLC");
                    exit(4);
                }
            }
        }
        /*Sending buffer full message to FTPC*/
        if(CircularBufferFull(&cB)){
            char buff[20];
            bzero(buff, 20);
            memcpy(buff, "full", 20);
            if(sendto(TCPDM2_ftpc_sock, buff, sizeof(buff), 0, (struct sockaddr *)&M2_ftpc_name, sizeof(M2_ftpc_name)) < 0) {
                perror("Error while sending BUFFER FULL MSG TO FTPC");
                exit(4);
            }
        }
    }
    /*Printing the connection shutdown message t*/
    if(CircularBufferEmpty(&cB)){
        fprintf(stdout, "\n\n....<Connection Shutdown>....\n");
    }
    /*Closing the sockets*/
    close(ftpc_TCPDM2_sock);
    close(TCPDM2_ftpc_sock);
    close(TCPDM2_trollc_sock);
    close(trolls_TCPDM2_sock);
    close(TCPDM2_timer_sock);
    close(timer_TCPDM2_sock);
    exit(0);
}

