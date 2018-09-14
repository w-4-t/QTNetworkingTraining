#include <myserver.h>

myserver::myserver(){}

myserver::~myserver(){}

void myserver::startServer(){
    if (this->listen(QHostAddress::Any, 5555)){
        qDebug()<<"Listening...";
    } else {
        qDebug()<<"Error: not listening";
    }
}


void myserver::incomingConnection(int socketDescriptor){
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

    qDebug() << socketDescriptor << " client connected";

    //send json array : {"type":"connect", "status":"OK"}
    socket->write("{\"type\":\"connect\", \"status\":\"OK\"}");

    qDebug() << "Connection status sent - OK";

}


void myserver::sockReady(){
    Data = socket->readAll();
    qDebug() << "Data: " << Data;


    doc = QJsonDocument::fromJson(Data, &docError);

    if (docError.error != QJsonParseError::NoError) {
        qDebug()<< "Error code: " << docError.errorString();
        return;
    }


    //Expect: {"type":"select", "params":"workers"}
    if (doc.object().value("type").toString() == "select" && doc.object().value("params").toString() == "workers"){
        QFile file;
        file.setFileName("C:\\Sanduku\\QTNetworkingTraining\\jsonserver\\workers.json");
        if (file.open(QIODevice::ReadOnly | QFile::Text)){
            QByteArray fromFile = file.readAll();
            // {"type":"resultSelect", "result":...}
            QByteArray returnString = "{\"type\":\"selectResult\", \"result\":"+ fromFile +"}";

            socket->write(returnString);
            socket->waitForBytesWritten(500);
        }

        file.close();
    }




}

void myserver::sockDisc(){
    qDebug() << "Disconnected";
    socket->deleteLater();
}
