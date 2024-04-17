#include <QApplication>
#include <QWidget>
#include <QGraphicsBlurEffect>
#include <QVBoxLayout>
#include <QPushButton>
#include <mydecoder.h>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    myDecoder mydecoder("Titanic.mkv");

    mydecoder.show();
    qDebug() << "myDecoder show---";

    return app.exec();
}
