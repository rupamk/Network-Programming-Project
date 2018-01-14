/*
 GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project submission
 FILE: ftpc Program (Client Application)
 */

#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "packet.h"

/*********** PORTS **********/
#define ftpc_M2_PORT 2468
#define M2_ftpc_PORT 8642

main(int argc, char *argv[])
{
  int ftpc_M2_sock, M2_ftpc_sock;
  char buffer[1000] ;
  char buff_name[20];
  struct sockaddr_in fptc_M2_name;		 				 
  struct sockaddr_in M2_ftpc_name; 						 
  struct hostent *hp, *gethostbyname();


  /********** Variables *************/
  int flag = 0;
  int flags;
  int zero_byte_flag = 0;
  socklen_t namelen;
  int filesz;          
  FILE *fp;		

 /********** Initializing Packet ***********/
  struct data_packet *packet;		
  struct data_packet *ack_packet;		
	
  packet = calloc(1,sizeof(struct data_packet));	
  ack_packet = calloc(1,sizeof(struct data_packet)); 	
	
 /*********** CRC calculation ***************/
  crcInit();				
  namelen = sizeof(struct sockaddr_in);	

	
/***************************************************** Main Program *******************************/
	
  if(argc < 2) {
    printf("Usage: client filename\n");
    exit(1);
  }
	
  /********* Creating Socket for sending to TCPD_M2 **********/
  ftpc_M2_sock = socket(AF_INET, SOCK_DGRAM,0);
  if(ftpc_M2_sock < 0) {
    perror("Datagram socket ftpc_M2_sock opened");
    exit(2);
  }else{printf("datagram socket opened successfully\n");}
  fptc_M2_name.sin_family = AF_INET;
  fptc_M2_name.sin_port = htons(ftpc_M2_PORT); 
  hp = gethostbyname("localhost");
 if(hp == 0) {
    fprintf(stderr, "%s:host not identified\n", argv[1]);
    exit(3);
  }
  bcopy((char *)hp->h_addr, (char *)&fptc_M2_name.sin_addr, hp->h_length);

 /****** Creating Socket to receive from TCPD_M2 **********/
  M2_ftpc_sock = socket(AF_INET, SOCK_DGRAM,0);
  if(M2_ftpc_sock < 0) {
    perror("Datagram socket M2_ftpc_sock opened");
    exit(2);
  }else{printf("datagram socket opened successfully\n");}
  flags = fcntl(M2_ftpc_sock, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(M2_ftpc_sock, F_SETFL, flags);

  M2_ftpc_name.sin_family = AF_INET;
  M2_ftpc_name.sin_port = htons(M2_ftpc_PORT);
  hp = gethostbyname("localhost");
  if(hp == 0) {
    fprintf(stderr, "host not identified\n");
    exit(3);
  }
  bcopy((char *)hp->h_addr, (char *)&M2_ftpc_name.sin_addr, hp->h_length);
  if(bind(M2_ftpc_sock, (struct sockaddr *)&M2_ftpc_name, sizeof(M2_ftpc_name)) < 0) { 		
    perror("Binding Error");
    exit(2);
  }else{printf("Binding successful\n");}
	
 /********** FILE Specifications ***********/
	
  if(strlen(argv[1]) > 20) {
    fprintf(stderr, "File Name < 20 Characters...\n");
  }
  if((fp = fopen(argv[1],"rb")) < 0){ 
    fprintf(stderr,"File could not be located \n");
    return 0;
  }
	
  fseek(fp,0,SEEK_END);
  filesz = (int) ftell(fp);
  rewind(fp);
	
  bzero(packet->payload, 1000);
  memcpy(packet->payload,&filesz,sizeof(int));
  packet->sequence_number = 0;
  bzero(buffer,1000);
  memcpy(buffer,packet,1000);
  packet->checksum = crcFast(buffer,1000);
  packet->size = 1000;
	
  fprintf(stdout,"Packet Sequence_Number:%d\tChecksum:%hu\n",packet->sequence_number,packet->checksum);
  if(sendto(ftpc_M2_sock, packet,sizeof(struct data_packet), 0, (struct sockaddr *)&fptc_M2_name, sizeof(fptc_M2_name)) <0) {
    perror("Erro: Datagram to M2");
    exit(1);
  }

  /******* Sending FILE details ***************/

  bzero(packet->payload,1000);
  memcpy(packet->payload,argv[1],strlen(argv[1]));
  packet->sequence_number = 1;
  bzero(buffer,1000);
  memcpy(buffer,packet->payload,1000);
  packet->checksum = crcFast(buffer,1000);
  packet->size = 1000;
	
  fprintf(stdout,"Packet data:%s\tSeq_Number:%d\tChecksum:%hu\n",packet->payload,packet->sequence_number,packet->checksum);
  if(sendto(ftpc_M2_sock, packet,sizeof(struct data_packet), 0, (struct sockaddr *)&fptc_M2_name, sizeof(fptc_M2_name)) <0) {
    perror("Error: Datagram to M2");
    exit(1);
  }
	
  /******* Sending FILE data ***************/

  while(1){
    int bytes_sent = 0;
    int bytes_read;
		
    sleep(1);
    if(flag == 0){
      bzero(packet->payload,1000);
      bytes_read = fread(packet->payload, sizeof(char),sizeof(packet->payload),fp);
	packet->size=bytes_read;

      if(bytes_read > 0){
	packet->sequence_number = packet->sequence_number+1;
	bzero(buffer,1000);
	memcpy(buffer,packet->payload,1000);
	packet->checksum = crcFast(buffer,1000);

	printf("\nClient sends:- Packet Sequence_Number: %d\tChecksum : %hu\n",packet->sequence_number,packet->checksum);
	bytes_sent = sendto(ftpc_M2_sock, packet,sizeof(struct data_packet), 0, (struct sockaddr *)&fptc_M2_name, sizeof(fptc_M2_name)) ;
	if (bytes_sent < 0) {
	  perror("Error: Datagram to M2");
	  exit(1);
	}
      }
      else if(bytes_read < 0 ){
	fprintf(stderr,"\n Reading failed \n");
      }
      else if(bytes_read == 0){
	sleep(1);
	flag =1;
	if(zero_byte_flag == 0){
	  zero_byte_flag = 1;
	 fprintf(stdout, "\nFILE read complete\n");
	  packet->FYN = '1';
	  packet->sequence_number = packet->sequence_number+1;
	  bzero(buffer,1000);
	  memcpy(buffer,packet->payload,1000);	
	  packet->checksum = crcFast(buffer,1000);

	  printf("\nClient sends:- FYN packet with SEQ_NO:\t%d",packet->sequence_number);
	  if(sendto(ftpc_M2_sock, packet,sizeof(struct data_packet), 0, (struct sockaddr *)&fptc_M2_name, sizeof(fptc_M2_name)) <0) {
	    perror("Error transmitting FYN PACKET to M2");
	    exit(1);
	  }
	}
      }
    }

              /* Acknowledgement and Shutdown */
    if(recvfrom(M2_ftpc_sock, buff_name, sizeof(buff_name), 0, (struct sockaddr *)&M2_ftpc_name, &namelen) > 0){
      if(strcmp(buff_name, "acked") == 0){
	printf("\nFYN packet acknowledged\n"); 
	sleep(2);
	fprintf(stdout, "\n\n....Connection Shutdown...\n"); 
	break;
      }
      else if(strcmp(buff_name, "full") == 0){
	printf("\n*****ERROR message from TCPD_M2: Buffer full****\n"); // in case the buffer is full
	flag = 1;
      }
      else{
	flag = 0;
      }
    }
  }	
}
