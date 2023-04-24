#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <stdio.h>
#include <stdlib.h>
#include <QMessageBox>
#include <string>
#include <QDebug>
#include <QProcess>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox_noise->setSingleStep(0.1);
    ui->doubleSpinBox_noise->setValue(0.4);
}

MainWindow::~MainWindow()
{
    delete ui;
}



//so-vits-svc
//sys.argv[1]是输入音频路径，sys.argv[2]是输出路径，sys.argv[3]是slice_db，sys.argv[4]为音高调整



//选择inference_main路径
void MainWindow::on_moegoepathbutton_2_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(NULL,"选择inference_main.py所在的路径","/");
    ui->moegoepath_2->setText(fileName);
}


//选择保存路径
void MainWindow::on_savepathbutton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(NULL,"保存","/","*.wav");
    ui->savepathline->setText(fileName);
}


void MainWindow::on_vitsconfirm_2_clicked()
{

    QString qssavepath=ui->savepathline->text();
    QString qstext=ui->inputpath->text();
    QString qspath=ui->moegoepath_2->text();
    QString qspitch=ui->spinBox_pitch->text();
    QString qsnoise=ui->doubleSpinBox_noise->text();
    if(qspath.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), tr("没有inference_main.py路径"));
        return;
    }
    if(qstext.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), tr("无待转换音频"));
        return;
    }
    else if(qssavepath.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), tr("未指定保存路径"));
        return;
    }
    //合并最终命令
    char *savepath,*text,*path,*pitch,*noise;
    QByteArray buff = qssavepath.toLatin1();
    savepath=buff.data();
    buff=qstext.toLatin1();
    text=buff.data();
    buff=qspath.toLatin1();
    path=buff.data();
    buff=qspitch.toLatin1();
    pitch=buff.data();
    buff=qsnoise.toLatin1();
    noise=buff.data();
    char command[500];
    sprintf(command,"python3 %s/inference_main.py -m \"%s/logs/44k/G.pth\" -c \"%s/configs/config.json\" -n %s -o %s -s \"gbn\" -ns %s",path,path,path,text,savepath,noise);
    if(ui->checkBox_autopitch->isChecked())
    {
        sprintf(command,"%s -a",command);
    }
    else
    {
        sprintf(command,"%s -t %s",command,pitch);
    }
    QFile file("inference.sh");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this, tr("Alert"), tr("Failed to save to file."));
        qDebug() << "file error!";
        return;
    }
    QTextStream out(&file);
    out <<"#!/bin/bash\nconda init\nconda activate so-vits-4\n"<< command;
    qDebug()<<"write student.json sucessfully: "<<command;
    file.close();
    //qDebug("%s",command);
    //popen("virtualenv","r");
    //fp=popen(command,"r");
    //fgets(output,10,fp);

    //fscanf(fp,"%[^EOF]",output);
    //qDebug("%s",output);

    //ui->output->appendPlainText(output);

    //system("conda activate MoeGoe");
    //system(command);
    system("chmod 777 inference.sh");
    QProcess* executeProcess = new QProcess();
    executeProcess->start("/bin/bash", QStringList()<<"inference.sh");
    while(executeProcess->canReadLine())
    {
        ui->output->appendPlainText(executeProcess->readLine());
    }
}




void MainWindow::on_inputpathbutton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(NULL,"选择输入音频","/","*.wav");
    ui->inputpath->setText(fileName);
}


void MainWindow::on_checkBox_autopitch_stateChanged(int arg1)
{
    if(arg1==2)
    {
        ui->spinBox_pitch->setDisabled(true);
    }
    else
        ui->spinBox_pitch->setDisabled(false);
}

