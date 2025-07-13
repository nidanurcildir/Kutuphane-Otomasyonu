#include "kitaplar.h"
#include "ui_kitaplar.h"

kitaplar::kitaplar(QSqlDatabase &db, QDilaog *parent)
    : QDialog(parent)
    , ui(new Ui::kitaplar)
    , db(db)
{
    ui->setupUi(this); // Doğru parametre olan 'this' kullanılmalı
}

kitaplar::~kitaplar()
{
    delete ui;
}
