#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include "minutea.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QImage image[2];
    QImage imageCopy[2];
    int **imageArray[2];
    bool **pixels[2];
    QList<Minutea*> minutea[2];
    QImage image2;
    QImage imageCopy2;
    QString lastFileName;
    int **imageArray2;
    static int A1px[];
    static int A0[];
    static int A1[];
    static int A2[];
    static int A3[];
    static int A4[];
    static int A5[];
    static int *A[7];

private:
    QPoint findMinutia(QPoint from, QPoint current, bool &begining, int index);
    bool isBlack(QRgb rgb);
    bool checkPixel(QPoint from, int x, int y, int index);
    bool checkRectangle(int x, int y, int index);
    bool checkRectangleImage (int x, int y, int index);
    void searchLine(int x, int y, QColor color, bool end, int index);
    void drawRectangle(QPoint point, QColor color, bool end, int index);
    void switchImage(int index);
    QTextStream& qStdOut();
    void thinnImage(QImage *img, int index, int ind);
    int weight(int x, int y, int index);
    bool checkArray(int weight, int index);
    void negateImage(int index);
    void searchMinutia(QColor color, bool end, int index);
    bool checkBorder(int x, int y, int index);
    void makeGraph(int index);
    bool verify(Minutea *m, Minutea *n, QList<Minutea *> *toVerification1, QList<Minutea*> *toVerification2);


private slots:
    void on_loadButton_clicked();
    void on_findButton_clicked();
    void on_loadButton2_clicked();
    void on_findButton2_clicked();
    void on_checkButton_clicked();
};

#endif // MAINWINDOW_H
