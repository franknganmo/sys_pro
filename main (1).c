//
//  main.c
//  Systemprogrammierung
//
//  Created by Anaël laura sergine Ngome on 08.12.23.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    
    
    pid_t Aktuator_prozess = fork();
    
    // Fehlerbehandlung der Prozess, führt zum Programmabbruch
    
    if (Aktuator_prozess < 0) {
        perror("Aktuator_prozess war nicht erfolgreich!\n");
        exit(1);
    }
    
    // Kindprozess
    if (Aktuator_prozess == 0){
        printf("K: Die ID dieses Aktuator_prozess ist: %i\n", getpid());
        printf("K: Die ID des Elterns dieses Aktuator_prozess ist: %i\n", getppid());
        
        // Kommunikation zwischen mit Steuerung Prozess erzeugen
        
        int pipe_steuerung_verbindung[2];
        
        pipe(pipe_steuerung_verbindung); // Erzeugt eine Pipe unter Angabe, wo Ein- und Ausgang der Pipe platziert werden sollen
        
        // Fehlerbehandlung der Pipe
        if (pipe(pipe_steuerung_verbindung) == 1){
            perror("Fehler beim Erstellen der Pipe");
            exit(EXIT_FAILURE); }
        
        close(pipe_steuerung_verbindung[1]); // Schließe Schreibseite der Pipe
        
        // Lese Daten von der Pipe
        char received_commands = ' '; //
        
        read(pipe_steuerung_verbindung[0], &received_commands, sizeof(received_commands));
        
        // Shared Memory
        
        key_t key = ftok("/tmp", 'P'); // Erstelle einen Schlüssel, der eindeutig für den Shared Memory ist
        int shmid;
        
        // Erstelle oder greife auf den Shared Memory zu
        // Fehler Behandlung der shared memory
        if ((shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT)) == -1) {
            perror("Fehler beim Zugriff auf den Shared Memory");
            exit(EXIT_FAILURE);
        }
        
        // Attach den Shared Memory zum Prozess
        int *shared_data = (int *)shmat(shmid, NULL, 0);
        if ((intptr_t)shared_data == -1) {
            perror("Fehler beim Anhängen des Shared Memory");
            exit(EXIT_FAILURE);
        }
        
        // Lies Daten aus dem Shared Memory
        printf("Gelesene Daten aus Shared Memory: %d\n", *shared_data);
        
        // printf("Aktuatorprozess hat empfangen: %s\n", received_commands "und",*shared_data);
        
        // Ausfuhren der Befehle von der Aktuatorprozess
        
        while (1){
            
            if (received_commands == 00 && *shared_data == 1)
            {
                printf(" Error: Aktuator wird ausgeschaltet, Erwarmung der Raum beeintrachtigt"); }
            
            if (received_commands == 00 && *shared_data == 2)
            {
                printf(" Error: Aktuator wird ausgeschaltet, Kuhlung der Raum beeintrachtigt"); }
            
            if (received_commands == 11 && *shared_data == 1)
            {
                printf(" Success: Aktuator wird eingeschaltet, Der Raum wird erwarmt"); }
            if (received_commands == 11 && *shared_data == 2)
            {
                printf(" Success: Aktuator wird eingeschaltet, Der Raum wird abgekuhlt"); }
            sleep(20);
            
            // Detach den Shared Memory vom Prozess
            if (shmdt(shared_data) == -1) {
                perror("Fehler beim Trennen des Shared Memory");
                exit(EXIT_FAILURE); }
            
            close(pipe_steuerung_verbindung[0]); // Schließe Leseseite der Pipe
        }
            
        }
}



