#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QPdfWriter>
#include <QPainter>
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include "commande.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ajouter_clicked();
    void on_afficher_clicked();
    void on_mettre_a_jour_clicked();
    void on_supprimer_clicked();
    void on_pdf_clicked();
    void on_lineEdit_recherche_cursorPositionChanged();
    void on_comboBox_3_activated(int index);
    void on_statistique_clicked();
    void on_ajouterArticle_clicked();
    void on_viderPanier_clicked();
    void on_afficherPanier_clicked();
    void on_calculerPrixTotal_clicked();
    void on_supprimerArticleButton_clicked();

private:
    Ui::MainWindow *ui;
    commande maCommande;
    QSqlTableModel *commandeModel;
    commande commandeInstance;


};

#endif // MAINWINDOW_H
