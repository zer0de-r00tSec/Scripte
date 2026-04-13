#include "slideshow.hpp"
#include "../../g19daemon.hpp"
#include <QImage>
#include <QPainter>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

Slideshow::Slideshow() {
    isActive = false;
    settings = new QSettings("G19Daemon", "G19Daemon");
    historyFilePath = "/tmp/g19_slideshow_history.txt";
    historyIndex = -1;
    switchTime = 5000;
    showTitle = false;

    QImage dummyIcon(32, 32, QImage::Format_ARGB32);
    dummyIcon.fill(Qt::blue);
    screen = new Gscreen(dummyIcon, tr("Slideshow"));

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Slideshow::nextImage);
}

Slideshow::~Slideshow() {
    delete settings;
    delete timer;
    delete screen;
}

QString Slideshow::getName() { return tr("Slideshow"); }
void Slideshow::mKeys(int keys) {}

void Slideshow::lKeys(int keys) {
    if (!isActive) return;
    if (keys & G19_KEY_LLEFT) prevImage();
    if (keys & G19_KEY_LRIGHT) nextImage();
}

void Slideshow::scanImages() {
    allImages.clear();
    QDirIterator it(imageFolder, QStringList() << "*.jpg" << "*.png" << "*.jpeg"  << "*.webm" , QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        allImages.append(it.next());
    }
}

void Slideshow::loadHistory() {
    shownImages.clear();
    QFile file(historyFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) shownImages.append(line);
        }
        file.close();
    }
    historyIndex = shownImages.size() - 1;
}

void Slideshow::saveToHistory(const QString &path) {
    shownImages.append(path);
    historyIndex = shownImages.size() - 1;
    QFile file(historyFilePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << path << "\n";
        file.close();
    }
}

void Slideshow::clearHistory() {
    shownImages.clear();
    historyIndex = -1;
    QFile file(historyFilePath);
    if (file.exists()) file.remove();
}

void Slideshow::setActive(bool active) {
    isActive = active;
    if (active) {
        // Lade alle drei Werte aus der Config
        imageFolder = settings->value("Slideshow.folder", QDir::homePath() + "/Bilder").toString();
        switchTime = settings->value("Slideshow.interval", 5000).toInt();
        showTitle = settings->value("Slideshow.showTitle", false).toBool();

        scanImages();
        loadHistory();
        timer->start(switchTime);
        nextImage();
    } else {
        timer->stop();
    }
}

void Slideshow::prevImage() {
    if (!isActive || allImages.isEmpty()) return;
    if (historyIndex > 0) {
        historyIndex--;
        showImage(shownImages[historyIndex]);
        timer->start(switchTime);
    }
}

void Slideshow::nextImage() {
    if (allImages.isEmpty() || !isActive) {
        int h = showTitle ? 206 : 240;
        QPainter *p = showTitle ? screen->begin() : screen->beginFullScreen();

        p->fillRect(0, 0, 320, h, Qt::black);
        p->setPen(Qt::white);
        p->drawText(0, 0, 320, h, Qt::AlignCenter, "Keine Bilder gefunden in:\n" + imageFolder);
        screen->end();
        emit doAction(showTitle ? displayScreen : displayFullScreen, screen);
        return;
    }

    timer->start(switchTime);

    if (historyIndex < shownImages.size() - 1 && historyIndex >= 0) {
        historyIndex++;
        showImage(shownImages[historyIndex]);
        return;
    }

    QStringList availableImages;
    for (int i = 0; i < allImages.size(); ++i) {
        if (!shownImages.contains(allImages[i])) availableImages.append(allImages[i]);
    }

    if (availableImages.isEmpty()) {
        clearHistory();
        availableImages = allImages;
    }

    int randomIndex = QRandomGenerator::global()->bounded(availableImages.size());
    QString selectedImage = availableImages[randomIndex];
    saveToHistory(selectedImage);
    showImage(selectedImage);
}

void Slideshow::showImage(const QString &path) {
    QImage img(path);
    if (!img.isNull()) {
        int w = 320;
        int h = showTitle ? 206 : 240;

        QImage scaledImg = img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Vollbild-Painter oder Bereichs-Painter aufrufen
        QPainter *p = showTitle ? screen->begin() : screen->beginFullScreen();
        p->fillRect(0, 0, w, h, Qt::black);

        int x = (w - scaledImg.width()) / 2;
        int y = (h - scaledImg.height()) / 2;
        p->drawImage(x, y, scaledImg);

        screen->end();

        // Befehl entsprechend senden
        emit doAction(showTitle ? displayScreen : displayFullScreen, screen);
    } else {
        QTimer::singleShot(0, this, &Slideshow::nextImage);
    }
}

bool Slideshow::isPopup() { return false; }
QObject *Slideshow::getQObject() { return this; }
QImage Slideshow::getIcon() {
    QImage dummyIcon(32, 32, QImage::Format_ARGB32);
    dummyIcon.fill(Qt::blue);
    return dummyIcon;
}
