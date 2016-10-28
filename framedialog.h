#ifndef FRAMEDIALOG_H
#define FRAMEDIALOG_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidgetItem>
#else
#include <QDialog>
#include <QListWidgetItem>
#endif
#include <QList>
#include <QImage>

#include "qvideofile.h"
#include "QProgressIndicator.h"

namespace Ui {
class FrameDialog;
}

class FrameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FrameDialog(QWidget *parent = 0);
    ~FrameDialog();
    void startAnimation();
public slots:
    void addAllFramesToList(QList<QImage> list);
    void previewPicture();
    void setVideo(QVideoFile *f);
    void savePicture();
private:
    QList<QImage> imglist;
    Ui::FrameDialog *ui;
    QString savepath;
    QVideoFile *video;
    QProgressIndicator* pi;
};

#endif // FRAMEDIALOG_H
