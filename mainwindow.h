#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "includer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int OpenDB(QString);
    int VerifyDBStructure();
    int RectifyDBStructure();
    int CloseDB();
    QStringList GetFilenamesForDir(QString) const;
    int PopulateListWidget(QStringList);
    QStringList RunSearch(QString) const;

    QString GetMediaDirPath() const;

private slots:
    void on_commonTasks_AddImageButton_clicked();

    void on_commonTasks_QuickSearchButton_clicked();

    void on_imageThumbnailDisplay_itemDoubleClicked(QListWidgetItem *item);

    void on_commonTasks_RemoveImageButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QString mediaDirPath;
};


#endif // MAINWINDOW_H
