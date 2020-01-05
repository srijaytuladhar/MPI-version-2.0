#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include "timer.c"


/*********************************************************
 * 
 * 
 * To Compile:
 * mpicc -o mpi_password_cracking mpi_password_cracking.c -lcrypt
 * 
 * 
 * To Run:
 * mpirun -n 3 ./mpi_password_cracking
 * 
 * 
 *********************************************************/


// no. of passwords i.e. 4
int no_of_passwords = 4;

// encrypted passwords for 4 passwords
char *passwords_encrypted[] = {

 "$6$KB$0G24VuNaA9ApVG4z8LkI/OOr9a54nBfzgQjbebhqBZxMHNg0HiYYf1Lx/HcGg6q1nnOSArPtZYbGy7yc5V.wP/",
  "$6$KB$WHGPdZpea02k5HC4Xe3b.hPfVKNmhB1N1PH2dipVWPaiIjfeXXw2/CKM/96w3KXJpy/bVmSKvPVEbxRcPbxKp/",
  "$6$KB$Z/wMw97.MbsjxFvFZacJuQYiyegBaODZm0KBZFiXMVhWxywV/LC5UD9YRr0WYUnzLcc65r886iJwORXyEf2eY.",
  "$6$KB$ukW6NZ71V8.a9.sQCa88rLsbdXXk4eojRocZEycdlKk9KXa2vLl4xzsFrARouuGo2xDzudYFHYo.qdc51OlkB0"
};

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

/* crack_password is used find cracked passwords where, '#' is used to denote the matched password */

// for kernel function 1
void kernel_function_1(char *salt_and_encrypted){
  int fi, si, td;  // fi -> first initial, si -> second initial, td -> two digits; as counters
  char salt[7];    // String used in hashing the password. Need space
  char plain[7];   // The combination of letters currently being checked
  char *enc;       // Pointer to the encrypted password
  int count = 0;   // The number of combinations explored so far

  substr(salt, salt_and_encrypted, 0, 6);

  for(fi='A'; fi<='M'; fi++){
    for(si='A'; si<='Z'; si++){
      for(td=0; td<=99; td++){

       //printf("Instance 1");
        sprintf(plain, "%c%c%02d", fi, si, td); 
        enc = (char *) crypt(plain, salt);
        count++;
        if(strcmp(salt_and_encrypted, enc) == 0){
          printf("#%-8d%s %s\n", count, plain, enc);	// '#' is used to denote the matched passwords
        } 
      }
    }
  }
}

// for kernel function 2
void kernel_function_2(char *salt_and_encrypted){
  int fi, si, td;     
  char salt[7];   
  char plain[7];   
  char *enc;       
  int count = 0;   

  substr(salt, salt_and_encrypted, 0, 6);

  for(fi='N'; fi<='Z'; fi++){
    for(si='A'; si<='Z'; si++){
      for(td=0; td<=99; td++){

        sprintf(plain, "%c%c%02d", fi, si, td); 
        enc = (char *) crypt(plain, salt);
        count++;
        if(strcmp(salt_and_encrypted, enc) == 0){
          printf("#%-8d%s %s\n", count, plain, enc);	// '#' is used to denote the matched passwords
        } 
      }
    }
  }
}




int main(int argc, char *argv[]){

  // initializing timer.c components
  struct timespec timer_start, timer_stop;   
  long long int time_taken_for_execution;
  
  // timer starts here
  clock_gettime(CLOCK_MONOTONIC, &timer_start);

  printf("\n===============================================================================\n");
  printf("!! CRACKED PASSWORD !! \n");
  printf("===============================================================================\n\n");
  

  int loop;		// counter 
  int mpi_size, mpi_rank;	// size and rank for sender and receiver
  
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  if(mpi_size != 3) {
    if(mpi_rank == 0) {
      printf("This program requires 3 processes to run!!!\n");
    }
  } 
  else {
    if(mpi_rank ==0){
      
      int var1;		// variable 1
      int var2;		// variable 2

      MPI_Send(&var1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);  
      MPI_Send(&var2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    } 

    else {
      if(mpi_rank == 1){
     
        int var3;		// variable 3

        MPI_Recv(&var3, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for (loop = 0; loop<no_of_passwords; loop<loop++) {
		    kernel_function_1(passwords_encrypted[loop]);
        }
      }
      else if(mpi_rank == 2) {
        int var4;		// variable 4
        MPI_Recv(&var4, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for (loop = 0; loop<no_of_passwords; loop<loop++) {
		    kernel_function_2(passwords_encrypted[loop]);
        }
      }
    }
  }
  
  MPI_Finalize(); 

  // timer stops here
  clock_gettime(CLOCK_MONOTONIC, &timer_stop);
  timer_calc(&timer_start, &timer_stop, &time_taken_for_execution);
  
  // output of time taken for execution  is displayed
  printf("\n\n===============================================================================\n");
  printf("!! TIME TAKEN FOR EXECUTION !! \n");
  printf("===============================================================================\n\n");
  printf("Nanoseconds: %lld\n", time_taken_for_execution); 
  printf("Seconds: %0.9lf\n", ((time_taken_for_execution/1.0e9))); 
  printf("Minutes: %0.4lf\n", ((time_taken_for_execution/1.0e9)/60));
  printf("Hours: %0.2lf\n", ((time_taken_for_execution/1.0e9)/3600)); 

  return 0;
}
