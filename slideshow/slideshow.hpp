#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "../../gscreen.hpp"
#include "../../plugininterface.hpp"
#include <QtCore>
#include <QtPlugin>
#include <QTimer>
#include <QStringList>
#include <QSettings>

class Slideshow : public QObject, public PluginInterface {
Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "slideshow")

public:
    Slideshow();
    ~Slideshow() override;
    void lKeys(int keys) override;
    void mKeys(int keys) override;
    QString getName() override;
    QImage getIcon() override;
    void setActive(bool active) override;
    bool isPopup() override;
    QObject *getQObject() override;

public slots:
    void nextImage();
    void prevImage();

signals:
    void doAction(gAction action, void *data);

private:
    void loadHistory();
    void saveToHistory(const QString &path);
    void clearHistory();
    void scanImages();
    void showImage(const QString &path);

    bool isActive;
    Gscreen *screen;
    QTimer *timer;
    QSettings *settings;

    QString imageFolder;
    QString historyFilePath;
    QStringList allImages;
    QStringList shownImages;
    int historyIndex;
    int switchTime;
    bool showTitle;
};

#endif // SLIDESHOW_H
