/*
 GOPI KRISHNA TUMMALA and RUPAM KUNDU
 The Ohio State Univerisity
 Network Programming: Final Project submission
 FILE: Jacobson Algorithm
 */
#define InitialRTO 250.0 //in ms

static float SRTT, RTTVAR;
static float g = .125, h = .25;
static double RTT = 250;

struct timeval timehash[1000] ;

void set_time(int seq_num){
    struct timeval tv;
	gettimeofday(&tv,NULL);
	timehash[seq_num] = tv ;
}

double calc_rtt(struct timeval ack_time, struct timeval packet_time){
  double ack_time_in_mill = ((ack_time.tv_sec) * 1000) + ((ack_time.tv_usec) / 1000) ;
  double packet_time_in_mill = ((packet_time.tv_sec) * 1000) + ((packet_time.tv_usec) / 1000) ;
  RTT = (ack_time_in_mill - packet_time_in_mill);
  return RTT;
}

void initialize(float InitialRTT){

	SRTT = InitialRTT;
	RTTVAR = InitialRTT / 2;
	printf("\nSelecting Default values: SRTT:%.1f \t RTTVAR: %.1f \t |\n",SRTT,RTTVAR);
}
/*Function to Calibrating the RTO value*/
float CalibrateRTO(){
	float err, RTO;
	
	if(RTT == -1.0){
		return InitialRTO;
	}
	err = RTT - SRTT;
	SRTT = SRTT + g * err;
	RTTVAR = RTTVAR + h * (abs(err) - RTTVAR);
	RTO = SRTT + 4* RTTVAR;
	
	printf("JACOBSONS ALGORITHM UPDATE:- RTT:%.1f | RTO:%.1f|SRTT:%.1f| RTTVAR: %.1f \t |\n",RTT, RTO, SRTT,RTTVAR);
			
	/*0if(RTO<1)
	{
		RTO=1;
	}	*/
	return RTO;
}
