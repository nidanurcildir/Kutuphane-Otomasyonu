// kitaplarr.cpp
#include "kitaplarr.h"
#include "ui_kitaplarr.h"
#include <QSqlError>
#include <QMessageBox>

Kitaplarr::Kitaplarr(QSqlDatabase &db, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Kitaplarr),
    db(db),
    model(new QSqlQueryModel(this)),
    model_2(new QSqlQueryModel(this)),
    model_3(new QSqlQueryModel(this)),
    sorgu(db),
    sorgu_2(db),
    sorgu_3(db),
    sorgu_4(db),
    sorguGuncelle(db)
{
    ui->setupUi(this);
    listelekitap();
    listeleOduncAlinan();
    ui->lineEdit->setEnabled(false);
}

Kitaplarr::~Kitaplarr()
{
    delete ui;

}

void Kitaplarr::listelekitap()
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM kitap");

    if (!query.exec()) {
        QMessageBox::critical(this, "Hata", "Kitapları getirirken bir hata oluştu: " + query.lastError().text());
        return;
    }

    model->setQuery(std::move(query));
    ui->tableView->setModel(model);
}

void Kitaplarr::on_tableView_clicked(const QModelIndex &index)
{
    QString id = model->index(index.row(), 0).data().toString();
    QString KitapAd = model->index(index.row(), 1).data().toString();
    QString Stok = model->index(index.row(), 2).data().toString();

    ui->lineEdit->setText(id);
    ui->lineEdit_2->setText(KitapAd);
    ui->lineEdit_3->setText(Stok);

    listeleSecilenKitap();
}


void Kitaplarr::listeleOduncAlinan()
{
    sorgu.prepare("SELECT * FROM odunc_alinan");
    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Hata", "Ödünç alınan kitapları getirirken bir hata oluştu: " + sorgu.lastError().text());
        return;
    }

    model_2->setQuery(std::move(sorgu));
    ui->tableView_2->setModel(model_2);
}

void Kitaplarr::listeleSecilenKitap()
{
    QString Kitapno = ui->lineEdit->text();
    sorgu_2.prepare("SELECT * FROM odunc_alinan WHERE KitapNo = ?");
    sorgu_2.addBindValue(Kitapno);

    if (!sorgu_2.exec()) {
        QMessageBox::critical(this, "Hata", "Seçilen kitapla ilgili bilgileri getirirken bir hata oluştu: " + sorgu_2.lastError().text());
        return;
    }

    model_3->setQuery(sorgu_2);
    ui->tableView_3->setModel(model_3);
}


void Kitaplarr::on_pushButton_4_clicked()
{
    this->close();
}

void Kitaplarr::on_pushButton_clicked()
{
    //QString kitapNo = ui->lineEdit->text();
    QString kitapAd = ui->lineEdit_2->text();
    QString kitapSayisi= ui->lineEdit_3->text();

    if ( kitapAd.isEmpty() || kitapSayisi.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Lütfen tüm bilgileri doldurun.");
        return;
    }

    bool ok;
    int stokSayisi = kitapSayisi.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Uyarı", "Stok sayısı sadece sayısal bir değer olmalıdır.");
        return;
    }

    QSqlQuery sorguEkle(db);
    sorguEkle.prepare("INSERT INTO kitap ( KitapAd, KitapSayisi) VALUES ( ?, ?)");
   // sorguEkle.addBindValue(kitapNo);
    sorguEkle.addBindValue(kitapAd);
    sorguEkle.addBindValue(stokSayisi);

    if (!sorguEkle.exec()) {
        QMessageBox::critical(this, "Hata", "Kitap eklenirken bir hata oluştu: " + sorguEkle.lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Yeni kitap başarıyla eklendi.");
    listelekitap();
}

void Kitaplarr::on_pushButton_2_clicked()
{
    QString kitapNo = ui->lineEdit->text();
    QString kitapSayisi = ui->lineEdit_3->text();
    QString kitapAd = ui->lineEdit_2->text();

    // Girdilerin boş olup olmadığını kontrol edin
    if (kitapNo.isEmpty() || kitapAd.isEmpty() || kitapSayisi.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Lütfen tüm bilgileri doldurun.");
        return;
    }

    // KitapSayisi'nin sayısal olup olmadığını kontrol edin
    bool ok;
    int stokSayisi;
    try {
        stokSayisi = kitapSayisi.toInt(&ok);
        if (!ok) {
            throw std::invalid_argument("Stok sayısı sadece sayısal bir değer olmalıdır.");
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Uyarı", QString(e.what()));
        return;
    }

    // Güncelleme sorgusunu hazırlayın
    QSqlQuery sorguGuncelle(db);
    sorguGuncelle.prepare("UPDATE kitap SET KitapAd = :kitapAd, KitapSayisi = :stokSayisi WHERE KitapNo = :kitapNo");
    sorguGuncelle.bindValue(":kitapAd", kitapAd);
    sorguGuncelle.bindValue(":stokSayisi", stokSayisi);
    sorguGuncelle.bindValue(":kitapNo", kitapNo);

    try {
        // Sorguyu çalıştırın
        if (!sorguGuncelle.exec()) {
            throw std::runtime_error("Sorgu çalıştırılamadı.");
        }

        // Başarılı mesajını gösterin
        QMessageBox::information(this, "Başarılı", "Kitap başarıyla güncellendi.");

        // Kitap listesini yenileyin
        listelekitap();
    } catch (const QSqlError& error) {
        QMessageBox::critical(this, "Hata", "Veritabanı hatası: " + error.text());
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Hata", "Bir hata oluştu: " + QString(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Hata", "Beklenmeyen bir hata oluştu.");
    }
}






void Kitaplarr::on_pushButton_3_clicked()
{
    QString kitapNo = ui->lineEdit->text();
    qDebug() << "Kitap numarası alındı: " << kitapNo;

    // Kitap numarasının boş olup olmadığını kontrol etme
    if (kitapNo.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Lütfen bir kitap numarası girin.");
        return;
    }

    try {
        // Kitap numarasının veritabanında mevcut olup olmadığını kontrol etme
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM kitap WHERE KitapNo = ?");
        checkQuery.addBindValue(kitapNo);

        qDebug() << "Kitap numarası veritabanında kontrol ediliyor...";
        if (!checkQuery.exec()) {
            throw std::runtime_error("Kitap numarası kontrol edilirken bir hata oluştu: " + checkQuery.lastError().text().toStdString());
        }

        checkQuery.next();
        int count = checkQuery.value(0).toInt();

        if (count == 0) {
            QMessageBox::warning(this, "Uyarı", "Girilen kitap numarası bulunamadı.");
            return;
        }

        qDebug() << "Veritabanında kitap numarası bulundu.";

        // Odunc_alinan tablosunda belirli bir KitapNo'nun olup olmadığını kontrol etme
        QSqlQuery borrowCheckQuery(db);
        borrowCheckQuery.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE KitapNo = ?");
        borrowCheckQuery.addBindValue(kitapNo);

        qDebug() << "Odunc_alinan tablosunda kitap numarası kontrol ediliyor...";
        if (!borrowCheckQuery.exec()) {
            throw std::runtime_error("Odunc_alinan tablosunda kitap numarası kontrol edilirken bir hata oluştu: " + borrowCheckQuery.lastError().text().toStdString());
        }

        borrowCheckQuery.next();
        int borrowCount = borrowCheckQuery.value(0).toInt();

        if (borrowCount > 0) {
            QMessageBox::warning(this, "Uyarı", "Silmek istediğiniz kitap ödünç alınmış durumda. Lütfen önce ödünç iade edin.");
            return;
        }

        qDebug() << "Odunc_alinan tablosunda kitap numarası bulunamadı.";

        // Kitap numarası veritabanında bulunduğunda silme işlemini gerçekleştirme
        QSqlQuery deleteQuery(db);
        deleteQuery.prepare("DELETE FROM kitap WHERE KitapNo = ?");
        deleteQuery.addBindValue(kitapNo);

        qDebug() << "Kitap siliniyor...";
        if (!deleteQuery.exec()) {
            throw std::runtime_error("Kitap silinirken bir hata oluştu: " + deleteQuery.lastError().text().toStdString());
        }

        qDebug() << "Kitap başarıyla silindi. Kitap No: " << kitapNo;
        QMessageBox::information(this, "Başarılı", "Kitap başarıyla silindi.");
        listelekitap(); // Güncellenen kitapları tabloya yeniden ekle
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }
}


