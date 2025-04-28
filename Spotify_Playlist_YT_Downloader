#!/bin/bash

# Konfiguration
TARGET_DIR="./Music"
QUALITY="320k"
CODEC="flac"
THREADS="8"
YOUTUBE_LOGIN="no"
CODEC_LIST=("mp3" "opus" "flac")

# Funktion: Zeige verfügbare Codecs
function show_available_codecs() {
    echo "Verfügbare Codecs:"
    for codec in "${CODEC_LIST[@]}"; do
        echo "- $codec"
    done
}

# Funktion: Überprüfen und installieren
function check_and_install() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "$1 ist nicht installiert."
        read -p "Möchtest du $1 installieren? (y/n) " choice
        if [[ "$choice" == "y" || "$choice" == "Y" ]]; then
            if [[ "$1" == "spotdl" ]]; then
                pip install spotdl
            elif [[ "$1" == "yt-dlp" ]]; then
                pip install yt-dlp
            elif [[ "$1" == "ffmpeg" ]]; then
                sudo apt install ffmpeg
            elif [[ "$1" == "ffprobe" ]]; then
                sudo apt install ffmpeg
            fi
            echo "$1 wird installiert..."
            sleep 2
            exec "$0" "$@" 
        else
            echo "$1 wird nicht installiert. Abbruch."
            exit 1
        fi
    }
}

# Überprüfen ob alle Tools vorhanden sind
check_and_install "spotdl"
check_and_install "yt-dlp"
check_and_install "ffmpeg"
check_and_install "ffprobe"

# Playlist-Link prüfen
if [ $# -lt 1 ]; then
    echo "Bitte gib einen Spotify-Playlist-Link an."
    echo "Beispiel: ./spotify_dl.sh https://open.spotify.com/playlist/playlist_id"
    exit 1
fi

# Playlist-Link holen
PLAYLIST_LINK="$1"
shift

# Optionen parsen
while getopts ":c:lt:" opt; do
    case ${opt} in
        c)
            if [[ " ${CODEC_LIST[@]} " =~ " $OPTARG " ]]; then
                CODEC=$OPTARG
            else
                echo "Ungültiger Codec: $OPTARG"
                show_available_codecs
                exit 1
            fi
            ;;
        l)
            YOUTUBE_LOGIN="yes"
            ;;
        t)
            if [[ "$OPTARG" =~ ^[0-9]+$ ]]; then
                THREADS="$OPTARG"
            else
                echo "Ungültige Thread-Zahl: $OPTARG (nur Zahlen erlaubt)"
                exit 1
            fi
            ;;
        \?)
            echo "Unbekannte Option: -$OPTARG"
            exit 1
            ;;
    esac
done

mkdir -p "$TARGET_DIR"

# Start Download
echo "Starte den Download der Playlist im $CODEC-Format mit $THREADS Threads..."
spotdl --threads "$THREADS" --output "$TARGET_DIR/{title}" --format "$CODEC" "$PLAYLIST_LINK"

# YouTube-Login falls nötig
if [ "$YOUTUBE_LOGIN" == "yes" ]; then
    echo "YouTube-Login aktiviert - Cookies speichern..."
    yt-dlp --cookies "cookies.txt" --no-check-certificate --format bestaudio[ext=$CODEC] --output "$TARGET_DIR/%(title)s.%(ext)s" --username "DEIN_EMAIL" --password "DEIN_PASSWORT" "$PLAYLIST_LINK"
fi

# Dateien sortieren
echo "Sortiere Dateien nach Interpret / Album / Tracknummer_Titel ..."

for file in "$TARGET_DIR"/*.$CODEC; do
    [ -e "$file" ] || continue

    # Tags sauber auslesen
    ARTIST=$(ffprobe -v error -show_entries format_tags=artist -of default=noprint_wrappers=1:nokey=1 "$file" | tr -cd '[:print:]\n' | sed 's/[/\\:*?"<>|]/-/g')
    ALBUM=$(ffprobe -v error -show_entries format_tags=album -of default=noprint_wrappers=1:nokey=1 "$file" | tr -cd '[:print:]\n' | sed 's/[/\\:*?"<>|]/-/g')
    TRACKNUMBER=$(ffprobe -v error -show_entries format_tags=track -of default=noprint_wrappers=1:nokey=1 "$file" | cut -d'/' -f1 | sed 's/[^0-9]*//g')
    TITLE=$(ffprobe -v error -show_entries format_tags=title -of default=noprint_wrappers=1:nokey=1 "$file" | tr -cd '[:print:]\n' | sed 's/[/\\:*?"<>|]/-/g')

    # Fehlende Tags abfangen
    ARTIST=${ARTIST:-"Unknown_Artist"}
    ALBUM=${ALBUM:-"Unknown_Album"}
    TRACKNUMBER=${TRACKNUMBER:-"00"}
    TITLE=${TITLE:-"Unknown_Title"}

    # Ordner erstellen
    DEST_DIR="$TARGET_DIR/$ARTIST/$ALBUM"
    mkdir -p "$DEST_DIR"

    # Tracknummer formatieren auf 2-stellig
    TRACKNUMBER=$(printf "%02d" "$TRACKNUMBER")

    # Zielpfad
    DEST_FILE="$DEST_DIR/${TRACKNUMBER}_${TITLE}.${CODEC}"

    echo "Verschiebe: $(basename "$file") → $DEST_FILE"
    mv "$file" "$DEST_FILE"
done

echo "Alles fertig! Sauber sortiert!"
