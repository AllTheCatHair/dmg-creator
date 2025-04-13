#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileDialogPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_appPathChooseBtn_clicked()
{

    QString appPath = QFileDialog::getOpenFileName(this,"选择一个app文件",fileDialogPath,"应用程序(*.app)");
    if(appPath.isEmpty()){
        qDebug() << "选择的路径为空";
        return;
    }

    ui->appPathLineEdit->setText(appPath);
}


void MainWindow::on_appBackgroundChooseBtn_clicked()
{
    QString appBackgroundPath =QFileDialog::getOpenFileName(this,"选择一个背景图片",fileDialogPath,"png图片(*.png)");
    if(appBackgroundPath.isEmpty()){
        qDebug() << "选择的路径为空";
        return;
    }

    ui->appBackgroundLineEdit->setText(appBackgroundPath);
}




void MainWindow::on_dmgOutputChooseBtn_clicked()
{
    QString dmgOutputPath = QFileDialog::getExistingDirectory(this,"选择输出路径",fileDialogPath,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(dmgOutputPath.isEmpty()){
        qDebug() << "选择的路径为空";
        return;
    }

    ui->dmgOutputLineEdit->setText(dmgOutputPath);
}


void MainWindow::on_iconPathChooseBtn_clicked()
{
    QString iconPath = QFileDialog::getOpenFileName(this,"选择图标路径",fileDialogPath,"icns图标(*.icns)");
    if(iconPath.isEmpty()){
        qDebug()<<"选择的路径为空";
    }
    ui->iconPathLineEdit->setText(iconPath);
}



void MainWindow::on_createDmgBtn_clicked()
{
    //app路径
    QString appPath = ui->appPathLineEdit->text();
    qDebug()<<"app的路径"<<appPath;

    //app大小
    qreal appSize = getDirSize(appPath)+10;
    qDebug()<<"app总大小" <<appSize<<"mb";

    //dmg路径
    QString dmgOutputPath = ui->dmgOutputLineEdit->text();
    qDebug()<<"dmg路径"<<dmgOutputPath;

    //dmg名称
    QString dmgName = ui->dmgNameLineEdit->text();
    qDebug() << "dmg的名称" << dmgName;

    //背景路径
    QString backgroundPath =ui ->appBackgroundLineEdit->text();
    qDebug() << "背景路径" <<backgroundPath;

    //宽和高
    QString width = ui->widthLineEdit->text();
    QString height = ui->heightLineEdit->text();
    qDebug() << "宽"<<width<<"高"<<height;



    //准备命令行参数
    QStringList args;
    args << "create"
         << "-volname" << dmgName
         << "-srcfolder" << appPath
         << "-ov"
         << "-format" << "UDRW"
         << QString("%1/%2.dmg").arg(dmgOutputPath, dmgName);



    //执行命令
    QProcess process;
    process.start("hdiutil", args);
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError()+process.readAllStandardOutput();

    //挂载创建的dmg
    QString mountPoint = QString("/Volumes/%1").arg(dmgName);
    process.start("hdiutil", {"attach", QString("%1/%2.dmg").arg(dmgOutputPath, dmgName)});
    process.waitForFinished(-1);
    qDebug()<<"mount位置"<<mountPoint;

    //拷贝背景图片到
    process.start("mkdir", {QString("%1/.background").arg(mountPoint)});
    process.waitForFinished(-1);
    process.start("cp", {backgroundPath, QString("%1/.background/background.png").arg(mountPoint)});
    process.waitForFinished(-1);



    // QString result = process.readAllStandardError()+process.readAllStandardOutput();
    // qDebug() << result;

    // 使用 AppleScript 调整窗口大小
    // 确保输入的是有效的数字
    bool okWidth, okHeight;
    int widthInt = width.toInt(&okWidth);
    int heightInt = height.toInt(&okHeight);

    // 将 QString 转换为 UTF-8 编码的字符串
    QByteArray dmgNameUtf8 = dmgName.toUtf8();
    QByteArray backgroundPathUtf8 = (mountPoint+"/.background/background.png").toUtf8();


        // 构建 AppleScript 脚本，动态替换宽度和高度
        QString appleScript = QString(R"(
tell application "Finder"
    open disk "%1" -- 使用你的 DMG 名称

    delay 2 -- 给 Finder 一些时间加载

    set bounds of window of disk "%1" to {%2, %3, %4, %5} -- 设置窗口大小


end tell
)").arg(QString::fromUtf8(dmgNameUtf8))
                                  .arg(200)  // 左边距
                                  .arg(200)  // 上边距
                                  .arg(widthInt + 200)  // 右边距，宽度加上一些偏移
                                  .arg(heightInt + 200);  // 下边距，高度加上一些偏移
        // 执行 AppleScript
        QProcess::execute("osascript", QStringList() << "-e" << appleScript);


        // QString script = QString("osascript -e 'tell application \"Finder\" to set desktop picture to POSIX file \"%1\"'").arg(backgroundPathUtf8);

        QString script = QString("osascript -e 'tell application \"Finder\" to set desktop picture to {%1, %2, %3}'")
                             .arg(0).arg(0).arg(0);
        QProcess::execute(script);


    QString result = process.readAllStandardError()+process.readAllStandardOutput();
    qDebug() << result;

    QMessageBox::information(this,"输出",result);






}



///计算文件夹大小
qreal  MainWindow::getDirSize(const QString &dirPath) {
    QDir dir(dirPath);
    qint64 totalSize = 0;

    if (!dir.exists()) {
        return totalSize;  // 如果目录不存在，返回 0
    }

    // 遍历目录中的所有文件，包括子目录
    QDirIterator it(dirPath, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo(it.filePath());
        if (fileInfo.isFile()) {
            totalSize += fileInfo.size();  // 如果是文件，累加文件大小
        }
    }

    return totalSize/1000/1000;
}

void MainWindow::on_createdmgCreateBtn_clicked()
{

    //app路径
    QString appPath = ui->appPathLineEdit->text();
    qDebug()<<"app的路径"<<appPath;

    //app大小
    qreal appSize = getDirSize(appPath)+10;
    qDebug()<<"app总大小" <<appSize<<"mb";

    //dmg路径
    QString dmgOutputPath = ui->dmgOutputLineEdit->text();
    qDebug()<<"dmg路径"<<dmgOutputPath;

    //dmg名称
    QString dmgName = ui->dmgNameLineEdit->text();
    qDebug() << "dmg的名称" << dmgName;

    //背景路径
    QString backgroundPath =ui ->appBackgroundLineEdit->text();
    qDebug() << "背景路径" <<backgroundPath;

    //宽和高
    QString width = ui->widthLineEdit->text();
    QString height = ui->heightLineEdit->text();
    qDebug() << "宽"<<width<<"高"<<height;

    QString iconPath  = ui->iconPathLineEdit->text();
    qDebug()<<"图标路径"<<iconPath;

    QString iconSize = ui->iconSizeLineEdit->text();
    qDebug()<<"图标大小"<<iconSize;




    QProcess process;
    qDebug()<<"process";
    QStringList arguments;
    arguments << "--volname" << dmgName
              << "--volicon" << iconPath
              << "--background" << backgroundPath
              << "--window-pos" << "200" << "120"
              << "--window-size" << width << height
              << "--icon-size" << iconSize
              << "--icon" << "Application.app" << "200" << "190"
              << "--hide-extension" << "Application.app"
              << "--app-drop-link" << "600" << "185"
              << "Application-Installer.dmg"
              << appPath;

    // 启动 create-dmg
    // process.start("create-dmg", arguments);
    process.start(QDir::currentPath()+"/create-dmg",arguments);

    process.waitForFinished(-1);
    qDebug()<<QDir::currentPath();
    QString result = process.readAllStandardError()+process.readAllStandardOutput();
    qDebug() << result;

    QMessageBox::information(this,"输出",result);

}


