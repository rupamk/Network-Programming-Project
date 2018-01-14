/*
 GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project submission
   FILE: TCPD_M1 Program (Server Application)
*/

#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "packet.h"
#define MAXLEN_MSG 64000

/******* PORT: TCPD_M1 TO TROLL ********/
#define M1_TROLL_PORT 5009
#define M1_TROLL_PORT_BUF "6009"
#define TROLL_MACHINE "gamma"

/******** PORT : FTPS TO TCPD_M1******/
#define FTPS_M1_PORT 2009

/********** Function to initialize Receive Buffer *************/
init_recvbuff(data_packet buff_received[], int length){
    int i;
    for(i = 0; i < length; i++){
        buff_received[i].sequence_number = -3;
    }
}

/***** Function to check buffer is empty or not ****/
int IsEmpty_recvbuff(data_packet buff_received[] , int length){
    int i, index;
    for(i = 0 ; i < length; i++){
        if(buff_received[i].sequence_number != -3){
            index = 0;
            break;
        }
    }
    return index;
}

 /***************************************************** Main Program *******************************/
main()
{
    /******* Initialization**********/
    int ftps_M1_sock, troll_M1_sock, M1_troll_sock;
    int length_name, buffer_length;
    char TCPD_M1_buf[MAXLEN_MSG];
    struct sockaddr_in ftps_M1_name;
    struct sockaddr_in troll_M1_name;
    struct sockaddr_in M1_listen_name;
    struct sockaddr_in M1_comm_name;
    struct sockaddr_in M1_troll_name;
    struct hostent *hp, *gethostbyname();
    
    /********Packet Declarations******/
    data_packet *packet, *ack_packet, *FYN_packet;
    packet = calloc(1,sizeof(data_packet));
    ack_packet = calloc(1,sizeof(data_packet));
    FYN_packet = calloc(1,sizeof(data_packet));
    
    /*********Initialization of Receiving Buffer*********/
    data_packet buff_received[1000] ;
    int buff_received_head = 0;
    int counter=0;
  
    
    /********** Creating socket for connecting to FTPS ************/
    ftps_M1_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(ftps_M1_sock < 0) {
        perror("Datagram Socket opened");
        exit(2);
    }else{
	printf("Datagram socket opened successfully \n");
  }
    
    /********* Connecting to FTPS **********/
    ftps_M1_name.sin_family = AF_INET;
    ftps_M1_name.sin_port = htons(FTPS_M1_PORT);
    ftps_M1_name.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(ftps_M1_sock, (struct sockaddr *)&ftps_M1_name, sizeof(ftps_M1_name)) < 0) {
        perror("BINDING ERROR");
        exit(2);
    }else{
	printf("Binding Successfully done for Connecting TCPD_M1 to FTPS \n");
  }
    
   /**********Creating socket for connecting TCPD_M1 to Troll ******************/
    M1_troll_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(M1_troll_sock < 0) {
        perror("Datagram Socket opened: for reverse communication from M2 to Troll");
        exit(2);
    }else{
	printf("Datagram socket opened successfully \n");
  }
    
    /* Connecting TCPD_M1 to TROLL */
    bzero((char*)&M1_comm_name, sizeof M1_comm_name);
    M1_comm_name.sin_family = AF_INET;
    M1_comm_name.sin_port = htons(M1_TROLL_PORT);
    M1_comm_name.sin_addr.s_addr = INADDR_ANY;
    if(bind(M1_troll_sock, (struct sockaddr *)&M1_comm_name, sizeof(M1_comm_name)) < 0) {
         perror("BINDING ERROR");
    exit(2);
  }else{
	printf("Binding Successfully done for Connecting  TCPD_M1 to TROLL \n");
  }
    /* Connecting to TROLL */
    bzero ((char*)&M1_troll_name, sizeof M1_troll_name);
    M1_troll_name.sin_family = AF_INET;
    M1_troll_name.sin_port = htons(atoi(M1_TROLL_PORT_BUF));
    
    /* Convert hostname("gamma") to IP address */
    hp = gethostbyname(TROLL_MACHINE);
    if(hp == 0) {
        fprintf(stderr, "%s:unknown host\n",TROLL_MACHINE);
        exit(3);
    }
    bcopy((char *)hp->h_addr, (char *)&M1_troll_name.sin_addr, hp->h_length);
    
     /* Declarations */
    length_name=sizeof(struct sockaddr_in);
    buffer_length = MAXLEN_MSG;
    crcInit(); //checksum initialization
    
    /* Establishing connection: From client on the bounded address and output what it receives from client */
    
    if(recvfrom(ftps_M1_sock, packet, sizeof(data_packet), 0, (struct sockaddr *)&ftps_M1_name, &length_name) < 0) {
        perror("error receiving"); 
    exit(4);
  }else{
	printf("Recevied Successful \n");
  }
    printf("TCPD_M1 receiving from FTPS (ListenPort):\t%s\n", packet->payload);
    
     /************* SETTING UP REMOTE COMMUNICATION FROM SERVER SIDE TO CLIENT SIDE**********/


  /* create socket for connecting to troll */
    troll_M1_sock = socket(AF_INET, SOCK_DGRAM,0);
    if(troll_M1_sock < 0) {
        perror("Datagram Socket opened");
        exit(2);
    }else{
	printf("Datagram socket opened successfully \n");
  }

  /* Connecting to troll */
    M1_listen_name.sin_family = AF_INET;
    M1_listen_name.sin_port = htons(atoi(packet->payload));
    M1_listen_name.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(troll_M1_sock, (struct sockaddr *)&M1_listen_name, sizeof(M1_listen_name)) < 0) {
        perror("Unable to bind (TROLL_to_M1 Sock)");
    exit(2);
  }else{
	printf("Binding Successfully done for Connecting to TROLL \n");
  }
    
   fprintf(stdout,"Establishing connection : : SERVER --> CLIENT via troll at %d ...\n",ntohs(M1_listen_name.sin_port));
    
 
                                  /************** INFORMATION RECIEVED ALONG WITH THE DETAILED STATISTICS ************/
    
    init_recvbuff(buff_received, sizeof(buff_received)/sizeof(data_packet));
    printf("\n ***********************Information Received Statistics*************** \n\n\n");

    while(1){
        
        if(recvfrom(troll_M1_sock, packet, sizeof(data_packet), 0, (struct sockaddr *)&troll_M1_name, &length_name) < 0) {
            perror("error receiving");
            exit(4);
        }
        printf("[TCPD_M2 TO TCPD_M1] Received packet with Sequence Number: %d", packet->sequence_number);
        
              
        if(packet->sequence_number <= 2000 && packet->FYN != '1'){
             /*******CRC check **********/
            if(crcFast(packet->payload,1000) == packet->checksum) {
                
                printf("Packet Condition: GOOD\n");
                
                ack_packet->sequence_number = packet->sequence_number; //acknowledgement packet 
                
                /******** packet added to receiver buffer ********/
                if(buff_received[packet->sequence_number].sequence_number == -3){
                    buff_received[packet->sequence_number] = *packet ;
                }
                
                /************ Sending acknowledgement packet on proper acceptance***********/
                printf("[TCPD_M1 to TCPD_M2]  SEND ACK_PACKET with sequence_number: %d \n",ack_packet->sequence_number);
                if(sendto(M1_troll_sock, ack_packet, sizeof(data_packet), 0, (struct sockaddr *)&M1_troll_name, sizeof(M1_troll_name)) < 0) {
                    perror("error sending ack");
                    exit(4);
                }
                
               /*********** Transfer packets from TCPD_M1 to FTPC ***********/
                if(buff_received[buff_received_head].sequence_number != -3){
                    printf("[TCPD-M1 to FTPS]  SEND DATA_PACKET with sequence number: %d\n\n", buff_received[buff_received_head].sequence_number);
                    if(sendto(ftps_M1_sock, &buff_received[buff_received_head], sizeof(data_packet), 0, (struct sockaddr *)&ftps_M1_name, sizeof(ftps_M1_name)) < 0) {
                        perror("Transferring packets from TCPD_M1 to FTPC failed");
                        exit(4);
                    }
                    buff_received[buff_received_head].sequence_number = -3;
                    buff_received_head++; 
                }
            }
            else {
                printf("Packet Health: CORRUPTED %hu, %hu\n",crcFast(packet->payload,1000),packet->checksum);
                printf("[TCPD-M2 to TCPD-M1]  DROP DATA_PACKET with sequence_number: %d\n",packet->sequence_number);
            }
        }
        
        /****** Check : FYN  and  FYN+ACK ******/
        else if(packet->FYN == '1'){
            
            FYN_packet->FYN = '1';
            
            /******* CRC Check **********/
            if(crcFast(packet->payload,1000) == packet->checksum){
                
                printf("Packet Condition: GOOD\n");
                
                ack_packet->sequence_number = packet->sequence_number; //acknowledgement packet
		 
		/******** packet added to receiver buffer ********/
                if(buff_received[packet->sequence_number].sequence_number == -3){
                   buff_received[packet->sequence_number] = *packet ;
                }
                
                /*****Connection Shutdown check***/
                if(packet->FYN == '1' && ack_packet->sequence_number == 1){
                    printf("[TCPD_M1 to TCPD_M2] ACK Packet received with sequence_number: %d\n",ack_packet->sequence_number);
                    if(IsEmpty_recvbuff(buff_received,sizeof(buff_received)/sizeof(data_packet))){
                        if(sendto(ftps_M1_sock, packet, sizeof(data_packet), 0, (struct sockaddr *)&ftps_M1_name, sizeof(ftps_M1_name)) < 0) {
                            perror("Sending failed");
                            exit(4);
                        }
                    }
                    break;
                }
                else if(packet->FYN == '1'){
                    ack_packet->FYN = '1';
                    printf("[TCPD_M1 TO TCPD_M2]  SEND FYN+ACK PACKET-> sequence_number: %d \n",ack_packet->sequence_number);
                    if(sendto(M1_troll_sock, ack_packet, sizeof(data_packet), 0, (struct sockaddr *)&M1_troll_name, sizeof(M1_troll_name)) < 0) {
                        perror("Sending ack Failed");
                        exit(4);
                    }
                }
            }
            else {
                printf("Packet Condition: CORRUPTED\n");
                printf("[TCPD-M2 TO TCPD_M1]  DROP FYN||FYN+ACK PACKET-> sequence_number: %d\n",ack_packet->sequence_number);
            }
        }
        else{
            printf("Packet Condition: CORRUPTED\n");
            printf("[TCPD-M2 TO TCPD-M1] DROP PACKET SEQUENCE NUMBER CORRUPTED.\n");
        }
    }
    
    for(counter=buff_received_head;counter<1000;counter++){
        if(buff_received[counter].sequence_number != -3){
            while(sendto(ftps_M1_sock, &buff_received[counter], sizeof(data_packet), 0, (struct sockaddr *)&ftps_M1_name, sizeof(ftps_M1_name)) < 0) {
                perror("Sending failed");
                exit(4);
            }
        }		
    }
    
    fprintf(stdout, "\n\n....CONNECTION SHUTDOWN...\n");
    
    /* close connection */
    close(troll_M1_sock);
    close(ftps_M1_sock);
    exit(0);
}
