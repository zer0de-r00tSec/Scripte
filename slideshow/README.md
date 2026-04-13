# G19Daemon – Slideshow Plugin

Dieses Plugin erweitert den G19Daemon um eine automatische Bilder-Slideshow für das Display der Logitech G19 Tastatur.

## Installation

Kopiere den gesamten Plugin-Ordner in das Plugin-Verzeichnis deiner G19Daemon-Source. Standardmäßig ist das `/home/$USER/g19daemon/src/plugins`.

Wechsle anschließend in das Hauptverzeichnis des Daemons, erstelle einen Build-Ordner und kompiliere das Projekt neu:

```bash
cd /home/$USER/g19daemon/
mkdir -p build && cd build
cmake ..
make
sudo make install
```

## Konfiguration

Öffne deine G19Daemon-Konfigurationsdatei, die sich unter `/home/$USER/.config/G19Daemon` befindet.

Füge dort die folgenden Zeilen ein. Die genaue Position in der Datei ist egal, du kannst sie einfach oben hinzufügen:

```ini
Slideshow-enabled=true
Slideshow.folder=/home/$USER/Bilder
Slideshow.interval=300000
Slideshow.showTitle=false
```

Passe den Pfad bei `Slideshow.folder` einfach an deinen gewünschten Bilderordner an. 

## Starten

Sobald alles installiert und eingetragen ist, musst du den Daemon nur noch neu starten. Die Slideshow beginnt danach automatisch auf dem Display deiner Tastatur!
