#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Funktion Prototyp
int generateRandomValue(int *data, int *initialRandomValue, int *initialRandomValue2);
void control(int randomValue, int *data,int *data2 ,int *initialRandomValue2);
void provideAir(int *data, int *data2); // Parametreleri düzeltildi

int main() {
    // Seed für die Erzeugung von Zufallszahlen festlegen
    srand(time(NULL));

    // Setzen Sie den Datenwert zunächst auf 0
    int data = 0;
    int data2 = 0; 
    int initialRandomValue;
    int initialRandomValue2;

    // Arbeit in einer Endlosschleife
    while (1) {
        // Neuen Zufallswert und Anfangswerte ermitteln
        initialRandomValue = generateRandomValue(&data, &initialRandomValue, &initialRandomValue2);

        // Anzeige des gesendeten Wertes in der Kontrollfunktion
      
        printf("Gesendeter Temperatur: %d\n", initialRandomValue2);

        // Kontrollfunktion aufrufen
        control(initialRandomValue2, &data, &data2, &initialRandomValue2);

        // Schlafzeit 
        sleep(5);
    }

    return 0;
}

// Zufallswert erzeugende Funktion
int generateRandomValue(int *data, int *initialRandomValue, int *initialRandomValue2) {
    // Wenn die Daten 0 sind, wird ein neuer Zufallswert ermittelt und als Anfangswert gespeichert.
    if (*data == 0) {
        *initialRandomValue = rand() % 41;
        *initialRandomValue2 = *initialRandomValue; // initialRandomValue2'yi initialRandomValue ile aynı yap
        printf("Anfänglicher Zufallswert: %d\n", *initialRandomValue);
    }

    // Wenn die Daten 1 sind, wird der Anfangswert verringert.
    if (*data == 1) {
        (*initialRandomValue2)++;
        printf("Anfänglicher Zufallswert Erhöht: %d\n", *initialRandomValue2);
  }

    // Wenn die Daten 2 sind, wird der Ausgangswert alle 2 Sekunden verringert.
    if (*data == 2) {
        (*initialRandomValue2)--;
        printf("Anfänglicher Zufallswert Verringert: %d\n", *initialRandomValue2);
        sleep(2);
    }

    return *initialRandomValue2;
}

// Funktion zur Kontrolle des Zufallswertes
void control(int randomValue, int *data, int *data2, int *initialRandomValue2) {
    if (randomValue >= 15 && randomValue <= 25 && *data == 0) {
        printf("Zufallswert zwischen 15 und 25. Info Beispiel %d\n", *data);
        // Wenn Daten 1 oder 2 sind, setzen Sie data auf 0 und *data2 auf 0
        if (*data == 1 || *data == 2) {
            *data = 0;
            *data2 = 0;
        }
    } else {
        if (*initialRandomValue2 == 20) {
            printf("Der Raum hat die gewünschte Temperatur erreicht. Information: Örnek %d\n", *data);
            // Wenn data=1 oder data=2 sind, Daten auf 1 setzen

            if (*data == 1 || *data == 2) {
                *data = 0;
            }
        }

        if (*initialRandomValue2 < 20) {
            printf("Zufallswert kleiner als 20. Info Beispiel %d\n", *data);
            // Wenn Daten 0 sind, setzen Sie Daten auf 1 und *Daten2 auf 11
            if (*data == 0) {
                *data = 1;
                *data2 = 11;
                provideAir(data, data2);
            }
        } else if (*initialRandomValue2 > 20) {
            printf("Zufallswert größer als 20. Info Beispiel %d\n", *data);
            // Wenn Daten 0 sind, setzen Sie Daten auf 2 und *Daten2 auf 11
            if (*data == 0) {
                *data = 2;
                *data2 = 11;
                provideAir(data, data2);
            }
        }
    }
}

// Funktion der Luftzufuhr
void provideAir(int *data, int *data2) {
    if (*data2 == 11) {
        printf("Klimaanlage aktiv\n");
        if (*data == 1) {
            printf("Es wird heiße Luft zugeführt.\n");
        } else if (*data == 2) {
            printf("Es wird kalte Luft zugeführt.\n");
        }
    }
    if (*data2 == 0) {
        printf("Klimaanlage nicht aktiv\n");
    }
}