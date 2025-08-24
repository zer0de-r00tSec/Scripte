Scripte Sammlung für dies und das, nix wildes.

hF



##### ffmpeg_installer

### Verwendung

1. Speichere den obigen Code als `ffmpeg_installer.sh`.
2. Mach die Datei ausführbar:  
   ```bash
   chmod +x ffmpeg_installer.sh
   ```
3. Führe sie aus (Root‑Rechte werden automatisch mit `sudo` abgefragt):  
   ```bash
   ./ffmpeg_installer.sh
   ```

Der Script installiert alle benötigten Pakete, baut die Bibliotheken in einem eigenen Installationsverzeichnis (`$HOME/ffmpeg_build`) und legt die ausführbaren Dateien unter `/usr/local/bin` ab – sodass sie systemweit verfügbar sind.  

Falls du später etwas entfernen möchtest, kannst du einfach das Verzeichnis `~/ffmpeg_build` löschen oder die installierten Pakete mit `apt remove --purge <paket>` zurücknehmen.
