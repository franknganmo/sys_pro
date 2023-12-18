
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>

// Shared Memory länge
#define SHM_SIZE sizeof(int)

// Structure erstellen
struct temp
{
    int command;
    float temperature;
    int pipe_fd[2];
    key_t shm_key;
    int shm_id;
    float mean;
    float stddev;
    int* shm_data;
    ssize_t schreib;
};

float rand_normal(void);

int main(void)
{
    struct temp t;
    t.command = 0;

    // Erstellen einer Pipe für die Kommunikation mit dem Steuerungsprozess
    if (pipe(t.pipe_fd) == -1)
    {
        perror("Fehler beim Erstellen der Pipe");
        exit(EXIT_FAILURE);
    }

    // Erstellen eines Shared Memory-Bereichs
    // t.shm_key = ftok(".", 'x');
    t.shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    // shmID = shmget(IPC_PRIVATE, SHMSEGSIZE,IPC_CREAT | 0644);
    
    if (t.shm_id == -1)
    {
        perror("Fehler beim Erstellen des Shared Memory");
        exit(EXIT_FAILURE);
    }

    t.shm_data = (int*)shmat(t.shm_id, NULL, 0);
    if (t.shm_data == (void*)-1)
    {
        perror("Erreur lors de l'attachement du segment de mémoire partagée");
        exit(EXIT_FAILURE);
    }

    // Forken, um den Sensorprozess zu starten
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Fehler beim Forken");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        close(t.pipe_fd[0]); // Schließe das Lese-Ende von Kind

        while (1)
        {
            // Simulation von Temperatur Wert
            if (t.command == 0)
            {
                t.mean = 22.5;      // Température moyenne (entre 0 et 45)
                t.stddev = 7.5;     // Écart-type (ajustable selon vos besoins)
                t.temperature = t.mean + t.stddev * rand_normal();

                printf("Température ist : %.f\n", t.temperature);

                // Daten an den Steuerungsprozess senden (über Pipe)
                ssize_t schreib = write(t.pipe_fd[1], &t.temperature, sizeof(float));
                printf("Temperatur ist gesendet durch pipe\n");
                
                if (schreib == -1)
                {
                    perror("Fehler beim Schreib in Pipe");
                    exit(EXIT_FAILURE);
                }
            
                // Warten, bevor ein neuer Wert gesendet wird
                // sleep(30);
                
            }

            // Simulation von Abkühlung des Raums
            if (t.command == 2)
            {
                while (t.command == 2)
                {
                    t.shm_data = (int*)shmat(t.shm_id, NULL, 0);
                    t.temperature--;
                    t.schreib = write(t.pipe_fd[1], &t.temperature, sizeof(float));

                    if (t.schreib == -1)
                    {
                        perror("Fehler beim Schreib in Pipe");
                        exit(EXIT_FAILURE);
                    }

                    printf("Temperatur ist gesendet durch pipe\n");

                    // Warten, bevor ein neuer Wert gesendet wird
                    sleep(30);
                    t.command = *t.shm_data;
                }
            }

            // Simulation von Erwärmung des Raums
            if (t.command == 1)
            {
                while (t.command == 1)
                {
                    t.shm_data = (int*)shmat(t.shm_id, NULL, 0);
                    t.temperature++;
                    t.schreib = write(t.pipe_fd[1], &t.temperature, sizeof(float));

                    if (t.schreib == -1)
                    {
                        perror("Fehler beim Schreib in Pipe");
                        exit(EXIT_FAILURE);
                    }
                    printf("Temperatur ist gesendet durch pipe\n");

                    // Warten, bevor ein neuer Wert gesendet wird
                    sleep(30);
                    t.command = *t.shm_data;
                }
            }

            // Warten, bevor ein neuer Wert generiert wird
            sleep(30);
            t.shm_data = (int*)shmat(t.shm_id, NULL, 0);
            t.command = *t.shm_data;
            shmdt(t.shm_data);
        }

        close(t.pipe_fd[1]); // Schließe das Schreib-Ende von Zweite Pipe
    }

    // Warten auf den Sensorprozess
    wait(NULL);

    // Freigabe des Shared Memory-Bereichs
    shmctl(t.shm_id, IPC_RMID, NULL);

    return 0;
}

// Kleine Random Funktion
float rand_normal(void)
{
    float u1 = rand() / (RAND_MAX + 1.0);
    float u2 = rand() / (RAND_MAX + 1.0);
    return sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
}



