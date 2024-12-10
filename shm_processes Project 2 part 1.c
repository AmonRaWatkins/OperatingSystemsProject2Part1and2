#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <wait.h>

#define MAX_STUDENTS 10

// Function prototypes
void DearOldDad(int *bankAccount, int semid);
void LovableMom(int *bankAccount, int semid);
void PoorStudent(int *bankAccount, int semid);

// Semaphore operations
void P(int semid) {
    struct sembuf sb = {0, -1, 0}; // Wait (P operation)
    semop(semid, &sb, 1);
}

void V(int semid) {
    struct sembuf sb = {0, 1, 0}; // Signal (V operation)
    semop(semid, &sb, 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_dads> <num_students>\n", argv[0]);
        exit(1);
    }

    int numDads = atoi(argv[1]);
    int numStudents = atoi(argv[2]);
    
    if (numDads < 1 || numStudents < 1 || numStudents > MAX_STUDENTS) {
        fprintf(stderr, "Invalid number of parents or students.\n");
        exit(1);
    }

    // Create shared memory for the bank account
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    
    int *bankAccount = (int *)shmat(shmid, NULL, 0);
    if (bankAccount == (int *)-1) {
        perror("shmat");
        exit(1);
    }
    
    *bankAccount = 0; // Initial balance

    // Create semaphore for synchronization
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1); // Initialize semaphore to 1

    srand(time(NULL)); // Seed for random number generation

    // Fork processes for Dear Old Dad
    for (int i = 0; i < numDads; i++) {
        if (fork() == 0) {
            DearOldDad(bankAccount, semid);
            exit(0);
        }
    }

    // Fork process for Lovable Mom
    if (fork() == 0) {
        LovableMom(bankAccount, semid);
        exit(0);
    }

    // Fork processes for Poor Students
    for (int i = 0; i < numStudents; i++) {
        if (fork() == 0) {
            PoorStudent(bankAccount, semid);
            exit(0);
        }
    }

    // Wait for all child processes to finish
    while (wait(NULL) > 0);

    // Cleanup
    shmdt(bankAccount);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}

void DearOldDad(int *bankAccount, int semid) {
    while (1) {
        sleep(rand() % 6); // Sleep between 0-5 seconds
        printf("Dear Old Dad: Attempting to Check Balance\n");

        int check = rand() % 2; // Random number to decide action
        P(semid); // Enter critical section

        if (check == 0) { // Even: Try to deposit money
            int localBalance = *bankAccount;
            if (localBalance < 100) {
                int amount = rand() % 100 + 100; // Random deposit between $100 - $199
                localBalance += amount;
                printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                *bankAccount = localBalance;
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
        } else { // Odd: Just check balance
            printf("Dear Old Dad: Last Checking Balance = $%d\n", *bankAccount);
        }

        V(semid); // Exit critical section
    }
}

void LovableMom(int *bankAccount, int semid) {
    while (1) {
        sleep(rand() % 11); // Sleep between 0-10 seconds
        printf("Lovable Mom: Attempting to Check Balance\n");

        P(semid); // Enter critical section

        int localBalance = *bankAccount;
        if (localBalance <= 100) { 
            int amount = rand() % 125 + 125; // Random deposit between $125 - $249
            localBalance += amount;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n", amount, localBalance);
            *bankAccount = localBalance;
        }

        V(semid); // Exit critical section
    }
}

void PoorStudent(int *bankAccount, int semid) {
    while (1) {
        sleep(rand() % 6); // Sleep between 0-5 seconds
        printf("Poor Student: Attempting to Check Balance\n");

        P(semid); // Enter critical section

        int check = rand() % 2; // Random number to decide action

        if (check == 0) { // Even: Try to withdraw money
            int localBalance = *bankAccount;
            int need = rand() % 50 + 50; // Random need between $50 - $99
            printf("Poor Student needs $%d\n", need);

            if (need <= localBalance) {
                localBalance -= need;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
                *bankAccount = localBalance;
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
        } else { // Odd: Just check balance
            printf("Poor Student: Last Checking Balance = $%d\n", *bankAccount);
        }

        V(semid); // Exit critical section
    }
}