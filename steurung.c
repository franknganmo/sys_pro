#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Shared Memory länge
#define SHM_SIZE sizeof(int)

// Prototipler
void evaluate_temperature(float temperature, int control_command, int shm_id);

int main(void) {
    int control_command=0;
    int sensor_pipe[2];
    int pipe_actuator[2];
    int shm_id;
    float temperature;
    int schaltung_control;
    
    printf("Steuerungprozess gestartet");

    // 1. Pipe: Sensörden değer almak için
    if (pipe(sensor_pipe) == -1 || pipe(pipe_actuator) == -1) {
        perror("Error creating sensor pipe");
        exit(EXIT_FAILURE);
    } else {
        printf("Pipe erstellt\n");
    }

    // Shared Memory oluşturuluyor
    shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);

    if (shm_id == -1) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    } else {
        printf("Shared Memory erstellt\n");
    }

    int* shm_data = (int*)shmat(shm_id, NULL, 0);
    
    if (shm_data == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de mémoire partagée");
        exit(EXIT_FAILURE);
    } else {
        printf("Shared Memory angehängt\n");
    }

    // Prozess
    pid_t pid_sensor = fork();
    
    if (pid_sensor == -1) {
        perror("Error forking sensor process");
        exit(EXIT_FAILURE);
    }

    if (pid_sensor > 0) {
        // Sensor process
        close(sensor_pipe[1]);
        printf("If-Schleife\n");

        while (1) {
            read(sensor_pipe[0], &temperature, sizeof(float));
            evaluate_temperature(temperature, control_command, shm_id);
            // Warten, bevor ein neuer Wert generiert wird
            sleep(30);
            // Warten auf den Sensorprozess
            // wait(NULL);
        }
        close(sensor_pipe[0]); // Schließe Leseseite der Pipe
        
        return 0;
    }
}

void evaluate_temperature(float temperature, int control_command, int shm_id) {
        // Sıcaklık değerlendirmesi
        int schaltung_control;
        int pipe_actuator[2];
        int sensor_pipe[2];
     
        if (temperature < 15.0) {
            while (temperature > 20.0 && temperature <= 21.0) {
                control_command = 1;
                int *shm_data = (int*)shmat(shm_id, NULL, 0);
                *shm_data = control_command;
                // detach shared memory von str prozess
                shmdt(shm_data);
                
                schaltung_control = 1;
                write(pipe_actuator[1], &schaltung_control, sizeof(int));
                printf("Klimaanlage schaltet ein (Heizung)\n");
                sleep(20);
                read(sensor_pipe[0], &temperature, sizeof(float));
            }
        }
        if (temperature > 25.0) {
            while (temperature > 20.0 && temperature <= 21.0) {
                control_command = 2;
                int *shm_data = (int*)shmat(shm_id, NULL, 0);
                *shm_data = control_command;
                shmdt(shm_data);
                
                schaltung_control = 1;
                write(pipe_actuator[1], &schaltung_control, sizeof(int));
                printf("Klimaanlage schaltet ein (Kühlung)\n");
                sleep(20);
                read(sensor_pipe[0], &temperature, sizeof(float));
            }
        }

        if (temperature >= 15.0 && temperature < 25.0) {
            control_command = 0;
            printf("Klimaanlage wird nicht eingeschaltet.\n");
            int* shm_data = (int*)shmat(shm_id, NULL, 0);
            *shm_data = control_command;
            shmdt(shm_data);
            
            schaltung_control = 0;
            write(pipe_actuator[1], &schaltung_control, sizeof(int));
            read(sensor_pipe[0], &temperature, sizeof(float));
        }
    }