#ifndef DBHELPER_H
#define DBHELPER_H

#include <QtSql>

class DatabaseHelper
{
private:
    QSqlDatabase db;

public:
    /*
     * ctors
     */
    DatabaseHelper() { }
    DatabaseHelper(QSqlDatabase& _db);

    /*
     * Sets database for executing queries
     */
    void setDatabase (QSqlDatabase& _db);

    /*
     * Creates database tables
     */
    void createTables();

    /*
     * Inserts record into Minutiae table
     * @param:  x, y - coordinates of minutiae
     *          type - type of minutiae
     *          indeks - index of minutiae
     * @return: id of inserted record
     */
    int insertMinutiae(int image_id, int x, int y, bool type, int indeks);

    /*
     * Inserts record into Image table
     * @param:  filePath - path to file
     *          name - name of the file
     *                 if not given default fileName without a path is set
     * @return: id of inserted record
     */
    int insertImage(QString filePath, QString name = "");

    /*
     * Inserts record into Image_Minutiae table
     * @param:  image_id - id of image record
     *          minutiae_id - id of minutiae record
     * @return: id of inserted record
     */
    int insertImageMinutiae(int image_id, int minutiae_id);

    /*
     * Inserts record into Minutiae_Minutiae table
     * @param:  minutiae1_id - id of first minutiae
     *          minutiae2_id - id of second minutiae
     * @return: id of inserted record
     */
    int insertMinutiaeMinutiae(int minutiae1_id, int minutiae2_id);

    /*
     * Selects all records from Image table
     * @param:  out names - list of selected names
     * @return: list of images in byteArray format
     */
    QList<QByteArray> selectImages(QList<QString>& names);
};

#endif // DBHELPER_H

