#include "dbhelper.h"
#include <QDebug>

DatabaseHelper::DatabaseHelper(QSqlDatabase &_db)
{
    db = _db;
}

void DatabaseHelper::setDatabase(QSqlDatabase& _db)
{
    db = _db;
}

void DatabaseHelper::createTables()
{
    QSqlQuery qry(db);

    /* Create Image table */
    if(!qry.prepare( "CREATE TABLE IF NOT EXISTS Image "
                 "(id INTEGER UNIQUE PRIMARY KEY, "
                 "image BLOB, "
                 "name VARCHAR(50))"))
    {
        qDebug() << qry.lastError();
    }
    if( !qry.exec() )
      qDebug() << qry.lastError();

    /* Create Minutiae table * /
    if(!qry.prepare( "CREATE TABLE IF NOT EXISTS Minutiae "
                 "(id INTEGER UNIQUE PRIMARY KEY, "
                 "x INTEGER, "
                 "y INTEGER, "
                 "type BOOLEAN, "
                 "indeks INTEGER)" ))
    {
        qDebug() << qry.lastError();
    }
    if( !qry.exec() )
      qDebug() << qry.lastError();


    /* Create Minutiae_Minutiae table * /
    if(!qry.prepare( "CREATE TABLE IF NOT EXISTS Minutiae_Minutiae "
                 "(id INTEGER UNIQUE PRIMARY KEY, "
                 "minutiae1_id INTEGER NOT NULL, "
                 "minutiae2_id INTEGER NOT NULL, "
                 "FOREIGN KEY(minutiae1_id) REFERENCES Minutiae(id), "
                 "FOREIGN KEY(minutiae2_id) REFERENCES Minutiae(id))"))
    {
        qDebug() << qry.lastError();
    }
    if( !qry.exec() )
      qDebug() << qry.lastError();


    /* Create Image_Minutiae table * /
    if (!qry.prepare( "CREATE TABLE IF NOT EXISTS Image_Minutiae "
                 "(id INTEGER UNIQUE PRIMARY KEY, "
                 "image_id INTEGER NOT NULL, "
                 "minutiae_id INTEGER NOT NULL, "
                 "FOREIGN KEY(image_id) REFERENCES Image(id), "
                 "FOREIGN KEY(minutiae_id) REFERENCES Minutiae(id))"))
    {
        qDebug() << qry.lastError();
    }
    if( !qry.exec() )
      qDebug() << qry.lastError();*/
}

int DatabaseHelper::insertMinutiae(int image_id, int x, int y, bool type, int indeks)
{
    QSqlQuery qry(db);
    int id;

    if(!qry.prepare("INSERT INTO Minutiae (x, y, type, indeks) "
                   "VALUES (:x, :y, :type, :indeks)"))
    {
        qDebug() << qry.lastError();
    }
    qry.bindValue(":x", x);
    qry.bindValue(":y", y);
    qry.bindValue(":type", type);
    qry.bindValue(":indeks", indeks);

    if( !qry.exec() )
      qDebug() << qry.lastError();

    id = qry.lastInsertId().toInt();
    insertImageMinutiae(image_id, id);

    return id;
}

int DatabaseHelper::insertImage(QString filePath, QString name)
{
    QSqlQuery qry(db);

    QFile file(filePath);
    if (name.isEmpty())
    {
        QFileInfo fileInfo(file.fileName());
        name = fileInfo.fileName();
    }
    if (!file.open(QIODevice::ReadOnly)) return -1;
    QByteArray byteArray = file.readAll();

    if(!qry.prepare("INSERT INTO Image (image, name) "
                     "VALUES (:image, :name)"))
    {
        qDebug() << qry.lastError();
    }

    qry.bindValue(":image", byteArray);
    qry.bindValue(":name", name);

    if( !qry.exec() )
      qDebug() << qry.lastError();

    return qry.lastInsertId().toInt();
}

int DatabaseHelper::insertImageMinutiae(int image_id, int minutiae_id)
{
    QSqlQuery qry(db);

    if(!qry.prepare("INSERT INTO Image_Minutiae (image_id, minutiae_id) "
                   "VALUES (:image_id, :minutiae_id)"))
    {
        qDebug() << qry.lastError();
    }
    qry.bindValue(":image_id", image_id);
    qry.bindValue(":minutiae_id", minutiae_id);

    if( !qry.exec() )
      qDebug() << qry.lastError();

    return qry.lastInsertId().toInt();
}

int DatabaseHelper::insertMinutiaeMinutiae(int minutiae1_id, int minutiae2_id)
{
    QSqlQuery qry(db);

    if(!qry.prepare("INSERT INTO Minutiae_Minutiae (minutiae1_id, minutiae2_id) "
                   "VALUES (:minutiae1_id, :minutiae2_id)"))
    {
        qDebug() << qry.lastError();
    }

    qry.bindValue(":minutiae1_id", minutiae1_id);
    qry.bindValue(":minutiae2_id", minutiae2_id);

    if( !qry.exec() )
      qDebug() << qry.lastError();

    return qry.lastInsertId().toInt();
}

QList<QByteArray> DatabaseHelper::selectImages(QList<QString>& names)
{
    QSqlQuery qry(db);
    QList<QByteArray> list;
    names.clear();

    qry.prepare( "SELECT * FROM Image" );
    if( !qry.exec() )
      qDebug() << qry.lastError();
    else
    {
      for( int r=0; qry.next(); ++r)
      {
          list.push_back(qry.value(1).toByteArray());
          names.push_back(qry.value(2).toString());
      }
    }

    return list;
}
