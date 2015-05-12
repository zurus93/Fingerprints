#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>

int MainWindow::A1px[] = {49, 3, 6, 7, 12, 14, 15, 24, 28, 30, 31, 48, 56,
                          60, 62, 63, 96, 112, 120, 124, 126, 127, 129, 131,
                          135, 143, 159, 191, 192, 193, 195, 199, 207, 223,
                          224, 225, 227, 231, 239, 240, 241, 243, 247, 248,
                          249, 251, 252, 253, 254};

int MainWindow::A0[] = {49, 3, 6, 7, 12, 14, 15, 24, 28, 30, 31, 48, 56, 60,
        62, 63, 96, 112, 120, 124, 126, 127, 129, 131, 135,
        143, 159, 191, 192, 193, 195, 199, 207, 223, 224,
        225, 227, 231, 239, 240, 241, 243, 247, 248, 249,
        251, 252, 253, 254};

int MainWindow::A1[] = {9, 7, 14, 28, 56, 112, 131, 193, 224};

int MainWindow::A2[] = {17, 7, 14, 15, 28, 30, 56, 60, 112, 120, 131, 135,
                        193, 195, 224, 225, 240};

int MainWindow::A3[] = {25, 7, 14, 15, 28, 30, 31, 56, 60, 62, 112, 120,
                        124, 131, 135, 143, 193, 195, 199, 224, 225, 227,
                        240, 241, 248};

int MainWindow::A4[] = {33, 7, 14, 15, 28, 30, 31, 56, 60, 62, 63, 112, 120,
        124, 126, 131, 135, 143, 159, 193, 195, 199, 207,
        224, 225, 227, 231, 240, 241, 243, 248, 249, 252};

int MainWindow::A5[] = {37, 7, 14, 15, 28, 30, 31, 56, 60, 62, 63, 112, 120,
        124, 126, 131, 135, 143, 159, 191, 193, 195, 199,
        207, 224, 225, 227, 231, 239, 240, 241, 243, 248,
        249, 251, 252, 254};

int* MainWindow::A[7] = {A0, A1, A2, A3, A4, A5, A1px};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lastFileName = ".";
}

MainWindow::~MainWindow()
{
    if (!image[0].isNull())
    {
        for (int i = 0; i < image[0].width(); ++i)
        {
            delete pixels[0][i];
        }
        delete[] pixels[0];

        for (int i = 0; i < image[0].width(); ++i)
            delete imageArray[0][i];
        delete[] imageArray[0];
    }

    if (!image[1].isNull())
    {
        for (int i = 0; i < image[1].width(); ++i)
        {
            delete pixels[1][i];
        }
        delete[] pixels[1];

        for (int i = 0; i < image[1].width(); ++i)
            delete imageArray[1][i];
        delete[] imageArray[1];
    }

    delete ui;
}

QTextStream& MainWindow::qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

bool MainWindow::isBlack(QRgb rgb)
{

    bool ret = false;

    if (qRed(rgb) == 0 && qBlue(rgb) == 0 && qGreen(rgb) == 0)
        ret = true;

    return ret;
}

bool MainWindow::checkPixel(QPoint from, int x, int y, int index)
{
    QRgb rgb = imageCopy[index].pixel(x, y);

    if (isBlack(rgb))
    {
        if (x == from.x() && y == from.y() || pixels[index][x][y])
            return false;
        qStdOut() << "FOUND pixel on path: (" << from.x() << "," << from.y() << ") --> (" << x << "," << y << ")" << endl;
        pixels[index][x][y] = true;
        return true;
    }

    return false;
}

QPoint MainWindow::findMinutia(QPoint from, QPoint current, bool &begining, int index)
{
    qStdOut() << "Entered findMinutia()" << endl;
    qStdOut() << "(" << from.x() << "," << from.y() << ") --> (" << current.x() << "," << current.y() << ")" << endl;

    QPoint to(-1, -1);
    bool check;
    int count = 0;

    if (current.y() - 1 >= 0)
    {
        check = checkPixel(from, current.x(), current.y() - 1, index);
        if (check)
        {
            to.setX(current.x());
            to.setY(current.y() - 1);
            ++count;
        }
    }
    if (current.y() - 1 >= 0 && current.x() - 1 >= 0)
    {
        check = checkPixel(from, current.x() - 1, current.y() - 1, index);
        if (check)
        {
            to.setX(current.x() - 1);
            to.setY(current.y() - 1);
            ++count;
        }
    }
    if (current.y() - 1 >= 0 && current.x() + 1 < imageCopy[index].width())
    {
        check = checkPixel(from, current.x() + 1, current.y() - 1, index);
        if (check)
        {
            to.setX(current.x() + 1);
            to.setY(current.y() - 1);
            ++count;
        }
    }
    if (current.x() - 1 >= 0)
    {
        check = checkPixel(from, current.x() - 1, current.y(), index);
        if (check)
        {
            to.setX(current.x() - 1);
            to.setY(current.y());
            ++count;
        }
    }
    if (current.x() + 1 < imageCopy[index].width())
    {
        check = checkPixel(from, current.x() + 1, current.y(), index);
        if (check)
        {
            to.setX(current.x() + 1);
            to.setY(current.y());
            ++count;
        }
    }
    if (current.y() + 1 < imageCopy[index].height(), index)
    {
        check = checkPixel(from, current.x(), current.y() + 1, index);
        if (check)
        {
            to.setX(current.x());
            to.setY(current.y() + 1);
            ++count;
        }
    }
    if (current.y() + 1 < imageCopy[index].height() && current.x() - 1 >= 0)
    {
        check = checkPixel(from, current.x() - 1, current.y() + 1, index);
        if (check)
        {
            to.setX(current.x() - 1);
            to.setY(current.y() + 1);
            ++count;
        }
    }
    if (current.y() + 1 < imageCopy[index].height() && current.x() + 1 < imageCopy[index].width())
    {
        check = checkPixel(from, current.x() + 1, current.y() + 1, index);
        if (check)
        {
            to.setX(current.x() + 1);
            to.setY(current.y() + 1);
            ++count;
        }
    }

    if (count == 1)
        begining = true;
    return to;
}

bool MainWindow::checkRectangle(int x, int y, int index)
{
    bool ret = true;
    int count = 0;

    if (isBlack(imageCopy[index].pixel(x, y - 1)))
        count++;
    if (isBlack(imageCopy[index].pixel(x - 1, y - 1)))
        count++;
    if (isBlack(imageCopy[index].pixel(x + 1, y - 1)))
        count++;
    if (isBlack(imageCopy[index].pixel(x - 1, y)))
        count++;
    if (isBlack(imageCopy[index].pixel(x+1, y)))
        count++;
    if (isBlack(imageCopy[index].pixel(x - 1, y + 1)))
        count++;
    if (isBlack(imageCopy[index].pixel(x, y + 1)))
        count++;
    if (isBlack(imageCopy[index].pixel(x+ 1, y + 1)))
        count++;

    if (count > 1)
        ret = false;

    return ret;
}

bool MainWindow::checkRectangleImage(int x, int y, int index)
{
    bool ret = false;
    int count = 0;

    if (isBlack(image[index].pixel(x, y - 1)))
        count++;
    if (isBlack(image[index].pixel(x - 1, y)))
        count++;
    if (isBlack(image[index].pixel(x+1, y)))
        count++;
    if (isBlack(image[index].pixel(x, y + 1)))
        count++;

    if (count >= 3)
        ret = true;

    return ret;
}

bool MainWindow::checkBorder(int x, int y, int index)
{
    bool ret = true;

    for (int i = x - 1; i > 0; --i)
    {
        if (isBlack(image[index].pixel(i, y)))
        {
            ret = false;
            break;
        }
    }

    if (ret)
        return true;

    ret = true;

    for (int i = x + 1; i < image[index].width(); ++i)
    {
        if (isBlack(image[index].pixel(i, y)))
        {
            ret = false;
            break;
        }
    }

    if (ret)
        return true;

    ret = true;

    for (int i = y - 1; i > 0; --i)
    {
        if (isBlack(image[index].pixel(x, i)))
        {
            ret = false;
            break;
        }
    }

    if (ret)
        return true;

    ret = true;

    for (int i = y + 1; i < image[index].height(); ++i)
    {
        if (isBlack(image[index].pixel(x, i)))
        {
            ret = false;
            break;
        }
    }

    return ret;

}

void MainWindow::drawRectangle(QPoint point, QColor color, bool end, int index)
{
    /*int w = 0.2 * image.width();
    int h = 0.1 * image.height();
    if (point.x() < w || point.x() > image.width() - w || point.y() < h || point.y() > image.height() - h)
        return;*/


    if (checkBorder(point.x(), point.y(), index))
        return;

    if (!checkRectangle(point.x(), point.y(), index))
        return;

    int length = 4;

    if (!isBlack(image[index].pixel(point.x(), point.y())))
    {
        for (int i = -length; i <= length; ++i)
        {
            for (int j = -length; j <= length; ++j)
            {
                if (isBlack(image[index].pixel(point.x() + i, point.y() + j)))
                {
                    if (checkRectangleImage(point.x() + i, point.y() + j, index))
                    {
                        QPoint p(point.x() + i, point.y() +j);
                        drawRectangle(p, color, end, index);
                        return;
                    }
                }
            }
        }
        return;
    }

    qStdOut() << "Entered drawRectangle(" << point.x() << ", " << point.y() << ")" << endl;

    if (point.y() - length >= 0 && point.x() - length >= 0 && point.x() + length < image[index].width())
    {
        for (int i = -length; i <= length; ++i)
            image[index].setPixel(point.x() + i, point.y() - 5, color.rgb());
    }
    if (point.x() - length >= 0 && point.y() - length >= 0 && point.y() + length < image[index].height())
    {
        for (int i = -length; i <= length; ++i)
            image[index].setPixel(point.x() - length, point.y() + i, color.rgb());
    }
    if (point.y() + length < image[index].height() && point.x() - length >= 0 && point.x() + length < image[index].width())
    {
        for (int i = -length; i <= length; ++i)
            image[index].setPixel(point.x() + i, point.y() + length, color.rgb());
    }
    if (point.x() + length < image[index].width() && point.y() - length >= 0 && point.y() + length < image[index].height())
    {
        for (int i = -length; i <= length; ++i)
            image[index].setPixel(point.x() + length, point.y() + i, color.rgb());
    }

    image[index].setPixel(point.x(), point.y(), qRgb(255, 0, 0));
    minutea[index].push_back(new Minutea(point.x(), point.y(), end, minutea[index].count()));
}

void MainWindow::searchLine(int x, int y, QColor color, bool end, int index)
{
    qStdOut() << "searchLine(" << x << ", " << y << ")" << endl;

    QPoint current(x, y);
    QPoint from(-2, -2);
    QPoint to;
    QPoint currentBegining;
    int count = 0;
    bool begining = false;
    bool once = true;

    while (true)
    {
        to = findMinutia(from, current, begining, index);
        if (once && begining)
        {
            currentBegining = current;
            once = false;
        }
        if (to.x() == -1)
        {
            qStdOut() << endl << "-------FOUND MINUTIA-----------" << endl;
            qStdOut() << "COUNT: " << count << endl << endl;
            if (count > 10)
            {
                drawRectangle(current, color, end, index);

                if (begining)
                    drawRectangle(currentBegining, color, end, index);
            }
            return;
        }

        from.setX(current.x());
        from.setY(current.y());
        current.setX(to.x());
        current.setY(to.y());
        ++count;
    }
}

void MainWindow::switchImage(int index)
{
    for (int x = 0; x < imageCopy[index].width(); ++x)
    {
        for (int y = 0; y < imageCopy[index].height(); ++ y)
        {
            QRgb rgb = imageCopy[index].pixel(x, y);
            if (isBlack(rgb))
                imageCopy[index].setPixel(x, y, qRgb(255, 255, 255));
            else
                imageCopy[index].setPixel(x, y, qRgb(0, 0 ,0));
        }
    }
}

bool MainWindow::checkArray(int weight, int index)
{
    bool ret = false;

    for (int i = 1; i < A[index][0]; ++i)
    {
        if (weight == A[index][i])
        {
            ret = true;
            break;
        }
    }

    return ret;
}

int MainWindow::weight(int x, int y, int index)
{
    return 1*(imageArray[index][x][y-1] % 2) + 2*(imageArray[index][x+1][y-1] % 2)+
    64*(imageArray[index][x-1][y] % 2) + 4*(imageArray[index][x+1][y] % 2)+
    32*(imageArray[index][x-1][y+1] % 2) + 16*(imageArray[index][x][y+1] % 2)+
    8*(imageArray[index][x+1][y+1] % 2) + 128*(imageArray[index][x-1][y-1] % 2);
}

void MainWindow::thinnImage(QImage *img, int index, int ind)
{
    for (int x = 0; x < img->width(); ++x)
    {
        for (int y = 0; y < img->height(); ++y)
        {
            if (isBlack(img->pixel(x, y)))
                imageArray[ind][x][y] = 1;
            else
                imageArray[ind][x][y] = 0;
        }
    }

    bool thinned;

    do
    {
       thinned = true;
       for (int x = 1; x < img->width() - 1; ++x)
       {
           for (int y = 1; y < img->height() - 1; ++y)
           {
               if (imageArray[ind][x][y] == 1)
               {
                   if (checkArray(weight(x, y, ind), 0))
                       imageArray[ind][x][y] = 3;
               }
           }
       }

       for (int x = 0; x < img->width(); ++x)
       {
           for (int y = 0; y < img->height(); ++y)
           {
               if (imageArray[ind][x][y] == 3)
               {
                    int w = weight(x, y, ind);

                    if (checkArray(w, index))
                    {
                        imageArray[ind][x][y] = 0;
                        thinned = false;
                    }
                    else
                        imageArray[ind][x][y] = 3;
               }
           }
       }

       for (int x = 0; x < img->width(); ++x)
       {
           for (int y = 0; y < img->height(); ++y)
           {
               if (imageArray[ind][x][y] == 3)
                   imageArray[ind][x][y] = 1;
           }
       }

    }while (!thinned);

    for (int x = 0; x < img->width(); ++x)
    {
        for (int y = 0; y < img->height(); ++y)
        {
            if (imageArray[ind][x][y] == 1)
            {
                img->setPixel(x, y, qRgb(0, 0 ,0));
            }
            else
            {
                img->setPixel(x, y, qRgb(255, 255, 255));
            }
        }
    }
}

void MainWindow::negateImage(int index)
{
    switchImage(index);

    /*ui->imageView->setPixmap(QPixmap::fromImage(imageCopy[index]));
    QString str2 = "negate3.png";
    imageCopy[index].save(str2, "PNG");*/

    for (int i = 1; i < 7; ++i)
        thinnImage(&imageCopy[index], i, index);
}

void MainWindow::searchMinutia(QColor color, bool end, int index)
{
    for (int x = 0; x < imageCopy[index].width(); ++x)
    {
        for (int y = 0; y < imageCopy[index].height(); ++y)
        {
           if (pixels[index][x][y])
               continue;

           QRgb rgb = imageCopy[index].pixel(x, y);

           if (isBlack(rgb))
           {
               searchLine(x, y, color, end, index);
           }
        }
    }

    for (int i = 0; i < imageCopy[index].width(); ++i)
        for (int j = 0; j < imageCopy[index].height(); ++j)
            pixels[index][i][j] = false;

    for (int x = imageCopy[index].width() - 1; x >= 0; --x)
    {
        for (int y = imageCopy[index].height() - 1; y >= 0; --y)
        {
           if (pixels[index][x][y])
               continue;

           QRgb rgb = imageCopy[index].pixel(x, y);

           if (isBlack(rgb))
           {
               searchLine(x, y, color, end, index);
           }
        }
    }

}

void MainWindow::makeGraph(int index)
{
    for (int i = 0; i < minutea[index].count(); ++i)
    {
        int max = 0;
        int max_index = 0;
        for (int j = 0, k = 0; j < 10; ++j, ++k)
        {
            if (i == j)
                ++k;
            minutea[index][i]->neighbours.push_back(minutea[index][k]);

            if (max < minutea[index][i]->distance(minutea[index][k]))
            {
                max = minutea[index][i]->distance(minutea[index][k]);
                max_index = j;
            }
        }

        for (int j = 10; j < minutea[index].count(); ++j)
        {
            if (j == i)
                continue;
            if (max <= minutea[index][i]->distance(minutea[index][j]))
                continue;

            max = minutea[index][i]->distance(minutea[index][j]);
            minutea[index][i]->neighbours.removeAt(index);
            minutea[index][i]->neighbours.push_back(minutea[index][j]);
            max_index = 9;

            for (int k = 0; k < 9; ++k)
            {
                if (max < minutea[index][i]->distance(minutea[index][i]->neighbours[k]))
                {
                    max = minutea[index][i]->distance(minutea[index][i]->neighbours[k]);
                    max_index = k;
                }
            }
        }
    }
}

void MainWindow::on_loadButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,  tr("Open Image"), lastFileName, tr("Image Files (*.png *.jpg *.bmp)"));

    if (!filename.isEmpty())
    {
        if (!image[0].load(filename))
        {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(filename));
            return;
        }

        image[0] = image[0].convertToFormat(QImage::Format_ARGB32);

        ui->imageView->setPixmap(QPixmap::fromImage(image[0]));
        ui->imageView->setScaledContents(true);

        pixels[0] = new bool*[image[0].width()];
        for (int i = 0; i < image[0].width(); ++i)
        {
            pixels[0][i] = new bool[image[0].height()];
        }

        for (int i = 0; i < image[0].width(); ++i)
        {
            for (int j = 0; j < image[0].height(); ++j)
                pixels[0][i][j] = false;
        }

        imageArray[0] = new int*[image[0].width()];

        for (int i = 0; i < image[0].width(); ++i)
            imageArray[0][i] = new int[image[0].height()];

        lastFileName = filename;
    }
}

void MainWindow::on_loadButton2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,  tr("Open Image"), lastFileName, tr("Image Files (*.png *.jpg *.bmp)"));

    if (!filename.isEmpty())
    {
        if (!image[1].load(filename))
        {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(filename));
            return;
        }

        image[1] = image[1].convertToFormat(QImage::Format_ARGB32);

        ui->imageView2->setPixmap(QPixmap::fromImage(image[1]));
        ui->imageView2->setScaledContents(true);

        pixels[1] = new bool*[image[1].width()];
        for (int i = 0; i < image[1].width(); ++i)
        {
            pixels[1][i] = new bool[image[1].height()];
        }

        for (int i = 0; i < image[1].width(); ++i)
        {
            for (int j = 0; j < image[1].height(); ++j)
                pixels[1][i][j] = false;
        }

        imageArray[1] = new int*[image[1].width()];

        for (int i = 0; i < image[1].width(); ++i)
            imageArray[1][i] = new int[image[1].height()];

        lastFileName = filename;
    }
}

void MainWindow::on_findButton_clicked()
{
    for (int i = 0; i < image[0].width(); ++i)
    {
        for (int j = 0; j < image[0].height(); ++j)
            pixels[0][i][j] = false;
    }
    minutea[0].clear();

    for (int i = 1; i < 7; ++i)
        thinnImage(&image[0], i, 0);
    imageCopy[0] = image[0];
    searchMinutia(QColor(0, 0, 255), true, 0);
    negateImage(0);
    ui->imageView->setPixmap(QPixmap::fromImage(imageCopy[0]));
    QString str2 = "negate.png";
    imageCopy[0].save(str2, "PNG");
    searchMinutia(QColor(255, 0, 0), false, 0);
    imageCopy[0] = image[0];
    ui->imageView->setPixmap(QPixmap::fromImage(imageCopy[0]));
    QString str3 = "negate2.png";
    imageCopy[0].save(str3, "PNG");

    makeGraph(0);

    ui->imageView->setPixmap(QPixmap::fromImage(image[0]));
    QString str = "out.png";
    image[0].save(str, "PNG");

    qStdOut() << "-----------------STATUS------------------" << endl;
    qStdOut() << "              Found Minuteas: " << minutea[0].count() << endl;
    qStdOut() << "-----------------------------------------" << endl;
}

void MainWindow::on_findButton2_clicked()
{
    for (int i = 0; i < image[1].width(); ++i)
    {
        for (int j = 0; j < image[1].height(); ++j)
            pixels[1][i][j] = false;
    }
    minutea[1].clear();

    for (int i = 1; i < 7; ++i)
        thinnImage(&image[1], i, 1);
    imageCopy[1] = image[1];
    searchMinutia(QColor(0, 0, 255), true, 1);
    negateImage(1);
    ui->imageView2->setPixmap(QPixmap::fromImage(imageCopy[1]));
    QString str2 = "negate.png";
    imageCopy[1].save(str2, "PNG");
    searchMinutia(QColor(255, 0, 0), false, 1);
    imageCopy[1] = image[1];
    ui->imageView2->setPixmap(QPixmap::fromImage(imageCopy[1]));
    QString str3 = "negate2.png";
    imageCopy[1].save(str3, "PNG");

    makeGraph(1);

    ui->imageView2->setPixmap(QPixmap::fromImage(image[1]));
    QString str = "out.png";
    image[1].save(str, "PNG");

    qStdOut() << "-----------------STATUS------------------" << endl;
    qStdOut() << "              Found Minuteas: " << minutea[1].count() << endl;
    qStdOut() << "-----------------------------------------" << endl;
}

bool MainWindow::verify(Minutea *m, Minutea *n, QList<Minutea *> *toVerification1, QList<Minutea *> *toVerification2)
{
    bool match = false;
    for (int k = 0; k < m->neighbours.count(); ++k)
    {
        Minutea *m1 = m->neighbours[k];
        if (m1->checked)
            continue;
        for (int l = 0; l < n->neighbours.count(); ++l)
        {
            Minutea *n1 = n->neighbours[l];
            if (n1->checked)
                continue;
            if (m1->type != n1->type)
                continue;
            if (abs(m->distance(m1) - n->distance(n1)) < 5)
            {
                match = true;

                toVerification1->push_back(m->neighbours[k]);
                toVerification2->push_back(n->neighbours[l]);
            }
        }
    }

    return match;
}

void MainWindow::on_checkButton_clicked()
{
    bool match, match2;
    QList<Minutea*> toVerification1;
    QList<Minutea*> toVerification2;
    int goodVertices = 0;

    for (int i = 0; i < minutea[0].count(); ++i)
    {
        Minutea *m = minutea[0][i];
        if (m->checked)
            continue;

        for (int j = 0; j < minutea[1].count(); ++j)
        {
            match = false;
            Minutea *n = minutea[1][j];
            if (n->checked)
                continue;

            if (m->type != n->type)
                continue;
            if (abs(m->x - n->x) > 5 || abs(m->y - n->y) > 5)
                continue;

            match = verify(m, n, &toVerification1, &toVerification2);

            if (match && !m->checked)
            {
                m->checked = true;
                n->checked = true;
                ++goodVertices;
            }

            while (!toVerification1.empty())
            {
                m = toVerification1.takeFirst();
                n = toVerification2.takeFirst();

                match2 = verify(m, n, &toVerification1, &toVerification2);

                if (match2 && !m->checked)
                {
                    m->checked = true;
                    n->checked = true;
                    ++goodVertices;
                }

            }
        }
    }

    qStdOut() << "goodVertices = " << goodVertices << endl;
}
















