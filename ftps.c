/*
 GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project submission 
 FILE: ftps Program (Server Application)
 */

#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "packet.h"
#define MAXLEN_MSG 1024
#define FTPS_M1_PORT 2009

int SEND(int sockfd, const void *msg, int len, unsigned int flags, const struct sockaddr *to, int tolen) {

  if(sendto(sockfd, msg, len, flags, to, tolen) <0) {
    perror("sending datagram message");
    exit(4);
  } else{printf("Datagram send successfully\n");}
  return 1;
}

int RECV(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen) {

  int receivedbytes = recvfrom(sockfd, buf, len, flags, from, fromlen);
  if (receivedbytes < 0) {
    perror("Sending datagram message");
    exit(4);
  }else{printf("Datagram send successfully\n");}
  return receivedbytes;
}

/***************************************************** Main Program *******************************/
main(int argc, char *argv[])
{
  int sock, buflen, length_name;
  char buf[MAXLEN_MSG];
  struct sockaddr_in name;
  struct hostent *hp, *gethostbyname();
    
   /********** Initializing file where server will receive and store ***********/
  int total_bytes_to_read;
  char* fileAddr = "./file/";
  char* fileName;
  FILE* fp;

   /********** Initializing Packet ***********/
  data_packet *packet;
  packet = calloc(1,sizeof(data_packet));

  if(argc < 1) {
    printf("syntax: ftps <Listen_port_number>\n");
    exit(1);
  }

  /********* Creating Socket for connecting to server ************/
  sock = socket(AF_INET, SOCK_DGRAM,0);
  if(sock < 0) {
    perror("opening datagram socket");
    exit(2);
  }else{printf("Datagram socket opened successfully\n");}
  name.sin_family = AF_INET;
  name.sin_port = htons(FTPS_M1_PORT);
  hp = gethostbyname("localhost");
  if(hp == 0) {
    fprintf(stderr, "%s:host not identified\n", argv[1]);
    exit(3);
  }
  bcopy((char *)hp->h_addr, (char *)&name.sin_addr, hp->h_length);

  /********** sharing remote port number to TCPD_M1 *******************/

  memcpy(packet->payload,argv[1],strlen(argv[1]));

  printf("FTPS sends ListenPORT:\%s\n",packet->payload);
  if(SEND(sock, packet, sizeof(data_packet), 0, (struct sockaddr *)&name, sizeof(name)) <0) {
    perror("sending datagram message");
    exit(4);
  }else{printf("Datagram send successfully\n");}
  length_name=sizeof(struct sockaddr_in);

  /********* Receiving File **********/

  while(1){

    if(RECV(sock, packet, sizeof(data_packet),0,(struct sockaddr *)&name, &length_name) < 0) {
      perror("Receiving Error:");
      exit(4);
    }

    printf("Packet Sequence Number:\t%d\n",packet->sequence_number);
		
    if(packet->FYN == '1'){
       printf("**********FILE RECEIVED ************\n");
      printf("FIN packet recieved...Closing Connection...\n"); // when FIN is received
      fclose(fp);
      printf("....Connection Shutdown...\n");
      break;
    }

    if(packet->sequence_number == 1){

      /********* name of the file to be received **********/
      fprintf(stdout,"Incoming File Name:\t%s\n",packet->payload);

      /*********** Allocating the file **********/

      fileName  = malloc(strlen(fileAddr)+strlen(packet->payload)+1);
      strcpy(fileName,fileAddr);
      strcat(fileName,packet->payload);
		
      printf("File storage details:\t%s\n",fileName);
		   
      /* Creating File */
      if((fp = fopen(fileName,"w")) < 0){
	fprintf(stderr,"File could not be located\n");
	exit(5);
      } 
    }

    /********* Writing the file to the destined location *********/
    if(packet->sequence_number > 1) {
      if(packet->size >0){
	fwrite(packet->payload,sizeof(char),packet->size,fp);
      }
      
    }
  }
    if(strlen(packet->payload) < 1001 && packet->sequence_number > 3){
      printf("*************************************FILE RECEIVED ****************************\n");
      printf("....Connection Shutdown...*\n");
      exit(0);
    }
	
  /* close connection */
  close(sock);
  exit(0);
}
