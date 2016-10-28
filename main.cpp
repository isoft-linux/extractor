#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif
#include "mainwindow.h"
#include <QTextCodec>
#include <QtPlugin>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QList<QImage> >("QList<QImage>");
    QApplication a(argc, argv);
    a.addLibraryPath(QCoreApplication::applicationDirPath());
#ifdef Q_WS_WIN
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB18030"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB18030"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB18030"));
#endif
    QTranslator trans;
    trans.load(":/trans/extractor.qm");
    a.installTranslator(&trans);

    MainWindow w(0,argv[1]);
    //w.show();
    w.showMaximized();
    return a.exec();
}
