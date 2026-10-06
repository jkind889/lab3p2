#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#define BankAccount 0
#define Turn 1

void  ClientProcess(int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;


     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of two integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");

     ShmPTR[BankAccount] = 0;
     ShmPTR[Turn] = 0;
     printf("BankAccount = %d, Turn = %d\n",
            ShmPTR[BankAccount], ShmPTR[Turn]);

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ClientProcess(ShmPTR);
          exit(0);
     }

     int account;
     int balance;

     for(int i = 0; i<25; i++) 
     {
        sleep(rand() % 6);
        account = ShmPTR[BankAccount];
        while (ShmPTR[Turn] != 0);
        
        if (account <= 100) {
          balance = rand() % 101;

          if (balance % 2 == 0) {
            account+= balance;

            printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
          }
          else {
            printf("Dear old Dad: Doesn't have any money to give \n");
          }
        }
       else {
          printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

          ShmPTR[BankAccount] = account;
          ShmPTR[Turn] = 1;
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

void  ClientProcess(int  SharedMem[])
{

  int account;
  int balance;

    for(int i = 0; i<25; i++) 
     {
        sleep(rand() % 6);

        account = SharedMem[BankAccount];
        while (SharedMem[Turn] != 1);
        
        balance = rand() % 51;
        printf("Poor Student needs $%d\n", balance);
        if (balance <= account) {
            account -= balance;

            printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        }
       else {
          printf("Poor Student: Not Enough Cash ($%d)\n", account);
          }
        SharedMem[BankAccount] = account;
        SharedMem[Turn] = 0;
        
     }
}
