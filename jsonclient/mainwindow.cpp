#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1", 5555);
}

void MainWindow::sockDisc(){
    socket->deleteLater();
}

void MainWindow::sockReady(){
    if (socket->waitForConnected(500)){

        socket->waitForReadyRead(500);
        Data = socket->readAll();
        doc = QJsonDocument::fromJson(Data, &docError);

        if (docError.error != QJsonParseError::NoError) {
            qDebug()<< "Error code: " << docError.errorString();
            QMessageBox::warning(this, "Warning", "Message parsing error");
            return;
        }

        if (doc.object().value("type").toString() == "connect" && doc.object().value("status").toString() == "OK"){
            QMessageBox::information(this, "Info", "Connection status: OK");
        }else if(doc.object().value("type").toString() == "selectResult"){
                QStandardItemModel* model = new QStandardItemModel(nullptr);
                model->setHorizontalHeaderLabels(QStringList()<< "name");

                QJsonArray docArr = doc.object().value("result").toArray();
                for (int i = 0; i< docArr.count(); i++){
                    QStandardItem* column = new QStandardItem(docArr[i].toObject().value("name").toString());
                    model->appendRow(column);
                }

                ui->tableView->setModel(model);
        }else{
            QMessageBox::warning(this, "Warning", "Connection status: FAIL");
        }

        qDebug() << Data;
    }



}

void MainWindow::on_pushButton_2_clicked()
{
    if (!socket->isOpen()) QMessageBox::warning(this, "Warning", "No connection estabilished. Please connect to server first.");
    socket->write("{\"type\":\"select\", \"params\":\"workers\"}");
    socket->waitForBytesWritten(500);
}
