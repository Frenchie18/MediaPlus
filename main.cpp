
#include "includer.h"

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    //add testing for data dir and make it if it's missing
    w.OpenDB("Data/MediaOrganizerDB.sqlite");

    //Check if the necessary tables are in the DB and, if not, add them.
    if(w.VerifyDBStructure()) {
        w.RectifyDBStructure();
    }

    QString p = w.GetMediaDirPath();
    qDebug() << "Main -- media dir path: " << p;
    QStringList l =  w.RunSearch("");//w.GetFilenamesForDir(p); //

    w.PopulateListWidget(l);

    //QStringList testList = w.RunSearch("harm");
    //for(QString s : testList) {
    //    qDebug() << "Found: " << s;
    //}


    w.show();

    return a.exec();
}
