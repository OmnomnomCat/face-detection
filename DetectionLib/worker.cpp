#include "worker.h"

#include <functional>
#include <memory>
#include <iostream>
#include <unordered_map>

#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "locker.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

using namespace cv;


Worker::Worker(Locker* workerLocker, Locker *finalizeLocker):
    wWorkerLocker(workerLocker),
    wFinalizeLocker(finalizeLocker),
    wAvailable(true)
{

}

Worker::~Worker()
{
    this->wait();
}

void Worker::process(std::string filepath)
{
    wWorkerLocker->decreaseCounter();
    this->wAvailable = false;
    this->wait();

    auto workFunction = tr1::bind(&threadFunction, filepath, this);
    wThread = std::thread(workFunction);
}

bool Worker::isAvailable()
{
    return wAvailable;
}

void Worker::wait()
{
    if(wThread.joinable())
        wThread.join();
}

void Worker::done()
{
    wWorkerLocker->increaseCounter();
    wFinalizeLocker->decreaseCounter();
    wAvailable = true;
}

FaceMap<Rect, pair<vector<Rect>, vector<Rect>>> threadFunction(string filepath, Worker* worker)
{
    auto cascadeLoader = [](std::string filepath) -> CascadeClassifier{
        CascadeClassifier classifier("C:/Users/DalidovichAV/Dropbox/Detection/Detection/" + filepath);
        if(classifier.empty()){
                std::cout << "Can't load classifier: "
                          << filepath<< " . "
                          << "Programm will be terminated";
                exit(-1);
        }
        else return classifier;
    };

    CascadeClassifier faceCascade = cascadeLoader("haarcascades/haarcascade_frontalface_default.xml");
    CascadeClassifier eyesCascade = cascadeLoader("haarcascades/haarcascade_eye.xml");
    CascadeClassifier mouthCascade = cascadeLoader("haarcascades/haarcascade_smile.xml");

    Mat grayImage, colorImage = imread(filepath);
    cvtColor(colorImage, grayImage, COLOR_BGR2GRAY);

    auto filenamePos = filepath.find_last_of('.');
    string filename = filepath.substr(0, filenamePos );

    vector<Rect> faces, eyes, mouthes;

    FaceMap<Rect, pair<vector<Rect>, vector<Rect>>> detectionResult;

    faceCascade.detectMultiScale(grayImage,faces, 1.3, 8);

    QJsonDocument doc;
    QJsonObject fileDesc;
    auto addRect = [](vector<Rect>& v1, Rect& add){
        for(Rect& value: v1){
            value.x += add.x;
            value.y += add.y;
        }
    };

    int fileMaskCounter = 0;
    for(Rect& face: faces){

        auto grayROI = grayImage.adjustROI(face.y, face.y + face.height, face.x, face.x + face.width);

        string outputFilename = filename + '_' + std::to_string(fileMaskCounter++) + ".jpg";

        QJsonObject posDesc;
        posDesc["x"] = face.x;
        posDesc["y"] = face.y;
        fileDesc[QString::fromStdString(outputFilename)] = posDesc;

        Mat flipImage;
        flip(colorImage(face), flipImage, 1);

        imwrite(outputFilename, flipImage);

        eyesCascade.detectMultiScale(grayROI(face), eyes);
        addRect(eyes, face);

        mouthCascade.detectMultiScale(grayROI(face),mouthes,6,10,0);
        addRect(mouthes, face);

        detectionResult.push_back(make_pair(face, make_pair(eyes, mouthes)));
    }

    doc.setObject(fileDesc);
    auto jsonData = doc.toJson();

    QFile file(QString::fromStdString(filename) + "_result.json");
    file.open(QIODevice::WriteOnly);
    file.write(jsonData);
    file.close();

    std::cout << "-------" << endl;
    std::cout << "File " << filepath << " has been processed" << std::endl;
    std::cout << std::to_string(faces.size()) + " faces was found" << std::endl;

    if(worker != nullptr)
        worker->done();

    return detectionResult;
}
