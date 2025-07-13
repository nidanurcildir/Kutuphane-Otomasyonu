#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uyeler.h"
#include "kitaplarr.h"
#include "oduncalinan.h"
#include "oduncverme.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(QSqlDatabase::addDatabase("QSQLITE"))
{
    ui->setupUi(this);
    db.setDatabaseName("odev_veri_tabani.db");

    if (db.open())
    {
        ui->statusbar->showMessage("Veritabanina baglandi.");
    }
    else
    {
        ui->statusbar->showMessage("Veritabanina baglanmadi!");
        return;
    }

    setupRadioButtons();
}

MainWindow::~MainWindow()
{
    db.close();  // Veritabanını kapatma
    delete ui;
}

void MainWindow::setupRadioButtons()
{
    // Radyo butonların sinyallerini slot'a bağlama
    connect(ui->radioButton, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
    connect(ui->radioButton_2, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
    connect(ui->radioButton_3, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
    connect(ui->radioButton_4, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
}

void MainWindow::onRadioButtonClicked()
{
    QRadioButton *senderButton = qobject_cast<QRadioButton*>(sender());

    if(senderButton == ui->radioButton)
    {
        uyeler *uye = new uyeler(db, this);
        uye->show();
        ui->statusbar->showMessage("Üye işlemleri  seçildi.");
    }
    else if(senderButton == ui->radioButton_2)
    {
        Kitaplarr *kitap = new Kitaplarr(db, this);
        kitap->show();
        ui->statusbar->showMessage("Kitap işlemleri seçildi.");
    }
    else if(senderButton == ui->radioButton_3)
    {
        OduncAlinan *odunc =new OduncAlinan(db,this);
        odunc->show();
        ui->statusbar->showMessage("Ödünç alma işlemi seçildi.");
    }
    else if(senderButton == ui->radioButton_4)
    {
        OduncVerme *oduncverme=new OduncVerme(db,this);
        oduncverme->show();
        ui->statusbar->showMessage("Ödünç teslim işlemi  seçildi.");
    }
}

void MainWindow::on_pushButton_clicked()
{
    this->close();
}
