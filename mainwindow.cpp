#include "includer.h"
#include "mediaorg.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->imageThumbnailDisplay->setViewMode(QListWidget::IconMode);
    ui->imageThumbnailDisplay->setIconSize(QSize(75, 75));
    ui->imageThumbnailDisplay->setResizeMode(QListWidget::Adjust);
    ui->imageThumbnailDisplay->setGridSize(QSize(100, 200));

    mediaDirPath = "Media/Images/";

}

MainWindow::~MainWindow()
{
    CloseDB();
    //delete &db;
    delete ui;
}

int MainWindow::OpenDB(QString path) {
    int rc = 0;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    rc = db.open();
    qDebug() << "rc after attempting to open DB: " << rc << "\n";
    if(!rc) {
        QMessageBox err;
        err.setText("Error: Failed to open Database.");
        err.exec();
    }

    return(!rc);
}
//Check the database for necessary tables. If one or more tables are missing, notify the user and return a non-zero value.
int MainWindow::VerifyDBStructure() {
    //Check if the DB is open. If it isn't, notify the user and return a non-zero value
    if(!db.isOpen()) {
        QMessageBox err;
        err.setText("Error: failed to run DB verification query. Database is not open.");
        err.setIcon(QMessageBox::Warning);
        err.exec();
        return(1);
    }

    //Prepare the query to pull the list of tables in the DB. If the query fails, notify the user and return a non-zero value.
    QSqlQuery dbCommand(db);
    dbCommand.prepare("SELECT name FROM sqlite_master WHERE type='table';");
    if (dbCommand.exec() == false) {
        QMessageBox warn;
        warn.setText("Error: failed to run DB verification query.");
        warn.setIcon(QMessageBox::Warning);
        warn.exec();
        return(2);
    }

    //Step through the query results and store them in a vector
    QVector<QString> dbTables;
    QSqlRecord record = dbCommand.record();
    int nameCol = record.indexOf("name");
    while(dbCommand.next()) {
        qDebug() << dbCommand.value(nameCol).toString();
        dbTables.push_back(dbCommand.value(nameCol).toString());
    }
    dbCommand.finish();

    //Test for all necessary tables and warn the user if one is missing.
    bool isImageFilesTablePresent = false;
    bool isTagsTablePresent = false;
    for(int i = 0; i < dbTables.length(); i++) {
        if(dbTables[i] == "ImageFiles") {
            isImageFilesTablePresent = true;
        }
        if(dbTables[i] == "Tags") {
            isTagsTablePresent = true;
        }
    }
    if(!isImageFilesTablePresent || !isTagsTablePresent) {
        //TO DO: Could make this nicer by adding a combined warning message if both tables are missing.
        if(!isImageFilesTablePresent) {
            QMessageBox warn;
            warn.setIcon(QMessageBox::Warning);
            warn.setText("Warning: Database is missing the ImageFiles table.");
            warn.exec();
        }
        if(!isTagsTablePresent) {
            QMessageBox warn;
            warn.setIcon(QMessageBox::Warning);
            warn.setText("Warning: Database is missing the Tags table.");
            warn.exec();
        }
        return(2);
    }

    return(0);
}

//This member function reuses most of the code from the verification function, however instead of warning the user, it creates the tables that are missing.
int MainWindow::RectifyDBStructure() {

    //Verify the DB is open before proceeding, as nothing else can be done for this function if it isn't
    if(!db.isOpen()) {
        QMessageBox err;
        err.setText("Error: failed to rectify DB. Database is not open.");
        err.setIcon(QMessageBox::Warning);
        err.exec();

        return(1);
    }

    //Create the query and run it. If it fails to run, notify the user.
    QSqlQuery dbCommand(db);
    dbCommand.prepare("SELECT name FROM sqlite_master WHERE type='table';");
    if (dbCommand.exec() == false) {
        QMessageBox err;
        err.setText("Error: failed to run DB verification query.");
        err.setIcon(QMessageBox::Warning);
        err.exec();
    }

    //Store the names of the tables found in the DB in a vector
    QVector<QString> dbTables;
    QSqlRecord record = dbCommand.record();
    int nameCol = record.indexOf("name");
    while(dbCommand.next()) {
        qDebug() << dbCommand.value(nameCol).toString();
        dbTables.push_back(dbCommand.value(nameCol).toString());
    }
    dbCommand.finish();

    //Go through the vector with the table names and check if one or more tables are missing. If a table is missing, add it to the DB.
    bool isImageFilesTablePresent = false;
    bool isTagsTablePresent = false;
    for(int i = 0; i < dbTables.length(); i++) {
        if(dbTables[i] == "ImageFiles") {
            isImageFilesTablePresent = true;
        }
        if(dbTables[i] == "Tags") {
            isTagsTablePresent = true;
        }
    }
    if(!isImageFilesTablePresent || !isTagsTablePresent) {
        if(!isImageFilesTablePresent) {
            dbCommand.prepare("CREATE TABLE ImageFiles(ID INTEGER PRIMARY KEY AUTOINCREMENT, Filename TEXT NOT NULL, FileSize INT NOT NULL, Filetype TEXT NOT NULL, imageWidth INTEGER NOT NULL, imageHeight NOT NULL, Tags TEXT NOT NULL);");
            bool success = dbCommand.exec();
            if(!success) {
                QMessageBox err;
                err.setIcon(QMessageBox::Warning);
                err.setText("Error: Failed to create ImageFiles table.");
                err.exec();
                return(2);
            }
            dbCommand.finish();
        }
        if(!isTagsTablePresent) {
            dbCommand.prepare("CREATE TABLE Tags(ID INTEGER PRIMARY KEY AUTOINCREMENT, TagName TEXT NOT NULL);");
            bool success = dbCommand.exec();
            if(!success) {
                QMessageBox err;
                err.setIcon(QMessageBox::Warning);
                err.setText("Error: Failed to create Tags table.");
                err.exec();
                return(3);
            }
            dbCommand.finish();
            dbCommand.prepare("INSERT INTO Tags (TagName) VALUES('tag_me');");
            dbCommand.exec();
            dbCommand.finish();
        }
    }

    return(0);
}

int MainWindow::CloseDB() {
    db.close();
    return(0);
}

//THIS IS A DEVELOPMENT-ONLY FUNCTION
//All filenames should be acquired via the SQL database. Nothing should actually be pulled directly from the file without
//  first referencing the DB.
/*QStringList MainWindow::GetFilenamesForDir(QString path) const {
    QStringList filenamesFromDir;
    QDir dir(path);

    if(!dir.exists()) {
        qDebug() << "Error: attempting to pull file list from nonexistent directory.";
        filenamesFromDir.push_back("");
        return(filenamesFromDir);
    }

    //Remove this when proper search functionality is implemented
    QStringList filetypeFilters;
    filetypeFilters << "*.jpeg" << "*.jpg" << "*.png" << "*.bitmap" << "*.bmp" << "*.gif";
    dir.setNameFilters(filetypeFilters);

    filenamesFromDir = dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::Readable);


    return(filenamesFromDir);
}*/

//***********************************************************

int MainWindow::PopulateListWidget(QStringList filenames) {

    if(filenames.isEmpty()) {
        qDebug() << "Error: no filenames provided.";
        return(1);
    }


    while(ui->imageThumbnailDisplay->count() > 0) {
        delete ui->imageThumbnailDisplay->takeItem(0);
    }
    ui->imageThumbnailDisplay->clear();

    for(int i = 0; i < filenames.size(); i++) {
        qDebug() << mediaDirPath+filenames[i];
        //QIcon imageIcon(mediaDirPath+filenames[i]);
        QListWidgetItem * imageThumbnail = new QListWidgetItem(QIcon(mediaDirPath+filenames[i]), filenames[i]);
        imageThumbnail->setData(Qt::UserRole, QVariant(QString(mediaDirPath+filenames[i])));
        imageThumbnail->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        imageThumbnail->setSizeHint(QSize(100, 100));
        qDebug() << "Adding thumbnail for: " << mediaDirPath+filenames[i];

        ui->imageThumbnailDisplay->addItem(imageThumbnail);
    }
    //ui->imageThumbnailDisplay->setGridSize(QSize(100, 200));

    return(0);
}

QStringList MainWindow::RunSearch(QString searchString) const {
    qDebug() << "Running search for: " << searchString;
    QSqlQuery dbCommand;
    QStringList filesFound;

    if(searchString.isEmpty() != true) {
        dbCommand.prepare( "SELECT Filename FROM ImageFiles WHERE Filename LIKE :searchStringVal" );
        dbCommand.bindValue(":searchStringVal", "%"+searchString+"%");
    }
    else {
        dbCommand.prepare( "SELECT Filename FROM ImageFiles" );
    }

    if(!dbCommand.exec()) {
        qDebug("Error: failed to run search for file.");
    }

    QSqlRecord record = dbCommand.record();
    int filenameCol = record.indexOf("Filename");
    while(dbCommand.next()) {
        filesFound.push_back(dbCommand.value(filenameCol).toString());
    }

    dbCommand.finish();

    return(filesFound);
}

QString MainWindow::GetMediaDirPath() const {
    return(mediaDirPath);
}

void MainWindow::on_commonTasks_AddImageButton_clicked() {

    //Present prompt to get file to add
    //QDialog addNewImage(this);
    //addNewImage.setWindowTitle("Media_Organizer -- Add Image");
    QString filename = QFileDialog::getOpenFileName(this, "Add Image to Media_Organizer", "C:/", "Image Files(*.png *.jpg *.jpeg *.gif *.bmp *.bitmap");
    if(filename.isEmpty()) {
        qDebug() << "Warning: failed to acquire filename.";
        return;
    }

    QFileInfo fileToAdd(filename);
    if(!fileToAdd.exists()) {
        QMessageBox err;
        err.setText("Error: specified file doesn't exist.");
        err.setIcon(QMessageBox::Warning);
        err.exec();
        return;
    }

    QFile file(filename);
    if(!file.exists()) {
        QMessageBox err;
        err.setText("Error: failed to find file when attempting to copy it to Media_Organizer's 'Media' directory.");
        err.setIcon(QMessageBox::Warning);
        err.exec();
        return;
    }
    if(!file.copy(mediaDirPath+fileToAdd.fileName())) {
        qDebug() << "Failed to copy file to media directory";
        qDebug() << "File to copy: " << file.fileName();
        qDebug() << "New File location: " << mediaDirPath+fileToAdd.fileName();
    }

    QSqlQuery dbCommand;
    //CREATE TABLE ImageFiles(ID INTEGER PRIMARY KEY AUTOINCREMENT, Filename TEXT NOT NULL, FileSize INT NOT NULL, Filetype TEXT NOT NULL,
    //                                                              imageWidth INTEGER NOT NULL, imageHeight NOT NULL, Tags TEXT NOT NULL);
    //TO DO: Add EXIF processing to acquire image dimension values
    //TO DO: Add proper tagging system to allow addition of tags.
    dbCommand.prepare("INSERT INTO ImageFiles (Filename, FileSize, Filetype, imageWidth, imageHeight, Tags) VALUES(:filename, :size, :type, 0, 0, 0);");
    dbCommand.bindValue(":filename", fileToAdd.fileName());
    dbCommand.bindValue(":size", fileToAdd.size());
    dbCommand.bindValue(":type", fileToAdd.suffix());
    dbCommand.exec();
    dbCommand.finish();

    PopulateListWidget(RunSearch(""));

    //Add file to DB
    //Update thumbnail display listWidget to show new addition(s)

}

void MainWindow::on_commonTasks_QuickSearchButton_clicked()
{

    QString searchVal = ui->commonTasks_QuickSearchLineEdit->text();
    qDebug() << "Grid size before running search: (" << ui->imageThumbnailDisplay->gridSize().width() << ", " << ui->imageThumbnailDisplay->gridSize().height() << ")";
    qDebug() << "Spacing before: " << ui->imageThumbnailDisplay->spacing();
    QStringList searchResults = RunSearch(searchVal);
    qDebug() << "Grid size after running search: (" << ui->imageThumbnailDisplay->gridSize().width() << ", " << ui->imageThumbnailDisplay->gridSize().height() << ")";
    qDebug() << "Spacing after: " << ui->imageThumbnailDisplay->spacing();
    PopulateListWidget(searchResults);
}

void MainWindow::on_imageThumbnailDisplay_itemDoubleClicked(QListWidgetItem *item) {
    QVariant itemData = item->data(Qt::UserRole);
    QString filePath = itemData.toString();

    QDialog viewer(this);
    viewer.setWindowTitle("Media_Organizer -- "+filePath);
    QVBoxLayout * layout = new QVBoxLayout;
    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsView * viewRegion = new QGraphicsView(scene);
    QImage img(filePath);
    QGraphicsPixmapItem * imagePixmap = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    scene->addItem(imagePixmap);
    //viewRegion->setScene(scene);
    //QLabel * l = new QLabel;
    //l->setText(filePath);
    layout->addWidget(viewRegion);

    viewer.setLayout(layout);
    viewer.exec();
    delete layout;
    delete scene;
    delete viewRegion;
    //TO DO: Figure out why deleting imagePixmap results in the program crashing even though deleting everything else works fine (maybe it's already deleted when viewRegion or scene is?)
    return;
}

void MainWindow::on_commonTasks_RemoveImageButton_clicked()
{
    QList<QListWidgetItem *> selectedThumbnails = ui->imageThumbnailDisplay->selectedItems();
    for(QListWidgetItem * t : selectedThumbnails) {
        QVariant d = t->data(Qt::UserRole);
        //qDebug() << d.toString();
        QFileInfo f(d.toString());
        QSqlQuery dbCommand;
        dbCommand.prepare("DELETE FROM ImageFiles WHERE Filename=:fn;");
        dbCommand.bindValue(":fn", f.fileName());
        dbCommand.exec();
        dbCommand.finish();
    }
    qDeleteAll(selectedThumbnails);

}
