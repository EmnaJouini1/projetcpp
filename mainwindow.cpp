#include "mainwindow.h"
#include "commande.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "connection.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QPdfWriter>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QFileDialog>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "mainwindow.h"
#include "commande.h"
#include "qprinter.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "connection.h"
#include <QSqlTableModel>
#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QFileDialog>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDate>
#include <QMap>
#include <utility>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTableView>
#include <QImage>
#include <QPixmap>
#include <cmath>
#include <QPair>
#include <QList>
#include <QMap>
#include <QObject>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Vérification de la connexion à la base de données
    if (QSqlDatabase::database().isOpen()) {
        qDebug() << "Connexion réussie.";

        // Initialisation du modèle pour afficher les commandes
        commandeModel = new QSqlTableModel(this);
        commandeModel->setTable("Commande");
        commandeModel->select(); // Charger les données de la table Commande

        ui->tableView_2->setModel(commandeModel);

        // Ajout des options au QComboBox pour l'état des commandes
        ui->comboBox_etat_commande->addItem("En cours");
        ui->comboBox_etat_commande->addItem("Livré");
        ui->comboBox_etat_commande->addItem("Annulé");
    } else {
        qDebug() << "Échec de la connexion. Vérifiez votre base de données.";
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_ajouter_clicked() {
    int id_commande = ui->id_commande->text().toInt();  // Récupérer l'ID de la commande
    int fournisseur = ui->fournisseur->text().toInt();  // Récupérer l'ID du fournisseur
    QDate date_commande = ui->date_commande->date();    // Récupérer la date de la commande
    QDate date_livraison = ui->date_livraison->date();  // Récupérer la date de livraison
    QString liste_articles = ui->liste_article->text(); // Récupérer la liste des articles
    float montant_total = ui->montant_total->text().toFloat();  // Récupérer le montant total
    QString etat_commande = ui->comboBox_etat_commande->currentText(); // Récupérer l'état de la commande

    // Vérification de la saisie
    if (id_commande == 0) {
        QMessageBox::warning(this, "Erreur", "L'ID commande ne peut pas être vide.");
        return;
    }
    if (fournisseur == 0) {
        QMessageBox::warning(this, "Erreur", "Le fournisseur ne peut pas être vide.");
        return;
    }
    if (liste_articles.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "La liste des articles ne peut pas être vide.");
        return;
    }
    if (montant_total <= 0) {
        QMessageBox::warning(this, "Erreur", "Le montant total doit être positif.");
        return;
    }
    if (etat_commande.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'état de la commande ne peut pas être vide.");
        return;
    }
    if (!date_commande.isValid()) {
        QMessageBox::warning(this, "Erreur", "La date de commande n'est pas valide.");
        return;
    }
    if (!date_livraison.isValid()) {
        QMessageBox::warning(this, "Erreur", "La date de livraison n'est pas valide.");
        return;
    }

    // Vérifier si l'ID de la commande existe déjà dans la base de données
    QSqlQuery query;
    query.prepare("SELECT id_commande FROM Commande WHERE id_commande = :id_commande");
    query.bindValue(":id_commande", id_commande);

    if (query.exec()) {
        if (query.next()) {
            // Si l'ID existe déjà, afficher un message d'erreur
            QMessageBox::warning(this, "Erreur", "L'ID de commande existe déjà.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la vérification de l'ID de commande.");
        return;
    }

    // Créer une instance de la commande avec les informations saisies
    commande c(id_commande, fournisseur, date_commande, date_livraison, liste_articles, montant_total, etat_commande);

    // Ajouter la commande
    if (c.ajouter()) {
        // Si l'ajout réussit, afficher un message de succès
        QMessageBox::information(this, tr("Succès"), tr("Commande ajoutée avec succès."));

        // Rafraîchir l'affichage des données dans la vue tableView_2 (le modèle des commandes)
        commandeModel->select();  // Mise à jour de la vue avec les nouvelles données
    } else {
        // Si l'ajout échoue, afficher un message d'erreur
        QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de l'ajout de la commande."));
    }
}

void MainWindow::on_afficher_clicked()
{
    // Create a 'commande' object
    commande cmd;

    // Display the data in the table view
    ui->tableView_2->setModel(cmd.afficher());
}

void MainWindow::on_mettre_a_jour_clicked() {
    int id_commande = ui->id_commande->text().toInt();
    int fournisseur = ui->fournisseur->text().toInt();
    QDate date_commande = ui->date_commande->date();
    QDate date_livraison = ui->date_livraison->date();
    QString liste_articles = ui->liste_article->text();
    float montant_total = ui->montant_total->text().toFloat();
    QString etat_commande = ui->comboBox_etat_commande->currentText();

    commande c(id_commande, fournisseur, date_commande, date_livraison, liste_articles, montant_total, etat_commande);
    if (c.mettre_a_jour()) {
        QMessageBox::information(this, tr("Succès"), tr("Commande mise à jour avec succès."));
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de la mise à jour de la commande."));
    }

    commandeModel->select(); // Rafraîchir l'affichage
}

void MainWindow::on_supprimer_clicked()
{
    // On récupère l'ID de la commande depuis l'interface utilisateur (par exemple, un champ de texte)
    bool ok;
    int code_commande = ui->id_commande_2->text().toInt(&ok);

    if (ok) {
        // On utilise le setter setId pour définir l'ID de la commande
        commandeInstance.setIdCommande(code_commande);

        // Maintenant, vous pouvez utiliser commandeInstance.id_commande (ou getId()) pour gérer la commande
        qDebug() << "ID de la commande : " << commandeInstance.getIdCommande();

        // Supposons que vous supprimez cette commande
        if (commandeInstance.supprimer(code_commande)) {
            QMessageBox::information(this, "Succès", "Commande supprimée.");
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de la suppression de la commande.");
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID de commande valide.");
    }
}


void MainWindow::on_lineEdit_recherche_cursorPositionChanged() {
    QString rechercheText = ui->lineEdit_recherche->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM Commande WHERE id_commande LIKE :rechercheText");
    query.bindValue(":rechercheText", "%" + rechercheText + "%");

    if (query.exec()) {
        QSqlQueryModel *model = new QSqlQueryModel();
        model->setQuery(query);

        model->setHeaderData(0, Qt::Horizontal, "ID Commande");
        model->setHeaderData(1, Qt::Horizontal, "Fournisseur");
        model->setHeaderData(2, Qt::Horizontal, "Date Commande");
        model->setHeaderData(3, Qt::Horizontal, "Date Livraison");
        model->setHeaderData(4, Qt::Horizontal, "Montant Total");
        model->setHeaderData(5, Qt::Horizontal, "Etat Commande");

        ui->tableView_2->setModel(model);
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la recherche.");
    }
}

void MainWindow::on_comboBox_3_activated(int index)
{
    // Vérifier si l'index est valide
    if (index < 0) {
        QMessageBox::warning(this, "Erreur", "Aucune option sélectionnée.");
        return;
    }

    // Récupérer l'option sélectionnée dans le QComboBox
    QString option = ui->comboBox_3->itemText(index).toLower().trimmed();

    // Associer les options au tri SQL
    QString query;
    if (option == "ordre croissant montant total") {
        query = "SELECT * FROM Commande ORDER BY montant_total ASC";
    } else if (option == "ordre décroissant montant total") {
        query = "SELECT * FROM Commande ORDER BY montant_total DESC";
    } else if (option == "ordre croissant id commande") {
        query = "SELECT * FROM Commande ORDER BY id_commande ASC";
    } else if (option == "ordre décroissant id commande") {
        query = "SELECT * FROM Commande ORDER BY id_commande DESC";
    } else if (option == "ordre alphabétique état commande") {
        query = "SELECT * FROM Commande ORDER BY etat_commande ASC";
    } else if (option == "ordre croissant date commande") {
        query = "SELECT * FROM Commande ORDER BY date_commande ASC";
    } else if (option == "ordre décroissant date commande") {
        query = "SELECT * FROM Commande ORDER BY date_commande DESC";
    } else if (option == "par défaut") {
        query = "SELECT * FROM Commande";
    } else {
        QMessageBox::warning(this, "Erreur", "Option non reconnue.");
        return;
    }

    // Exécuter la requête SQL
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);

    // Vérifier si la requête a réussi
    if (model->lastError().isValid()) {
        QMessageBox::critical(this, "Erreur SQL", "Impossible d'exécuter la requête : " + model->lastError().text());
        delete model;
        return;
    }

    // Appliquer le modèle à la tableView
    ui->tableView_2->setModel(model);

    // Ajuster automatiquement les colonnes et les lignes
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->resizeRowsToContents();

    // Facultatif : Message de débogage pour confirmer l'exécution
    qDebug() << "Requête exécutée avec succès : " << query;
}



void MainWindow::on_pdf_clicked() {
    // Get the QSqlQueryModel from the table view
    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView_2->model());

    if (model) {
        QString content;

        // Define the logo path and check if the logo exists
        QString logoPath = "C:/Users/21624/Desktop/hedha howaa/emnaaaaaaaa/logo.png";  // Correct the path to your logo image
        int logoWidth = 100;  // Logo width in pixels
        int logoHeight = 100; // Logo height in pixels

        QFile logoFile(logoPath);
        if (!logoFile.exists()) {
            QMessageBox::warning(this, "Erreur", "Le fichier logo n'existe pas.");
            return;
        }

        // Insert the logo at the top of the document
        content += "<div style=\"text-align:center; margin-bottom: 20px;\">";
        content += "<img src=\"" + QUrl::toPercentEncoding(logoPath) + "\" width=\"" + QString::number(logoWidth) + "\" height=\"" + QString::number(logoHeight) + "\">";
        content += "</div>";

        // Title of the document
        content += "<h1 style=\"text-align:center;\">Liste des Commandes</h1>";

        // Table style
        content += "<style>table { border-collapse: collapse; width: 100%; } th, td { border: 1px solid black; padding: 8px; text-align: center; } th { background-color: lightgray; }</style>";
        content += "<table>";

        // Add column headers
        content += "<tr><th>ID Commande</th><th>Fournisseur</th><th>Date Commande</th><th>Date Livraison</th><th>Liste Articles</th><th>Montant Total</th><th>Etat Commande</th></tr>";

        // Check if the model has any rows
        if (model->rowCount() > 0) {
            for (int row = 0; row < model->rowCount(); ++row) {
                content += "<tr>";
                content += "<td>" + model->data(model->index(row, 0)).toString() + "</td>";
                content += "<td>" + model->data(model->index(row, 1)).toString() + "</td>";
                content += "<td>" + model->data(model->index(row, 2)).toString() + "</td>";
                content += "<td>" + model->data(model->index(row, 3)).toString() + "</td>";
                content += "<td>" + model->data(model->index(row, 4)).toString() + "</td>";
                content += "<td>" + model->data(model->index(row, 5)).toString() + "</td>";
                content += "<td>" + model->data(model->index(row, 6)).toString() + "</td>";
                content += "</tr>";
            }
        } else {
            content += "<tr><td colspan=\"7\">Aucune donnée disponible</td></tr>";
        }

        // End table
        content += "</table>";

        // Create QTextDocument and set its HTML content
        QTextDocument document;
        document.setHtml(content);

        // Dialog for saving the PDF
        QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "commandes", "Fichiers PDF (*.pdf)");

        if (!fileName.isEmpty()) {
            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);

            // Directly print the document
            document.print(&printer);

            // Show success message
            QMessageBox::information(this, "Export PDF", "Le PDF a été créé avec succès.");
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Le modèle de données est invalide.");
    }
}



void MainWindow::on_statistique_clicked()
{

    // Obtenir les statistiques des commandes
    QMap<QString, int> statistiques = commandeInstance.statistiquesEtat(); // Remplacez commandeInstance par votre instance

    // États attendus (par exemple, dans la comboBox)
    QStringList etats = { "En cours", "Livré", "Annulé" };

    if (!statistiques.isEmpty()) {
        QVector<QPair<QString, int>> chartData;

        // Filtrer les données pour afficher uniquement les états pertinents
        for (const QString& etat : etats) {
            int count = statistiques.value(etat, 0); // Valeur par défaut à 0 si état inexistant
            chartData.append(qMakePair(etat, count));
        }

        // Total pour le calcul des pourcentages
        int total = 0;
        for (const auto& item : chartData) {
            total += item.second;
        }

        // Vérifier s'il y a des données
        if (total == 0) {
            QMessageBox::information(this, tr("Aucune donnée"), tr("Aucune commande trouvée pour ces états."));
            return;
        }

        // Palette de couleurs
        QVector<QColor> colorPalette = {
            QColor(255, 99, 71),  // Tomato red
            QColor(70, 130, 180), // Steel blue
            QColor(34, 139, 34)   // Forest green
        };

        // --- Création du graphique en secteurs ---
        const int pieChartWidth = 400;
        const int pieChartHeight = 400;
        QImage pieChartImage(pieChartWidth, pieChartHeight, QImage::Format_ARGB32);
        pieChartImage.fill(Qt::white);

        QPainter pieChartPainter(&pieChartImage);
        pieChartPainter.setRenderHint(QPainter::Antialiasing);

        QRectF pieRect(50, 50, 300, 300);
        int startAngle = 0;
        int colorIndex = 0;

        for (const auto& item : chartData) {
            int spanAngle = (item.second * 360) / total;
            QColor color = colorPalette[colorIndex % colorPalette.size()];
            pieChartPainter.setBrush(color);

            pieChartPainter.drawPie(pieRect, startAngle * 16, spanAngle * 16);

            // Affichage des étiquettes
            int midAngle = startAngle + spanAngle / 2;
            QPointF textPosition(
                pieRect.center().x() + 100 * qCos(midAngle * M_PI / 180),
                pieRect.center().y() - 100 * qSin(midAngle * M_PI / 180)
            );

            pieChartPainter.setPen(Qt::black);
            pieChartPainter.drawText(textPosition, item.first);

            startAngle += spanAngle;
            colorIndex++;
        }

        QPixmap pieChartPixmap = QPixmap::fromImage(pieChartImage);

        // Configuration de tableView_2
        QStandardItemModel* pieChartModel = new QStandardItemModel(1, 1, this);
        QStandardItem* pieChartItem = new QStandardItem();
        pieChartItem->setData(pieChartPixmap, Qt::DecorationRole);
        pieChartModel->setItem(0, 0, pieChartItem);
        ui->tableView_3->setModel(pieChartModel);
        ui->tableView_3->resizeColumnsToContents();
        ui->tableView_3->resizeRowsToContents();

        // --- Création du graphique à barres ---
        const int barChartWidth = 500;
        const int barChartHeight = 300;
        QImage barChartImage(barChartWidth, barChartHeight, QImage::Format_ARGB32);
        barChartImage.fill(Qt::white);

        QPainter barChartPainter(&barChartImage);
        barChartPainter.setRenderHint(QPainter::Antialiasing);

        int barWidth = 50;
        int gap = 20;
        int xOffset = 40;
        int maxBarHeight = barChartHeight - 60;

        int maxValue = 0;
        for (const auto& item : chartData) {
            maxValue = qMax(maxValue, item.second);
        }

        float scaleFactor = static_cast<float>(maxBarHeight) / maxValue;
        colorIndex = 0;

        for (const auto& item : chartData) {
            int barHeight = static_cast<int>(item.second * scaleFactor);
            QRectF barRect(xOffset, barChartHeight - barHeight - 30, barWidth, barHeight);

            QColor barColor = colorPalette[colorIndex % colorPalette.size()];
            barChartPainter.setBrush(barColor);
            barChartPainter.drawRect(barRect);

            barChartPainter.setPen(Qt::black);
            barChartPainter.drawText(
                QPointF(barRect.x() + barWidth / 4, barRect.y() - 5),
                QString::number(item.second)
            );
            barChartPainter.drawText(
                QPointF(barRect.x() + barWidth / 4, barChartHeight - 10),
                item.first
            );

            xOffset += barWidth + gap;
            colorIndex++;
        }

        QPixmap barChartPixmap = QPixmap::fromImage(barChartImage);

        // Configuration de tableView_3
        QStandardItemModel* barChartModel = new QStandardItemModel(1, 1, this);
        QStandardItem* barChartItem = new QStandardItem();
        barChartItem->setData(barChartPixmap, Qt::DecorationRole);
        barChartModel->setItem(0, 0, barChartItem);
        ui->tableView_4->setModel(barChartModel);
        ui->tableView_4->resizeColumnsToContents();
        ui->tableView_4->resizeRowsToContents();
    } else {
        QMessageBox::warning(this, tr("Erreur"), tr("Aucune donnée statistique trouvée."));
    }
}

void MainWindow::on_ajouterArticle_clicked()
{
    // Récupérer les données saisies dans les champs
    QString article = ui->lineEdit_article->text();  // Nom de l'article
    bool ok;
    float prix = ui->lineEdit_prix->text().toFloat(&ok);  // Prix de l'article (converti en float)

    // Vérifier si les données sont valides
    if (article.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom de l'article ne peut pas être vide.");
        return;
    }
    if (!ok || prix <= 0) {
        QMessageBox::warning(this, "Erreur", "Le prix doit être un nombre positif.");
        return;
    }

    // Ajouter l'article et son prix au panier
    commandeInstance.ajouterArticle(article, prix);

    // Mettre à jour l'affichage du panier
    ui->textEdit_panier->setPlainText(commandeInstance.afficherPanier());

    // Effacer les champs de saisie pour un nouvel ajout
    ui->lineEdit_article->clear();
    ui->lineEdit_prix->clear();

    // Afficher un message de confirmation
    QMessageBox::information(this, "Succès", "Article ajouté au panier.");
}


void MainWindow::on_afficherPanier_clicked()
{
    // Récupérer et afficher le contenu du panier
    QString panierText = commandeInstance.afficherPanier();
    ui->textEdit_panier->setPlainText(panierText);
}


void MainWindow::on_viderPanier_clicked()
{
    // Vider le panier
    commandeInstance.viderPanier();

    // Mettre à jour l'affichage du panier (affichage vide)
    ui->textEdit_panier->setPlainText("Le panier est maintenant vide.");

    // Afficher un message de confirmation
    QMessageBox::information(this, "Succès", "Le panier a été vidé.");
}



void MainWindow::on_supprimerArticleButton_clicked()
{
    // Récupérer le nom de l'article à supprimer
    QString articleASupprimer = ui->lineEdit_article->text();

    if (articleASupprimer.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez saisir le nom de l'article à supprimer.");
        return;
    }

    // Supprimer l'article du panier
    commandeInstance.supprimerArticle(articleASupprimer);

    // Mettre à jour l'affichage du panier après suppression
    QString panierText = commandeInstance.afficherPanier();
    ui->textEdit_panier->setPlainText(panierText);

    // Afficher un message de confirmation
    QMessageBox::information(this, "Succès", QString("L'article '%1' a été supprimé.").arg(articleASupprimer));
}


void MainWindow::on_calculerPrixTotal_clicked()
{
    // Appeler la méthode calculerPrixTotal de la commandeInstance pour obtenir le total
    float total = commandeInstance.calculerPrixTotal();

    // Vérifier si le panier est vide (total égal à 0)
    if (!commandeInstance.afficherPanier().contains("vide")) {
        // Afficher le prix total dans un QLabel ou tout autre widget
        ui->prix->setText(QString("Total : %1 €").arg(total, 0, 'f', 2));  // Formatage avec 2 décimales
    } else {
        // Si le panier est vide, afficher un message approprié
        ui->prix->setText("Le panier est vide.");
    }
}

