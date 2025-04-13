#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QFileDialog>
#include <Qprocess>
#include <QDirIterator>
#include <QMessageBox>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_appPathChooseBtn_clicked();

    void on_appBackgroundChooseBtn_clicked();

    void on_dmgOutputChooseBtn_clicked();

    void on_createDmgBtn_clicked();

    void on_createdmgCreateBtn_clicked();

void on_iconPathChooseBtn_clicked();

private:
    Ui::MainWindow *ui;
    qreal getDirSize(const QString &path);
    QString fileDialogPath;

};
#endif // MAINWINDOW_H
