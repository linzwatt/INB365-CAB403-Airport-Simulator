#define _BSD_SOURCE // allows usleep function to be defined in 'unistd.h'

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "airportsim.h"


int main(int argc, char* argv[]) {
	pthread_t landing_thread, takeoff_thread, monitor_thread;
	srand((unsigned)time(NULL)); // seed the random number generator with the system time
	ALPHABET = generateAlphabet();
	running = 1; // variable that allows landing and takeoff threads to run
	planesParked = 0; // will keep track of airport state
	parking = malloc(NUM_PARKING_BAYS * sizeof(aeroplane_t));

	// Terminal command error checking
	if (argc != 3) {
		fprintf(stderr, "usage: %s <landing probability> <takeoff probability>\n", argv[0]);
		return -1;
	}
	// get numbers from argument list
	landing_prob = atoi(argv[1]);
	takeoff_prob = atoi(argv[2]);
	if (landing_prob < MIN_PROB || landing_prob > MAX_PROB ||
		takeoff_prob < MIN_PROB || takeoff_prob > MAX_PROB) {
		fprintf(stderr, "probabilities must be between 1 and 90 inclusive.\n");
		return -1;
	}

	printHeader(); // prints header and returns when 'return' key is pressed

	// initialise semaphore
	if (sem_init(&mutex, 0, 1)) {
		fprintf(stderr, "Could not create a semaphore");
		return -1;
	}
	
	// spawn 3 threads
	pthread_create(&landing_thread, NULL, landing_function, NULL);
	pthread_create(&takeoff_thread, NULL, takeoff_function, NULL);
	pthread_create(&monitor_thread, NULL, monitor_function, NULL);	
	// wait for 3 threads to finish execution
	pthread_join(landing_thread, NULL);
	pthread_join(takeoff_thread, NULL);
	pthread_join(monitor_thread, NULL);

	printAirport();

	freeMemory(); // frees all allocated memory
	sem_destroy(&mutex);

	return EXIT_SUCCESS;
}

void* landing_function(void *arg) {
	while (running) {
		sem_wait(&mutex); // wait for other thread to release the semaphore
		if (planesParked < NUM_PARKING_BAYS) { // if there is room in the airport
			if (randint(0,99) < landing_prob) { // test probability
				int p = getFreeParkingBay();
				parking[p] = generateAeroplane();
				printf("Plane %s is landing...\n", parking[p]->flight_number);
				sleep(LANDING_TIME); // simulate landing time
				time(&parking[p]->time_landed); // assign landing time once plane has landed
				printf("Plane %s parked in landing bay %d.\n", parking[p]->flight_number, p);
				planesParked++;
			}
			if (planesParked >= NUM_PARKING_BAYS) { // if no more room in the airport
				printf("Airport is full\n\n");
			}
		}
		sem_post(&mutex); // release semaphore to other threads

		// while no room in the airport, block thread
		while ((planesParked >= NUM_PARKING_BAYS) && running) {
			usleep(BLOCKING_SLEEP_TIME);  // sleep for a short time to reduce CPU usage
		}

		// sleep for specified time before next aeroplane generated
		usleep(LANDING_SLEEP_TIME);
	}
	return NULL;
}

void* takeoff_function(void *arg) {
	while (running) {
		sem_wait(&mutex);
		if (planesParked > 0) { // if there are aircraft in the airport
			if (randint(0,99) < takeoff_prob) {
				int p = getFullParkingBay();
				printf("After staying at bay %d for %.2f seconds, plane %s is taking off...\n",
						p, getTimeDiff(parking[p]->time_landed), parking[p]->flight_number);
				sleep(TAKEOFF_TIME);
				printf("Plane %s has finished taking off.\n", parking[p]->flight_number);
				freeAeroplane(parking[p]); // free memory allocated to the aircraft
				parking[p] = NULL; // ensure pointer is NULL
				planesParked--;
			}
			if (planesParked <= 0) { // if airport is now empty
				printf("Airport is empty\n\n");
			}
		}
		sem_post(&mutex);

		// while airport is empty, block thread
		while ((planesParked <= 0) && running) {
				usleep(BLOCKING_SLEEP_TIME);
		}

		// sleep for specified time before next aeroplane generated
		usleep(TAKEOFF_SLEEP_TIME);
	}
	return NULL;
}

void* monitor_function(void *arg) {
	char console_input; // will store single char read from console
	while (running) {
		scanf("%c", &console_input);
		switch (console_input) {
			case 'p':
			case 'P':
				// print airport state case
				printAirport();
				break;
			case 'q':
			case 'Q':
				// terminate program case
				running = 0;
				break;
		}
	}
	return NULL;
}
