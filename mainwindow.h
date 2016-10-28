#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QListWidgetItem>
#else
#include <QMainWindow>
#include <QListWidgetItem>
#endif
#include "extractthread.h"
#include "framedialog.h"
#include "framesthread.h"
#include "qvideofile.h"

#include <QImage>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0,QString filename="");
    ~MainWindow();
    
private slots:
    void on_loadBtn_clicked();

    void on_saveBtn_clicked();

    void on_startBtn_clicked();

    void enableUi(bool);

    void extractFinished();

    void addPreviewImageToList(QImage,int);

    void showFrameDialog(QListWidgetItem*);

private:
    Ui::MainWindow *ui;
    ExtractThread *eth;
    FramesThread *fth;
    QVideoFile *video;
    FrameDialog *fd;
};

#endif // MAINWINDOW_H
