#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include "airportsim.h"

// returns a pointer to an aeroplane_t object with a flight number
// aeroplane_t->time_taken == NULL
aeroplane_t* generateAeroplane() {
    aeroplane_t* airplane = malloc(sizeof(aeroplane_t));
    airplane->flight_number = getFlightNumber();
    return airplane;
}

// returns a pointer to a char array containing the alphabet
char* generateAlphabet() {
    char *ptr = malloc(sizeof(char) * 26);
    for(char c='A'; c<='Z'; ++c)
        ptr[c-(int)'A'] = c;
    return ptr;
}

// returns a pointer to a char array containing a randomly generated flight number
char* getFlightNumber() {
    char *ptr = malloc(sizeof(char) * 6); // flight number will be 6 chars long
    sprintf(ptr, "%c%c%d%d%d%d", ALPHABET[randint(0, 25)], ALPHABET[randint(0, 25)],
            randint(0, 9), randint(0, 9), randint(0, 9), randint(0, 9));
    return ptr;
}

// searches parking array and returns a random index to an empty parking bay
// pre: there is at least one parking bay that is empty
int getFreeParkingBay() {
    int r, available = -1;
    while (available == -1) {
        r = randint(0,NUM_PARKING_BAYS-1);
        if (parking[r] == NULL)
            available = r;
    }
    return r;
}

// searches parking array and returns a random index to a full parking bay
// pre: there is at least one parking bay full
int getFullParkingBay() {
    int r, full = -1;
    while (full == -1) {
        r = randint(0,NUM_PARKING_BAYS-1);
        if (parking[r] != NULL)
            full = r;
    }
    return r;
}

// generates a random integer between min and max
// pre: min >= max
int randint(int min, int max) {
    return rand() % ((max + 1) - min) + min;
}

// returns a time delta in seconds, where then is a time in the past
double getTimeDiff(time_t then) {
    time_t now;
    time(&now);
    return difftime(now, then);
}

// prints the current state of the airport parking, waits for 
// semaphore to be released before printing.
void printAirport() {
    sem_wait(&mutex);
    printf("\nAirport State:\n");
    for (int i=0; i<NUM_PARKING_BAYS; ++i) {
        if (parking[i] == NULL) {
            printf("%d: Empty\n", i);
        } else {
            printf("%d: %s (has parked for %.2f seconds)\n", i,
                    parking[i]->flight_number, getTimeDiff(parking[i]->time_landed));
        }
    }
    printf("\n");
    sem_post(&mutex);
    return;
}

void printHeader() {
    printf("\nWelcome to the Airport Simulator.\n");
    printf("Press p or P followed by return to display the state of the airport.\n");
    printf("Press q or Q followed by return to terminate the simulation.\n\n");
    printf("Press return to start the simulation.\n");
    getchar();
    return;
}

// will free all memory that was allocated using malloc
void freeMemory() {
    for (int i=0; i<NUM_PARKING_BAYS; ++i) {
        if (parking[i] != NULL) {
            free(parking[i]->flight_number);
            free(parking[i]);
        }
    }
    free(parking);
    free(ALPHABET);
    return;
}

// will free a single aeroplane's allocated memory
void freeAeroplane(aeroplane_t* ptr) {
    free(ptr->flight_number);
    free(ptr);
    return;
}

