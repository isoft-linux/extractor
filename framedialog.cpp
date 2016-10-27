#include "framedialog.h"
#include "ui_framedialog.h"
#include <QDir>
#include <QDesktopWidget>
#include <QMessageBox>
FrameDialog::FrameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrameDialog)
{
    ui->setupUi(this);
    connect(ui->frameList,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(previewPicture()));
    connect(ui->frameList,SIGNAL(itemSelectionChanged()),this,SLOT(previewPicture()));
    connect(ui->okBtn,SIGNAL(clicked()),this,SLOT(savePicture()));

    QDesktopWidget *d = new QDesktopWidget();
    //this->setFixedSize(d->availableGeometry(-1).width(),d->availableGeometry().height());
    ui->image->setFixedWidth(d->width() - 320);

    ui->image->hide();
    pi = new QProgressIndicator();
    pi->setFixedWidth(d->width()-320);
    ui->verticalLayout->addWidget(pi,0,Qt::AlignCenter);
}

FrameDialog::~FrameDialog()
{
    delete ui;
}


void FrameDialog::startAnimation()
{
    ui->frameList->clear();
    ui->image->hide();
    pi->show();
    pi->startAnimation();
}

void FrameDialog::savePicture()
{
    QDir dir(this->savepath);

    //If frames is loading, just reture to avoid button clicked and crash.
    if(ui->frameList->count()<=0)
        return;
    QImage img = imglist.at(ui->frameList->currentItem()->text().toInt());
    QString filename = img.text("Description")+"_"+ui->frameList->currentItem()->text();

    QString savefile = dir.absolutePath()+QString("/%1-%2.png").arg(QFileInfo(savepath).fileName()).arg(filename);

    if(img.save(savefile))
        QMessageBox::warning(this,tr("saved ok"),tr("Copy successfully!"));
}

void FrameDialog::setVideo(QVideoFile *f)
{
    video = f;
    //video object will be closed and destroyed to avoid memleak.
    //save the 'savepath' here.
    savepath = video->getSavePath();
}

void FrameDialog::addAllFramesToList(QList<QImage> list)
{
    this->imglist = list;

    ui->image->setPixmap(QPixmap::fromImage(imglist.at(0)));

    this->pi->stopAnimation();
    this->pi->hide();
    this->ui->image->show();

    ui->frameList->clear();

    for (int i = 0; i < imglist.size(); ++i) {
        QIcon icon = QIcon(QPixmap::fromImage(imglist.at(i)));
        QListWidgetItem *item = new QListWidgetItem(ui->frameList);
        item->setIcon(icon);
        item->setText(QString("%1").arg(i));
        ui->frameList->setIconSize(QSize(256,256));
        ui->frameList->addItem(item);
    }

    ui->frameList->setCurrentItem(ui->frameList->itemAt(0,0));

    //Frame extract thread finished, the video object is useless.
    this->video->close();
    delete this->video;
}

void FrameDialog::previewPicture()
{
    QString f = ui->frameList->currentItem()->text();
    if(!imglist.at(QString(f).toInt()).isNull())
        ui->image->setPixmap(QPixmap::fromImage(imglist.at(QString(f).toInt())));
}


