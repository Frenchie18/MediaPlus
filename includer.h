#ifndef INCLUDER_H
#define INCLUDER_H

#include <QtGlobal>
#include <QtDebug>
#include <QMainWindow>
#include <QWidget>
#include <QApplication>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QString>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDialog>
#include <QLayout>
#include <QFileDialog>

/*While most of the documentation floating around says to use "#include <QtSql>" in the source files and "QT += sql" in the *.pro file,
 * This doesn't work with the setup being used for development. All files are present and in the correct locations, so they are being included
 * by using the #include statements seen below (#include <QtSql/DESIRED_FILE>).
 */
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>



#endif // INCLUDER_H
