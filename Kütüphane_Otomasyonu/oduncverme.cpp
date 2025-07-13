#include "oduncverme.h"
#include "ui_oduncverme.h"

OduncVerme::OduncVerme(QSqlDatabase &db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OduncVerme)
    , model(new QSqlQueryModel(this))
    , model_2(new QSqlQueryModel(this))
    , model_3(new QSqlQueryModel(this))
    , sorgu(db)
    , sorgu_2(db)
    , sorgu_3(db)
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    listele();
    listele_2();
}

OduncVerme::~OduncVerme()
{
    delete ui;
}

void OduncVerme::listele()
{
    try {
        sorgu.prepare("SELECT * FROM odunc_alinan");
        if (!sorgu.exec()) {
            QString errorMsg = "Odunc_alinan tablosu sorgusu çalıştırılırken bir hata oluştu: " + sorgu.lastError().text();
            throw std::runtime_error(errorMsg.toStdString());
        }
        model->setQuery(sorgu);
        ui->tableView->setModel(model);
        QMessageBox::information(this, "Bilgi", "Odunc_alinan tablosu başarıyla listelendi.");
    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        throw;
    }
}

void OduncVerme::listele_2()
{
    try {
        sorgu_2.prepare("SELECT * FROM odunc_teslim_edilen");
        if (!sorgu_2.exec()) {
            QString errorMsg = "Odunc_teslim_edilen tablosu sorgusu çalıştırılırken bir hata oluştu: " + sorgu_2.lastError().text();
            throw std::runtime_error(errorMsg.toStdString());
        }
        model_2->setQuery(sorgu_2);
        ui->tableView_2->setModel(model_2);
        QMessageBox::information(this, "Bilgi", "Odunc_teslim_edilen tablosu başarıyla listelendi.");
    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        throw;
    }
}

void OduncVerme::on_pushButton_clicked()
{
    try {
        QString uyeNo = ui->lineEdit->text();
        QString kitapNo = ui->lineEdit_2->text();
        QDate almaTarihi = selectedAlmaTarihi;
        QDate vermeTarihi = ui->dateEdit->date();

        if (uyeNo.isEmpty() || kitapNo.isEmpty()) {
            throw std::runtime_error("Üye No veya Kitap No boş olamaz.");
        }

        // Gün farkını hesapla
        int gunFarki = almaTarihi.daysTo(vermeTarihi);

        // Günlük borcu 2 birim olarak hesapla
        int borc = gunFarki * 2;

        if (gunFarki < 15) {
            borc = 0;
        }

        sorgu_3.prepare("INSERT INTO odunc_teslim_edilen (UyeNo, KitapNo, AlmaTarihi, VermeTarihi, Borc) VALUES (?, ?, ?, ?, ?)");
        sorgu_3.addBindValue(uyeNo);
        sorgu_3.addBindValue(kitapNo);
        sorgu_3.addBindValue(almaTarihi);
        sorgu_3.addBindValue(vermeTarihi);
        sorgu_3.addBindValue(borc);

        if (!sorgu_3.exec()) {
            QString errorMsg = "Odunc_teslim_edilen tablosuna veri eklenirken bir hata oluştu: " + sorgu_3.lastError().text();
            throw std::runtime_error(errorMsg.toStdString());
        }

        QSqlQuery silSorgu;
        silSorgu.prepare("DELETE FROM odunc_alinan WHERE UyeNo = ? AND KitapNo = ?");
        silSorgu.addBindValue(kitapNo);
        silSorgu.addBindValue(uyeNo);

        if (!silSorgu.exec()) {
            QString errorMsg = "Odunc_alinan tablosundan veri silinirken bir hata oluştu: " + silSorgu.lastError().text();
            throw std::runtime_error(errorMsg.toStdString());
        }

        QMessageBox::information(this, "Bilgi", "Veri başarıyla eklendi ve odunc_alinan tablosundan silindi.");

        listele();
        listele_2();
    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }
}

void OduncVerme::on_tableView_clicked(const QModelIndex &index)
{
    try {
        QString uyeNo = model->index(index.row(), 0).data().toString();
        QString kitapNo = model->index(index.row(), 1).data().toString();
        selectedAlmaTarihi = model->index(index.row(), 2).data().toDate();
        ui->lineEdit->setText(uyeNo);
        ui->lineEdit_2->setText(kitapNo);
    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }
}
