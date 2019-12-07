#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int [],int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;
  
     int arr[2];  
    
     int buffer[10]; //using array to store 10 integers
     pipe(arr);

  
  if (argc != 11) { //to get 10 arguments
    printf("Use: %s #1 #2 #3 #4 ... \n", argv[0]);
    exit(1);
  }
  
  ShmID = shmget(IPC_PRIVATE, 10*sizeof(int), IPC_CREAT | 0666);
  if (ShmID < 0) {
    printf("*** shmget error (server) ***\n");
    exit(1);
  }
  printf("Server has received a shared memory of ten integers...\n");

  // pointer to point the shared memory segment
  ShmPTR = (int *) shmat(ShmID, NULL, 0);
  
  if (*ShmPTR == -1) {
    printf("*** shmat error (server) ***\n");
    exit(1);
  }
  printf("Server has attached the shared memory...\n");

  // using loop to read arguments
  int i;
  for (i =0; i < 10; i++){
    ShmPTR[i] = atoi(argv[i+1]);
  }
  printf("Server is about to fork a child process...\n");
  pid = fork();
  if (pid < 0) {
    printf("*** fork error (server) ***\n");
    exit(1);
  }
  else if (pid == 0) {
    printf("Child: ");
    ClientProcess(ShmPTR, arr);
    exit(0);
  }

  wait(&status);
  printf("Server has detected the completion of its child...\n");
  close(arr[1]);

  printf("Parent: ");
  for(i = 0; i < 10; i++){
    read(arr[0], buffer, sizeof(int));
    printf("%d  ",*buffer);
  }
  printf("\n");
  
  shmdt((void *) ShmPTR);
  printf("Server has detached its shared memory...\n");
  shmctl(ShmID, IPC_RMID, NULL);
  printf("Server has removed its shared memory...\n");
  printf("Server exits...\n");
  exit(0);
}


void  ClientProcess(int  SharedMem[], int arr[]) 
{
  printf("   Child process started\n");
  printf("Child: ");
  for (int i = 0; i < 10; i++){
    printf("%d  ",SharedMem[i]);
  }
  printf("\n");

  close(arr[0]);
  
  for(int j = 0; j < 10; j++){
    write(arr[1], &SharedMem[j], sizeof(int));
  }
}