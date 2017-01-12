/*!
  * \brief Threadpool testing app.
  *
  * \details First command line param should be path to folder with images.
  * Second (optional) parameter is pool size. Default pool size = 10.
  *
*/


#include <QCoreApplication>

#include <QDir>
#include <QDirIterator>
#include <iostream>

#include "threadpool.h"

#define DEFAULT_POOL_SIZE 10

int error(std::string errMsg);

int main(int argc, char *argv[])
{
    if(argc < 2)
        return error("Invalid argument count");

    int poolSize;
    bool ok = false;
    if(argc > 2)
        poolSize = QString(argv[2]).toInt(&ok);

    poolSize = ok && (argc > 2)? poolSize: DEFAULT_POOL_SIZE;

    ThreadPool pool(poolSize);

    QDirIterator fileIterator(QString(argv[1]), QStringList() << "*.JPG", QDir::Files, QDirIterator::Subdirectories);

    bool haveFiles = false;
    while (fileIterator.hasNext()){
        auto filepath = fileIterator.next().toStdString();
        std::cout << "Send to process: " << filepath << std::endl;
        pool.processImage(filepath);
        haveFiles = true;
    }

    pool.waitWhilePoolProcessing();

    std::cout << "-------" << std::endl;

    int returnCode = 0;
    if(haveFiles)
        std::cout << "Finished!";
    else
        returnCode = error("Invalid path");

    std::cout << std::endl;

    return returnCode;
}

int error(std::string errMsg)
{
    std::cerr << errMsg << std::endl;
    return -1;
}
