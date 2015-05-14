#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "k3mtables.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>

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
            delete checkedPixels[0][i];
            delete K3MImageArray[0][i];
        }
        delete[] checkedPixels[0];
        delete[] K3MImageArray[0];
    }

    if (!image[1].isNull())
    {
        for (int i = 0; i < image[1].width(); ++i)
        {
            delete checkedPixels[1][i];
            delete K3MImageArray[1][i];
        }
        delete[] checkedPixels[1];
        delete[] K3MImageArray[1];
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

void MainWindow::drawRectangle(QPoint point)
{
    if (checkIfAtBorder(point.x(), point.y()))
        return;

    int length = 4;

    /* Check and find correct bifurcation */
    if (!isBlack(image[INDEX].pixel(point.x(), point.y())))
    {
        for (int i = -length; i <= length; ++i)
        {
            for (int j = -length; j <= length; ++j)
            {
                if (isBlack(image[INDEX].pixel(point.x() + i, point.y() + j)))
                {
                    if (isBifurcation(point.x() + i, point.y() + j))
                    {
                        QPoint p(point.x() + i, point.y() +j);
                        drawRectangle(p);
                        return;
                    }
                }
            }
        }
        return;
    }

    /* Draw rectangle */
    qStdOut() << "Entered drawRectangle(" << point.x() << ", " << point.y() << ")" << endl;

    if (point.y() - length >= 0 && point.x() - length >= 0 && point.x() + length < image[INDEX].width())
    {
        for (int i = -length; i <= length; ++i)
            image[INDEX].setPixel(point.x() + i, point.y() - 5, COLOR.rgb());
    }
    if (point.x() - length >= 0 && point.y() - length >= 0 && point.y() + length < image[INDEX].height())
    {
        for (int i = -length; i <= length; ++i)
            image[INDEX].setPixel(point.x() - length, point.y() + i, COLOR.rgb());
    }
    if (point.y() + length < image[INDEX].height() && point.x() - length >= 0 && point.x() + length < image[INDEX].width())
    {
        for (int i = -length; i <= length; ++i)
            image[INDEX].setPixel(point.x() + i, point.y() + length, COLOR.rgb());
    }
    if (point.x() + length < image[INDEX].width() && point.y() - length >= 0 && point.y() + length < image[INDEX].height())
    {
        for (int i = -length; i <= length; ++i)
            image[INDEX].setPixel(point.x() + length, point.y() + i, COLOR.rgb());
    }
    image[INDEX].setPixel(point.x(), point.y(), qRgb(255, 0, 0));

    /* Add minutiae to list of found minutiaes */
    minutiaes[INDEX].push_back(new Minutiae(point.x(), point.y(), TYPE, minutiaes[INDEX].count()));
}

bool MainWindow::checkIfAtBorder(int x, int y)
{
    bool ret = true;

    /* Vertical check "up"*/
    for (int i = x - 1; i > 0; --i)
    {
        if (isBlack(image[INDEX].pixel(i, y)))
        {
            ret = false;
            break;
        }
    }

    if (ret)
        return true;

    ret = true;

    /* Vertical check "down" */
    for (int i = x + 1; i < image[INDEX].width(); ++i)
    {
        if (isBlack(image[INDEX].pixel(i, y)))
        {
            ret = false;
            break;
        }
    }

    if (ret)
        return true;

    ret = true;

    /* Horizontal check "left" */
    for (int i = y - 1; i > 0; --i)
    {
        if (isBlack(image[INDEX].pixel(x, i)))
        {
            ret = false;
            break;
        }
    }

    if (ret)
        return true;

    ret = true;

    /* Horizontal check "right" */
    for (int i = y + 1; i < image[INDEX].height(); ++i)
    {
        if (isBlack(image[INDEX].pixel(x, i)))
        {
            ret = false;
            break;
        }
    }

    return ret;
}

bool MainWindow::isBifurcation(int x, int y)
{
    bool ret = false;
    int count = 0;
    int coords[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};

    for (int i = 0; i < 4; ++i)
    {
        if (isBlack(image[INDEX].pixel(x + coords[i][0], y + coords[i][1])))
            ++count;
    }

    if (count >= 3)
        ret = true;

    return ret;
}

void MainWindow::reduceCount(bool directions[], int &count)
{
    if (directions[1] && directions[0])
        --count;
    if (directions[1] && directions[2])
        --count;
    if (directions[3] && directions[0])
        --count;
    if (directions[3] && directions[5])
        --count;
    if (directions[4] && directions[2])
        --count;
    if (directions[4] && directions[7])
        --count;
    if (directions[6] && directions[5])
        --count;
    if (directions[6] && directions[7])
        --count;
}

bool MainWindow::isPixelOnPath(int x, int y, int &count, QList<QPoint> &to)
{
    if (x < 0 || x >= imageCopy[INDEX].width() || y < 0 || y >= imageCopy[INDEX].height())
        return false;

    QRgb rgb = imageCopy[INDEX].pixel(x, y);

    if (isBlack(rgb))
    {
        ++count;

        if (checkedPixels[INDEX][x][y])
            return true;

        checkedPixels[INDEX][x][y] = true;
        to.push_back(QPoint(x, y));
        return true;
    }

    return false;
}

QList<QPoint> MainWindow::findNextPixel(QPoint current, bool &foundMinutiae)
{
    QList<QPoint> to;
    int count = 0;
    bool directions[8];

    for (int i = 0; i < 8; ++i)
        directions[i] = false;

    int coords[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

    for (int i = 0; i < 8; ++i)
    {
        if(isPixelOnPath(current.x() + coords[i][0], current.y() + coords[i][1], count, to))
            directions[i] = true;
    }

    reduceCount(directions, count);

    if (count == 1)
        foundMinutiae = true;
    return to;
}

void MainWindow::searchLine(int x, int y)
{
    qStdOut() << "searchLine(" << x << ", " << y << ")" << endl;

    QPoint current(x, y);
    QList<QPoint> toList;
    QList<QPoint> possibleMinutiaes;
    int count = 0;
    bool foundMinutiae = false;

    checkedPixels[INDEX][x][y] = true;

    toList = findNextPixel(current, foundMinutiae);
    if (foundMinutiae)
        possibleMinutiaes.push_back(current);

    while (!toList.isEmpty())
    {
        current = toList.takeLast();

        foundMinutiae = false;
        QList<QPoint> tmpList = findNextPixel(current, foundMinutiae);

        for (int i = 0; i < tmpList.count(); ++i)
            toList.push_back(tmpList[i]);

        if (foundMinutiae)
            possibleMinutiaes.push_back(current);

        ++count;
    }

    if (count > 10)
    {
        for (int i = 0; i < possibleMinutiaes.count(); ++i)
            drawRectangle(possibleMinutiaes[i]);
    }
}

void MainWindow::searchMinutiae()
{
    for (int x = 0; x < imageCopy[INDEX].width(); ++x)
    {
        for (int y = 0; y < imageCopy[INDEX].height(); ++y)
        {
           if (checkedPixels[INDEX][x][y])
               continue;

           QRgb rgb = imageCopy[INDEX].pixel(x, y);

           if (isBlack(rgb))
               searchLine(x, y);
        }
    }
}

int MainWindow::weight(int x, int y)
{
    return 1*(K3MImageArray[INDEX][x][y-1] % 2) + 2*(K3MImageArray[INDEX][x+1][y-1] % 2)+
    64*(K3MImageArray[INDEX][x-1][y] % 2) + 4*(K3MImageArray[INDEX][x+1][y] % 2)+
    32*(K3MImageArray[INDEX][x-1][y+1] % 2) + 16*(K3MImageArray[INDEX][x][y+1] % 2)+
    8*(K3MImageArray[INDEX][x+1][y+1] % 2) + 128*(K3MImageArray[INDEX][x-1][y-1] % 2);
}

bool MainWindow::checkArray(int weight, int phase)
{
    for (int i = 1; i < k3mTables::A[phase][0]; ++i)
    {
        if (weight == k3mTables::A[phase][i])
            return  true;
    }

    return false;
}

void MainWindow::thinImage(QImage *img, int phase)
{
    /* Mark image pixels as 1 and background as 0 */
    for (int x = 0; x < img->width(); ++x)
    {
        for (int y = 0; y < img->height(); ++y)
        {
            if (isBlack(img->pixel(x, y)))
                K3MImageArray[INDEX][x][y] = 1;
            else
                K3MImageArray[INDEX][x][y] = 0;
        }
    }

    /* Thin */
    bool thinned;
    do
    {
       thinned = true;
       for (int x = 1; x < img->width() - 1; ++x)
       {
           for (int y = 1; y < img->height() - 1; ++y)
           {
               if (K3MImageArray[INDEX][x][y] == 1)
               {
                   if (checkArray(weight(x, y), 0))
                   {
                       int w = weight(x, y);

                       if (checkArray(w, phase))
                       {
                           K3MImageArray[INDEX][x][y] = 0;
                           thinned = false;
                       }
                   }
               }
           }
       }
    }while (!thinned);

    /* Mark even pixels as background and odd as image */
    for (int x = 0; x < img->width(); ++x)
    {
        for (int y = 0; y < img->height(); ++y)
        {
            if (K3MImageArray[INDEX][x][y] == 1)
                img->setPixel(x, y, qRgb(0, 0 ,0));
            else
                img->setPixel(x, y, qRgb(255, 255, 255));
        }
    }
}

void MainWindow::negateImage()
{
    for (int x = 0; x < imageCopy[INDEX].width(); ++x)
    {
        for (int y = 0; y < imageCopy[INDEX].height(); ++ y)
        {
            QRgb rgb = imageCopy[INDEX].pixel(x, y);
            if (isBlack(rgb))
                imageCopy[INDEX].setPixel(x, y, qRgb(255, 255, 255));
            else
                imageCopy[INDEX].setPixel(x, y, qRgb(0, 0 ,0));
        }
    }

    for (int i = 1; i < 7; ++i)
        thinImage(&imageCopy[INDEX], i);
}

int MainWindow::verifyMatchedMinutiaesGraph(QList<Minutiae *> matchedMinutiaes)
{
    int goodVertices = matchedMinutiaes.count() + 1;

    for (int i = 0; i < matchedMinutiaes.count(); ++i)
    {
        int count = 0;
        bool good = false;
        for (int j = 0; j < matchedMinutiaes[i]->neighbours.count(); ++j)
        {
            for(int k = 0; k < matchedMinutiaes.count(); ++k)
            {
                if (k == i)
                    continue;
                if (matchedMinutiaes[i]->neighbours[j] == matchedMinutiaes[k])
                    ++count;
                if (count > 1)
                {
                    good = true;
                    break;
                }
            }
            if (good)
                break;
        }
        if (count < 2)
            --goodVertices;
    }

    return goodVertices;
}

bool MainWindow::verify(Minutiae *m, Minutiae *n, QList<Minutiae *> *toVerification1, QList<Minutiae *> *toVerification2)
{
    bool match = false;
    bool next;

    for (int k = 0; k < m->neighbours.count(); ++k)
    {
        Minutiae *m1 = m->neighbours[k];

        for (int l = 0; l < n->neighbours.count(); ++l)
        {
            next = false;
            Minutiae *n1 = n->neighbours[l];

            if (m1->type != n1->type)
                continue;
            if (abs(m->distance(m1) - n->distance(n1)) < 2)
            {
                if (abs(m1->x - n1->x) < 15 && abs(m1->y - n1->y) < 15)
                {
                    match = true;
                    next = true;

                    if (!m1->checked && !toVerification1->contains(m1))
                    {
                        toVerification1->push_back(m1);
                        toVerification2->push_back(n1);
                    }
                }
            }
            if (next)
                break;
        }
    }

    return match;
}

void MainWindow::makeGraph()
{
    for (int i = 0; i < minutiaes[INDEX].count(); ++i)
    {
        int max = 0;
        int max_index = 0;
        for (int j = 0, k = 0; j < 10; ++j, ++k)
        {
            if (i == j)
                ++k;
            minutiaes[INDEX][i]->neighbours.push_back(minutiaes[INDEX][k]);

            if (max < minutiaes[INDEX][i]->distance(minutiaes[INDEX][k]))
            {
                max = minutiaes[INDEX][i]->distance(minutiaes[INDEX][k]);
                max_index = k;
            }
        }

        for (int j = 10; j < minutiaes[INDEX].count(); ++j)
        {
            if (j == i)
                continue;
            if (max <= minutiaes[INDEX][i]->distance(minutiaes[INDEX][j]))
                continue;

            max = minutiaes[INDEX][i]->distance(minutiaes[INDEX][j]);
            minutiaes[INDEX][i]->neighbours.removeAt(max_index);
            minutiaes[INDEX][i]->neighbours.push_back(minutiaes[INDEX][j]);
            max_index = 9;

            for (int k = 0; k < 9; ++k)
            {
                if (max < minutiaes[INDEX][i]->distance(minutiaes[INDEX][i]->neighbours[k]))
                {
                    max = minutiaes[INDEX][i]->distance(minutiaes[INDEX][i]->neighbours[k]);
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

        checkedPixels[0] = new bool*[image[0].width()];
        for (int i = 0; i < image[0].width(); ++i)
            checkedPixels[0][i] = new bool[image[0].height()];

        for (int i = 0; i < image[0].width(); ++i)
            for (int j = 0; j < image[0].height(); ++j)
                checkedPixels[0][i][j] = false;

        K3MImageArray[0] = new int*[image[0].width()];

        for (int i = 0; i < image[0].width(); ++i)
            K3MImageArray[0][i] = new int[image[0].height()];

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

        checkedPixels[1] = new bool*[image[1].width()];
        for (int i = 0; i < image[1].width(); ++i)
            checkedPixels[1][i] = new bool[image[1].height()];

        for (int i = 0; i < image[1].width(); ++i)
            for (int j = 0; j < image[1].height(); ++j)
                checkedPixels[1][i][j] = false;

        K3MImageArray[1] = new int*[image[1].width()];

        for (int i = 0; i < image[1].width(); ++i)
            K3MImageArray[1][i] = new int[image[1].height()];

        lastFileName = filename;
    }
}

void MainWindow::on_findButton_clicked()
{
    INDEX = 0;
    COLOR = QColor(0, 0, 255);
    TYPE = true;

    for (int i = 0; i < image[INDEX].width(); ++i)
        for (int j = 0; j < image[INDEX].height(); ++j)
            checkedPixels[INDEX][i][j] = false;

    minutiaes[INDEX].clear();

    for (int i = 1; i < 7; ++i)
        thinImage(&image[INDEX], i);

    imageCopy[INDEX] = image[INDEX];

    searchMinutiae();

    negateImage();

    ui->imageView->setPixmap(QPixmap::fromImage(imageCopy[INDEX]));
    QString str = "negateImage.png";
    imageCopy[INDEX].save(str, "PNG");

    COLOR = QColor(255, 0, 0);
    TYPE = false;
    searchMinutiae();

    makeGraph();

    ui->imageView->setPixmap(QPixmap::fromImage(image[INDEX]));
    str = "outImage1.png";
    image[INDEX].save(str, "PNG");

    qStdOut() << endl;
    qStdOut() << "-----------------STATUS------------------" << endl;
    qStdOut() << "              Found Minuteas: " << minutiaes[INDEX].count() << endl;
    qStdOut() << "-----------------------------------------" << endl;
}

void MainWindow::on_findButton2_clicked()
{
    INDEX = 1;
    COLOR = QColor(0, 0, 255);
    TYPE = true;

    for (int i = 0; i < image[INDEX].width(); ++i)
        for (int j = 0; j < image[INDEX].height(); ++j)
            checkedPixels[INDEX][i][j] = false;

    minutiaes[INDEX].clear();

    for (int i = 1; i < 7; ++i)
        thinImage(&image[INDEX], i);

    imageCopy[INDEX] = image[INDEX];

    searchMinutiae();

    negateImage();

    ui->imageView2->setPixmap(QPixmap::fromImage(imageCopy[INDEX]));
    QString str = "negateImage2.png";
    imageCopy[INDEX].save(str, "PNG");

    COLOR = QColor(255, 0, 0);
    TYPE = false;
    searchMinutiae();

    makeGraph();

    ui->imageView2->setPixmap(QPixmap::fromImage(image[INDEX]));
    str = "outImage2.png";
    image[INDEX].save(str, "PNG");

    qStdOut() << endl;
    qStdOut() << "-----------------STATUS------------------" << endl;
    qStdOut() << "              Found Minuteas: " << minutiaes[INDEX].count() << endl;
    qStdOut() << "-----------------------------------------" << endl;
}

void MainWindow::on_checkButton_clicked()
{
    bool match;
    QList<Minutiae*> toVerification1;
    QList<Minutiae*> toVerification2;
    QList<Minutiae*> matchedMinutiaes;
    int goodVertices = 0;
    int maxFound = 0;

    for (int i = 0; i < minutiaes[0].count(); ++i)
    {
        Minutiae *m = minutiaes[0][i];
        if (m->checked)
            continue;

        for (int j = 0; j < minutiaes[1].count(); ++j)
        {
            match = false;
            Minutiae *n = minutiaes[1][j];
            if (n->checked)
                continue;

            if (m->type != n->type)
                continue;
            if (abs(m->x - n->x) > 15 || abs(m->y - n->y) > 15)
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

                match = verify(m, n, &toVerification1, &toVerification2);

                if (match)
                {
                    ++goodVertices;
                    if (!m->checked)
                    {
                        m->checked = true;
                        n->checked = true;
                        matchedMinutiaes.push_back(m);
                    }

                }
            }

            goodVertices = verifyMatchedMinutiaesGraph(matchedMinutiaes);
            int minMatched = ui->matchedVertices->toPlainText().toInt();
            double minTreshold = ui->treshold->toPlainText().toDouble();

            if (goodVertices >= minMatched || (double)goodVertices/(double)minutiaes[0].count() * 100 >= minTreshold)
            {
                double percent = (double)goodVertices/(double)minutiaes[0].count() * 100;
                QMessageBox::information(this, tr("Match confirmed!"),
                                         tr("Matched %1 vertices which makes %2% of compatibility!").arg(goodVertices).arg(percent));
                return;
            }
            else
            {
                if (maxFound < goodVertices)
                    maxFound = goodVertices;
                goodVertices = 0;
                for (int k = 0; k < matchedMinutiaes.count(); ++k)
                    matchedMinutiaes[k]->checked = false;
                matchedMinutiaes.clear();
            }
        }
    }
    QMessageBox::warning(this, tr("No match"), tr("Fingerprints not matched\n Matched vertices: %1 which makes %2% of compatibility")
                         .arg(maxFound).arg((double)maxFound/(double)minutiaes[0].count() * 100));
}
