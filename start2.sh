#!/bin/bash

set -euo pipefail


# Kompilieren Sie C-Dateien
if gcc Temperatur.c -o Temperatur -lm && gcc steurung.c -o steurung && gcc main.c -o main; then
    echo "Compilation réussie."
else
    echo "Kompilierungsfehler. Stoppen des Skripts."
    exit 1
fi

#Sensorvorgang starten
./Temperatur &

# Warten Sie kurz, damit der Sensor starten kann
sleep 5

# Starten Sie den Kontrollvorgang
./steurung &

# Warten Sie kurz, damit der Regelvorgang beginnen kann
sleep 5

# Hauptprozess starten
./main &

# Überwachen Sie laufende Prozesse
while :
do
    # Überprüfen Sie, ob noch Prozesse ausgeführt werden
    if ! pgrep -x "tempo" || ! pgrep -x "steurung" || ! pgrep -x "main"; then
        echo "Un des processus s'est arrêté."
        break
    fi

    #Überprüfen Sie, ob das Skript gestoppt wurde (z. B. STRG+C)
    if [[ ! -e /proc/$$/status ]]; then
        echo "Arrêt du script."
        # Beenden Sie alle untergeordneten Prozesse
        pkill -P $$
        break
    fi

    # Warten Sie, bevor Sie die Überprüfung erneut durchführen
    sleep 1
done

# Warten Sie, bis die untergeordneten Prozesse abgeschlossen sind
wait

# Bereinigen Sie ggf. temporäre Dateien
# ...

echo "Script terminé."

