// The MIT License (MIT)

// Copyright 2015 Siney/Pangweiwei siney@yeah.net
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogconnect.h"
#include <QDebug>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->plainTextEdit,SIGNAL(command(QString)),this,SLOT(onCommand(QString)));
	QObject::connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onConnect);
	QObject::connect(ui->actionClear, &QAction::triggered, this, &MainWindow::onClear);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnect()
{
    DialogConnect* dlg = new DialogConnect(this);
    int result = dlg->exec();
    if(result==QDialog::Accepted) {
        QString ip;
        quint16 port;
        if(dlg->getConectAddr(ip,port)) {
            doConnect(ip,port);
        }
        else {
            error("Invalid host address");
        }
    }
}

void MainWindow::onClear()
{
	ui->plainTextEdit->setPlainText("");
	result();
}

void MainWindow::onCommand(QString cmd)
{
    if(cmd=="$(Prompt)") {
        ui->plainTextEdit->setPrompt("Slua> ");
    }
    else if(cmd=="$(PromptDebug)") {
        ui->plainTextEdit->setPrompt("Debug> ");
    }
    else {
        if(socket && socket->isOpen()) {
            cmd+="\n";
            std::string str = cmd.toStdString();
            quint16 len = (quint16)str.size();
            socket->write((const char*)&len,sizeof(quint16));
            socket->write(str.data(),str.size());
			socket->flush();
        }
        else
            error("Host not conncted");
    }
}

void MainWindow::onRecv()
{
    while(true) {

        if(packageLen==0 && socket->bytesAvailable()>=sizeof(quint16))
        {
            socket->read((char*)&packageLen,sizeof(quint16));
        }

        if(packageLen>0 && socket->bytesAvailable()>=packageLen) {

            char* bytes = new char[packageLen+1];
            socket->read(bytes,packageLen);
            bytes[packageLen]='\0';
            QString str(bytes);
			delete[] bytes;

            if(str.startsWith("$(")) {
                onCommand(str.trimmed());
            }
            else
            {
                replace(bytes);
				result();
            }

            packageLen=0;
        }
        else
            break;
    }
}

void MainWindow::onConnected()
{
    qDebug() << "conncted";
}

void MainWindow::onDisconnected()
{
    qDebug() << "disconncted";
}

void MainWindow::onSocketError(QAbstractSocket::SocketError err)
{
	Q_UNUSED(err);
    QString error = socket->errorString();
    replace(error);
    result();
}

void MainWindow::error(QString err)
{
    result(err);
}

void MainWindow::doConnect(QString ip,quint16 port)
{
    socket = new QTcpSocket();

    QObject::connect(socket,&QTcpSocket::readyRead,this,&MainWindow::onRecv);
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),
		this,SLOT(onSocketError(QAbstractSocket::SocketError)));
    QObject::connect(socket,SIGNAL(connected()),this,SLOT(onConnected()));
    QObject::connect(socket,SIGNAL(disconnected()),this,SLOT(onDisconnected()));

    QHostAddress addr(ip);
    socket->connectToHost(addr,port);
}

void MainWindow::replace(QString str)
{
    ui->plainTextEdit->replace(str);
}

void MainWindow::result(QString str)
{
    ui->plainTextEdit->result(str);
}

void MainWindow::closeEvent(QCloseEvent *)
{
	if (socket)
		socket->disconnectFromHost();
}
