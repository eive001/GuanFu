#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

int doWithCheck(int returnValue, char* errorMessage);

const int bytesToWrite = 200;
const int bytesToRead = 100;

int main(){
  int pipefd[2];
  doWithCheck(pipe(pipefd), "pipe");

  int readEnd = pipefd[0];
  int writeEnd = pipefd[1];

  // P1 forks P2
  printf("P1 forks P2!\n");
  pid_t pid = doWithCheck(fork(), "fork");

  // P2
  if(pid == 0){
    // P2 forks P3
    printf("P2 forking P3!\n");
    pid_t p3_pid = doWithCheck(fork(), "fork");
    // P3
    if(p3_pid == 0){
        char p3_buf[bytesToRead];
        printf("P3 is reading from pipe!\n");
        int bytes = doWithCheck(read(readEnd, p3_buf, bytesToRead), "read");
        if(bytesToRead != bytes){
            fprintf(stderr, "Read less bytes than expected...");
            exit(1);
        }
        printf("P3 done!\n");
        return 0;
    }

    // P2 forks P4
    printf("P2 forking P4!\n");
    pid_t p4_pid = doWithCheck(fork(), "fork");
    // P4
    if(p4_pid == 0){
        char p4_buf[bytesToWrite];
        printf("P4 is writing to the pipe!\n");
        int bytes = doWithCheck(write(writeEnd, p4_buf, bytesToWrite), "write");
        if(bytesToWrite != bytes){
            fprintf(stderr, "Wrote less bytes than expected...");
            exit(1);
        }
        printf("P4 done!\n");
        return 0;
    }
    pid_t wpid2;
    int status2 = 0;
    while ((wpid2 = wait(&status2)) > 0);
    printf("P2 done!\n");
    return 0;
  }

  // P1
  char buffer[bytesToRead];
  printf("P1 is reading from pipe!\n");
  int bytes = doWithCheck(read(readEnd, buffer, bytesToRead), "read");
  if(bytesToRead != bytes){
      fprintf(stderr, "Read less bytes than expected...");
      exit(1);
  }

  pid_t wpid;
  int status = 0;
  while((wpid = wait(&status)) > 0);
  printf("P1 done!\n");
  return 0;
}

int doWithCheck(int returnValue, char* errorMessage){
  char* whyError = strerror(errno);
  if(returnValue == -1){
    fprintf(stderr, "%s failed: %s\n", errorMessage, whyError);
    exit(1);
  }
  return returnValue;
}
