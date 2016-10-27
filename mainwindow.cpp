#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPixmap>
#include <QUrl>
#include <QDesktopServices>

#include "extractthread.h"

#include "framedialog.h"

MainWindow::MainWindow(QWidget *parent, QString filename) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frameList->setIconSize(QSize(256,256));

    video = new QVideoFile();
    fd = new FrameDialog();
    fth = new FramesThread();
    eth = new ExtractThread();

    connect(eth, SIGNAL(getOnePreviewImage(QImage,int)), this, SLOT(addPreviewImageToList(QImage,int)));
    connect(eth, SIGNAL(finished()),this,SLOT(extractFinished()));

    connect(ui->frameList,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(showFrameDialog(QListWidgetItem*)));

    connect(fth,SIGNAL(getAllFrames(QList<QImage>)),fd,SLOT(addAllFramesToList(QList<QImage>)));
    //connect(fth,SIGNAL(finished()),fd,SLOT(gotframe()));

    ui->filepath->setText(filename);
    QFileInfo *fi = new QFileInfo(filename);
    ui->savepath->setText(fi->filePath().remove("."+fi->suffix()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"), QDir::homePath(),"");
    ui->filepath->setText(fileName);
    QFileInfo *fi = new QFileInfo(fileName);
    ui->savepath->setText(fi->filePath().remove("."+fi->suffix()));


}

void MainWindow::on_saveBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->savepath->setText(dir);
}

void MainWindow::on_startBtn_clicked()
{
    ui->frameList->clear();

    QString filepath = ui->filepath->text();
    QString savepath = ui->savepath->text();

    int step = ui->timestep->value();
    video->openFile(filepath);
    video->setSavePath(savepath);
    video->setTimeStep(step);

    if(filepath == "" || savepath == "") {
        QMessageBox::warning(this,"Please choose video file","Please choose video file!",QMessageBox::Ok);
        return;
    }

    ui->progressBar->setMaximum(video->getLength()/step+1);

    QDir d;
    d.mkdir(savepath);

    enableUi(false);

    QUrl url(QDir(savepath).absolutePath());
    QDesktopServices::openUrl(QUrl("file:///"+url.toString()));
/*#ifdef Q_WS_WIN
    QProcess::startDetached("explorer.exe",QStringList(savepath.replace("/","\\")));
#else
    QProcess::startDetached("pcmanfm",QStringList(savepath));
#endif*/

    eth->setVideo(this->video);
    eth->start();
}


void MainWindow::addPreviewImageToList(QImage img, int sec)
{
    //update progress bar
    int val = ui->progressBar->value()+1;
    ui->progressBar->setValue(val);

    if(img.isNull())
       return;
    QIcon icon = QIcon(QPixmap::fromImage(img));
    QListWidgetItem *item = new QListWidgetItem(ui->frameList);
    item->setIcon(icon);

    item->setText(QString("%1").arg(sec));

    ui->frameList->addItem(item);

}
void MainWindow::extractFinished()
{
    video->close();
    enableUi(true);
}

void MainWindow::enableUi(bool isenable)
{
    ui->filepath->setEnabled(isenable);
    ui->savepath->setEnabled(isenable);

    ui->loadBtn->setEnabled(isenable);
    ui->saveBtn->setEnabled(isenable);
    ui->startBtn->setEnabled(isenable);
    ui->timestep->setEnabled(isenable);
    if(isenable){
        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(100);
    }

}

void MainWindow::showFrameDialog(QListWidgetItem * item)
{
    QVideoFile *video = new QVideoFile();
    QString filepath = ui->filepath->text();
    QString savepath = ui->savepath->text();

    int step = ui->timestep->value();
    video->openFile(filepath);
    video->setSavePath(savepath);
    video->setTimeStep(step);
    fth->setVideo(video);
    fth->setStartTime(item->text().toInt());
    fth->start();

    fd->setVideo(video);
    fd->setWindowTitle(
                " All frames of Seconds " + item->text()+
                " to " + QString("%1").arg(item->text().toInt()+video->getTimeStep()));
    fd->setModal(true);
    fd->startAnimation();
    fd->showMaximized();
}

