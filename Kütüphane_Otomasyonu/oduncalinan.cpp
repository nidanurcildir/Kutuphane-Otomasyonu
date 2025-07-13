#include "oduncalinan.h"
#include "ui_oduncalinan.h"

OduncAlinan::OduncAlinan(QSqlDatabase &db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OduncAlinan)
    , model(new QSqlQueryModel(this))
    , model_2(new QSqlQueryModel(this))
    , model_3(new QSqlQueryModel(this))
    , sorgu(db)
    , sorgu_2(db)
    , sorgu_3(db),
    sorgu_4(db)
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    try {
        listeleUye();
        listeleKitap();
        listeleOduncAlinan();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }

    connect(ui->tableView, &QTableView::clicked, this, &OduncAlinan::on_tableView_clicked);
    connect(ui->tableView_2, &QTableView::clicked, this, &OduncAlinan::on_tableView_2_clicked);
}

OduncAlinan::~OduncAlinan()
{
    delete ui;
}

void OduncAlinan::listeleUye()
{
    try {
        sorgu.prepare("SELECT * FROM uye");
        qDebug() << "Üye tablosu sorgusu hazırlanıyor...";

        if (!sorgu.exec()) {
            QString errorMsg = "Üye tablosu sorgusu çalıştırılırken bir hata oluştu: " + sorgu.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        qDebug() << "Üye tablosu sorgusu başarıyla çalıştırıldı.";
        model->setQuery(std::move(sorgu));
        ui->tableView->setModel(model);
        QMessageBox::information(this, "Bilgi", "Üye tablosu başarıyla listelendi.");

    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        throw;
    }
}

void OduncAlinan::listeleKitap()
{
    try {
        sorgu_2.prepare("SELECT * FROM kitap");
        qDebug() << "Kitap tablosu sorgusu hazırlanıyor...";

        if (!sorgu_2.exec()) {
            QString errorMsg = "Kitap tablosu sorgusu çalıştırılırken bir hata oluştu: " + sorgu_2.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        qDebug() << "Kitap tablosu sorgusu başarıyla çalıştırıldı.";
        model_2->setQuery(std::move(sorgu_2));
        ui->tableView_2->setModel(model_2);
        QMessageBox::information(this, "Bilgi", "Kitap tablosu başarıyla listelendi.");

    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        throw;
    }
}

void OduncAlinan::listeleOduncAlinan()
{
    try {
        sorgu_3.prepare("SELECT * FROM odunc_alinan");
        qDebug() << "Odunc_alinan tablosu sorgusu hazırlanıyor...";

        if (!sorgu_3.exec()) {
            QString errorMsg = "Odunc_alinan tablosu sorgusu çalıştırılırken bir hata oluştu: " + sorgu_3.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        qDebug() << "Odunc_alinan tablosu sorgusu başarıyla çalıştırıldı.";
        model_3->setQuery(sorgu_3);
        ui->tableView_3->setModel(model_3);
        QMessageBox::information(this, "Bilgi", "Odunc_alinan tablosu başarıyla listelendi.");

    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        throw;
    }
}

void OduncAlinan::on_tableView_clicked(const QModelIndex &index)
{
    try {
        QString uyeId = model->index(index.row(), 0).data().toString();
        qDebug() << "Seçilen üyenin ID'si: " << uyeId;
        ui->lineEdit->setText(uyeId);
        QMessageBox::information(this, "Bilgi", "Üye ID'si lineEdit'e yazıldı.");

    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }
}

void OduncAlinan::on_tableView_2_clicked(const QModelIndex &index)
{
    try {
        QString kitapId = model_2->index(index.row(), 0).data().toString();
        qDebug() << "Seçilen kitabın ID'si: " << kitapId;
        ui->lineEdit_2->setText(kitapId);
        QMessageBox::information(this, "Bilgi", "Kitap ID'si lineEdit_2'ye yazıldı.");

    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }
}

void OduncAlinan::on_pushButton_clicked()
{
    try {
        QString uyeNo = ui->lineEdit->text();
        QString kitapNo = ui->lineEdit_2->text();
        QDate oduncAlmaTarihi = ui->dateEdit->date();

        qDebug() << "Alınan Üye No: " << uyeNo;
        qDebug() << "Alınan Kitap No: " << kitapNo;
        qDebug() << "Alınan Ödünç Alma Tarihi: " << oduncAlmaTarihi.toString();

        if (uyeNo.isEmpty() || kitapNo.isEmpty()) {
            throw std::runtime_error("Üye No veya Kitap No boş olamaz.");
        }

        // Aynı kitabın zaten alınıp alınmadığını kontrol et
        QSqlQuery sorgu_4;
        sorgu_4.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE UyeNo = ? AND KitapNo = ?");
        sorgu_4.addBindValue(uyeNo);
        sorgu_4.addBindValue(kitapNo);

        if (!sorgu_4.exec()) {
            QString errorMsg = "Mevcut ödünç kayıtları kontrol edilirken bir hata oluştu: " + sorgu_4.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        sorgu_4.next();
        if (sorgu_4.value(0).toInt() > 0) {
            throw std::runtime_error("Bu kullanıcı zaten bu kitabı ödünç almış.");
        }

        // Kitap stoğunu kontrol et
        QSqlQuery sorgu_5;
        sorgu_5.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE KitapNo = ?");
        sorgu_5.addBindValue(kitapNo);

        if (!sorgu_5.exec()) {
            QString errorMsg = "Kitap stoğu kontrol edilirken bir hata oluştu: " + sorgu_5.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        sorgu_5.next();
        int oduncAlinanSayisi = sorgu_5.value(0).toInt();

        // Kitap stoğu ile ödünç alınan sayısını kontrol et
        QSqlQuery sorgu_6;
        sorgu_6.prepare("SELECT KitapSayisi FROM kitap WHERE KitapNo = ?");
        sorgu_6.addBindValue(kitapNo);

        if (!sorgu_6.exec()) {
            QString errorMsg = "Kitap stoğu kontrol edilirken bir hata oluştu: " + sorgu_6.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        sorgu_6.next();
        int kitapStogu = sorgu_6.value(0).toInt();

        if (oduncAlinanSayisi >= kitapStogu) {
            throw std::runtime_error("Bu kitap şu anda mevcut değil.");
        }

        // Ödünç alma kaydını ekle
        QSqlQuery sorgu_7;
        sorgu_7.prepare("INSERT INTO odunc_alinan (UyeNo, KitapNo, OduncAlmaTarihi) VALUES (?, ?, ?)");
        sorgu_7.addBindValue(uyeNo);
        sorgu_7.addBindValue(kitapNo);
        sorgu_7.addBindValue(oduncAlmaTarihi);

        if (!sorgu_7.exec()) {
            QString errorMsg = "Ödünç alma kaydı eklenirken bir hata oluştu: " + sorgu_7.lastError().text();
            qDebug() << errorMsg;
            throw std::runtime_error(errorMsg.toStdString());
        }

        QMessageBox::information(this, "Başarılı", "Ödünç alma kaydı başarıyla eklendi.");
        qDebug() << "Ödünç alma kaydı başarıyla eklendi.";

        listeleOduncAlinan(); // Güncellenen listeyi yeniden getir

    } catch (const std::exception &e) {
        qDebug() << "Hata oluştu: " << e.what();
        QMessageBox::critical(this, "Hata", QString::fromStdString(e.what()));
    }
}


