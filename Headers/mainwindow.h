#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include "minutiae.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*
     * ctor
     */
    explicit MainWindow(QWidget *parent = 0);
    /*
     * d-tor
     */
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    /* Index of the image: 0 - first image is processed, 1 - second image is processed */
    int INDEX;

    /* Color of drawn rectangle */
    QColor COLOR;

    /* Type of searched minutiae */
    bool TYPE;

    QImage image[2];
    QImage imageCopy[2];
    int **K3MImageArray[2];
    bool **checkedPixels[2];
    QList<Minutiae*> minutiaes[2];

    QString lastFileName;

private:
    /*
     * stdout - print message
     */
    QTextStream& qStdOut();

    /*
     * check pixel color
     *
     * @param:  rgb - color
     * @return: true - black, false - while
     */
    bool isBlack(QRgb rgb);

    /*
     * Draws rectangle around found minutiae
     *
     * @param: point - found minutiae
     *         color - color of rectangle
     *         end - type of minutiae
     */
    void drawRectangle(QPoint point);

    /*
     * Checkes wheter the pixel lies at the border
     * of the fingerpring
     *
     * @param:  x, y - coordinates of the pixel
     * @return: true if the pixel is on border, false otherwise
     */
    bool checkIfAtBorder(int x, int y);

    /*
     * Checkes wheter the given coordinates point to
     * a proper bifurcation
     *
     * @param:  x, y - coordinates of pixel
     * @return: true if the pixel is bifurcation false otherwise
     */
    bool isBifurcation (int x, int y);

    /*
     * Makes sure count shows proper number of neighbours
     *
     * @param: directions - table indicating where are neighbours
     *         out count - neighbours count
     */
    void reduceCount(bool directions[], int &count);

    /*
     * Makes sure the pixel is on path(is black)
     * and wasn't checked before
     *
     * @param:  x, y - coordinates of pixel
     *          out count - number of neighbours
     *          out to - list of next pixels on path
     * @return: true - if the neighbour is black, false otherwise
     */
    bool isPixelOnPath(int x, int y, int &count, QList<QPoint> &to);

    /*
     * Searches surrounding of the pixel
     * in order to find the next pixel on path
     *
     * @param:  current - checked pixel
     *          out foundMinutiae - minutiae found
     * @return: list of next pixels on path or null if there are no such pixels
     */
    QList<QPoint> findNextPixel(QPoint current, bool &foundMinutiae);

    /*
     * Searches the line of a fingerprint in order to find minutiaes
     *
     * @param: x, y - coordinates of the pixel
     *         color - color of rectangle
     *         end - type of minutiae
     */
    void searchLine(int x, int y);

    /*
     * Main function whitch searches minutiae in the image
     *
     * @param: color - color of rextangle
     * @param: end - type of searched minutiae
     */
    void searchMinutiae();

    /*
     * @K3M Algorithm
     * Calculates the weight of the pixel
     *
     * @param:  x, y - coordinates of the pixel
     * @return: calculated weight
     */
    int weight(int x, int y);

    /*
     * @K3M Algorithm
     * Checkes the array used in algorithm contiains calculated weitght
     *
     * @param:  weight - weight of the pixel we want to check
     * @return: true if the table contains the value, false otherwise
     */
    bool checkArray(int weight, int phase);

    /*
     * @K3M Algorithm
     * Thines the image to 1 pixel widht
     *
     * @param: img - image to thin
     *         phase - number of phase
     */
    void thinImage(QImage *img, int phase);

    /*
     * Negate the image
     */
    void negateImage();

    /*
     * Estimates real matched minutiaes (the ones with
     * more than one neighbour
     *
     * @param:  matchedMinutiaes - list of found matched minutiaes
     * @return: number of matched vertices
     */
    int verifyMatchedMinutiaesGraph(QList<Minutiae *> matchedMinutiaes);

    /*
     * Checkes wheter two minutiaes from two different images
     * match by looking throught their neighbours
     *
     * @param:  m, n - minutiaes of two images to verify
     *          out toVerification1, toVerification2 - list of minutiaes to verification
     * @return: true if the minutiaes match,false otherwise
     */
    bool verify(Minutiae *m, Minutiae *n, QList<Minutiae *> *toVerification1, QList<Minutiae*> *toVerification2);

    /*
     * Makes k-graph of found minutiaes
     * The neighbours are the vertices closest to the minutiae
     */
    void makeGraph();


private slots:
    void on_loadButton_clicked();
    void on_loadButton2_clicked();
    void on_findButton_clicked();
    void on_findButton2_clicked();
    void on_checkButton_clicked();
};

#endif // MAINWINDOW_H
