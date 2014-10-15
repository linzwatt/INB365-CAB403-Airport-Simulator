// conditional statement to prevent multiple includes
#ifndef __AIRPORT_SIM_H__
#define __AIRPORT_SIM_H__

#define NUM_PARKING_BAYS 10
#define MIN_PROB 1
#define MAX_PROB 90
#define BLOCKING_SLEEP_TIME 10000  // micro seconds
#define TAKEOFF_SLEEP_TIME 500000
#define LANDING_SLEEP_TIME 500000
#define TAKEOFF_TIME 2 // seconds
#define LANDING_TIME 2

typedef struct aeroplane {
	char* flight_number;
	time_t time_landed;
} aeroplane_t;

// function prototypes
void printAirport();
void printHeader();
char* getFlightNumber();
char* generateAlphabet();
int getFreeParkingBay();
int getFullParkingBay();
void freeMemory();
void freeAeroplane(aeroplane_t* ptr);
int randint(int min, int max);
double getTimeDiff(time_t then);
aeroplane_t* generateAeroplane();
void* landing_function(void *arg);
void* takeoff_function(void *arg);
void* monitor_function(void *arg);

// variable declarations
char* ALPHABET;
int landing_prob, takeoff_prob, running, planesParked;
sem_t mutex;
aeroplane_t** parking;

#endif
