#include "uyeler.h"
#include "ui_uyeler.h"
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

uyeler::uyeler(QSqlDatabase &db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uyeler),
    db(db)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(this);
    sorgu = new QSqlQuery(db);
    ui->lineEdit->setEnabled(false);

    // Veritabanı bağlantısının açık olup olmadığını kontrol et
    if (!db.isOpen()) {
        if (!db.open()) {
            QString errorMessage = "Veritabanı bağlantısı açılamadı: " + db.lastError().text();
            QMessageBox::critical(this, "Veritabanı Hatası", errorMessage);
            qWarning() << errorMessage;
            return;
        }
    }

    listele();  // Pencere oluşturulurken listele fonksiyonunu çağır
}

uyeler::~uyeler()
{
    delete ui;
    delete sorgu;  // QSqlQuery nesnesini serbest bırak
}

void uyeler::listele()
{
    // Sorguyu hazırlama ve çalıştırma
    sorgu->prepare("SELECT * FROM uye");

    // Sorguyu çalıştırma
    if (!sorgu->exec()) {
        QString errorMessage = "Sorgu hatası: " + sorgu->lastError().text();
        QMessageBox::critical(this, "Veritabanı Hatası", errorMessage);
        qWarning() << errorMessage;
        return;
    }

    // Modeli sorguyla doldurma
    model->setQuery(*sorgu);

    // Başlıkları ayarla
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("İsim"));
    model->setHeaderData(2, Qt::Horizontal, tr("Soyisim"));

    // Modeli QTableView'e ata
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();  // Sütunları içeriğe göre boyutlandır
}

void uyeler::on_pushButton_4_clicked()
{
    this->close();
}


void uyeler::on_tableView_clicked(const QModelIndex &index)
{
    QString id = model->index(index.row(), 0).data().toString();
    QString isim = model->index(index.row(), 1).data().toString();
    QString soyisim = model->index(index.row(), 2).data().toString();

    ui->lineEdit->setText(id);
    ui->lineEdit_2->setText(isim);
    ui->lineEdit_3->setText(soyisim);
}


void uyeler::on_pushButton_clicked()
{
    // Kullanıcı tarafından girilen değerleri al
    QString isim = ui->lineEdit_2->text().trimmed();  // Boşlukları temizle
    QString soyisim = ui->lineEdit_3->text().trimmed();  // Boşlukları temizle

    // Değerlerin boş olup olmadığını kontrol et
    if (isim.isEmpty() || soyisim.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "İsim ve soyisim alanları boş olamaz.");
        return;
    }

    // Sorguyu hazırla ve veritabanına kayıt ekle
    QSqlQuery sorgu_2(db);
    sorgu_2.prepare("INSERT INTO uye (UyeAd, UyeSoyad) VALUES (?, ?)");
    sorgu_2.addBindValue(isim);
    sorgu_2.addBindValue(soyisim);

    // Sorguyu çalıştır ve sonucu kontrol et
    if (sorgu_2.exec()) {
        QMessageBox::information(this, "Başarılı", "Yeni kayıt başarıyla eklendi.");
        listele();  // Tabloyu güncelle
    } else {
        QMessageBox::critical(this, "Hata", "Kayıt eklenirken bir hata oluştu: " + sorgu_2.lastError().text());
    }
}



void uyeler::on_pushButton_2_clicked()
{
    // Kullanıcı tarafından girilen değerleri al
    QString id = ui->lineEdit->text();
    QString isim = ui->lineEdit_2->text().trimmed();  // Boşlukları temizle
    QString soyisim = ui->lineEdit_3->text().trimmed();  // Boşlukları temizle

    // Değerlerin boş olup olmadığını kontrol et
    if (isim.isEmpty() || soyisim.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "İsim ve soyisim alanları boş olamaz.");
        return;
    }

    // Sorguyu hazırla ve veritabanında kaydı güncelle

    sorgu_3.prepare("UPDATE uye SET UyeAd = ?, UyeSoyad = ? WHERE UyeNo = ?");
    sorgu_3.addBindValue(isim);
    sorgu_3.addBindValue(soyisim);
    sorgu_3.addBindValue(id);

    // Sorguyu çalıştır ve sonucu kontrol et
    if (sorgu_3.exec()) {
        QMessageBox::information(this, "Başarılı", "Kayıt başarıyla güncellendi.");
        listele();  // Tabloyu güncelle
    } else {
        QMessageBox::critical(this, "Hata", "Kayıt güncellenirken bir hata oluştu: " + sorgu_3.lastError().text());
    }
}




void uyeler::on_pushButton_3_clicked()
{
    QString id = ui->lineEdit->text();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "ID alanı boş olamaz.");
        return;
    }

    // Odunc_alinan tablosunda UyeNo'ya sahip bir kayıt var mı kontrol et
    QSqlQuery sorgu_kontrol(db);
    sorgu_kontrol.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE UyeNo = ?");
    sorgu_kontrol.addBindValue(id);
    if (sorgu_kontrol.exec() && sorgu_kontrol.next()) {
        int kayit_sayisi = sorgu_kontrol.value(0).toInt();
        if (kayit_sayisi > 0) {
            QMessageBox::warning(this, "Uyarı", "Bu üye üzerinde " + QString::number(kayit_sayisi) + " adet ödünç alınmış kitap bulunmaktadır. Kaydı silemezsiniz.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Hata", "Veritabanı hatası: " + sorgu_kontrol.lastError().text());
        return;
    }

    // Üye üzerinde ödünç alınmış kitap yoksa silme işlemine devam et
    QSqlQuery sorgu_4(db);
    sorgu_4.prepare("DELETE FROM uye WHERE UyeNo = ?");
    sorgu_4.addBindValue(id);

    if (sorgu_4.exec()) {
        QMessageBox::information(this, "Başarılı", "Kayıt başarıyla silindi.");
        listele();
    } else {
        QMessageBox::critical(this, "Hata", "Kayıt silinirken bir hata oluştu: " + sorgu_4.lastError().text());
    }
}



