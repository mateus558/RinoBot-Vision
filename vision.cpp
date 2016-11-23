#include "vision.h"
#include <time.h>
#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <ctime>
#include <QMessageBox>

using namespace std;

Vision::Vision(QObject *parent): QThread(parent)
{
    stop = true;
    mode = 0;
    robots.resize(6);
    robots[0].set_nick("Leona");
    robots[1].set_nick("Gandalf");
    robots[2].set_nick("Presto");
    robots[3].set_nick("T2");
    robots[4].set_nick("T2");
    robots[5].set_nick("T2");
    low.resize(3);
    upper.resize(3);
    low = {0,0,0};
    upper = {255, 255, 255};
}

void Vision::add_adjacents(queue<Point> &pilha, Point u, bool **visited){
    Point p1(int(u.x/2), int(u.y/2)), p2(int(u.x + u.x/2), int(u.y/2)), p3(int(u.x/2), int(u.y + u.y/2)), p4(int(u.x + u.x/2), int(u.y + u.y/2));
    /*cout << p1.x << " " << p1.y << endl;
    cout << p2.x << " " << p2.y << endl;
    cout << p3.x << " " << p3.y << endl;
    cout << p4.x << " " << p4.y << endl;*/
    if(p1.x < rows && p1.y < cols && !visited[p1.x][p1.y])
        pilha.push(p1);
    if(p2.x < rows && p2.y < cols && !visited[p2.x][p2.y])
        pilha.push(p2);
    if(p3.x < rows && p3.y < cols && !visited[p3.x][p3.y])
        pilha.push(p3);
    if(p4.x < rows && p4.y < cols && !visited[p4.x][p4.y])
        pilha.push(p4);
}

void Vision::detect_robots(Mat frame, vector<Robot> robots){
    int i, j = 0;
    Mat out_team1, out_team2, out_r[3];
    vector<int> low, upper;

    for(i = 0; i < robots.size()-3; ++i){
        low = robots[i].get_low_color();
        upper = robots[i].get_upper_color();
        inRange(frame, Scalar(low[0], low[1], low[2]), Scalar(upper[0], upper[1], upper[2]), out_r[i]);
    }

    low = robots[0].get_team_low_color();
    upper = robots[0].get_team_upper_color();
    inRange(frame, Scalar(low[0], low[1], low[2]), Scalar(upper[0], upper[1], upper[2]), out_team1);

    low = robots[4].get_team_low_color();
    upper = robots[4].get_team_upper_color();
    inRange(frame, Scalar(low[0], low[1], low[2]), Scalar(upper[0], upper[1], upper[2]), out_team2);
}

Mat Vision::adjust_gamma(double gamma, Mat org)
{
    if(gamma == 1.0)
        return org;

    double inv_gamma = 1.0 / gamma;

    Mat lut_matrix(1, 256, CV_8UC1);
    uchar *ptr = lut_matrix.ptr();

    for(int i = 0; i < 256; i++)
        ptr[i] = (int)(pow((double)i / 255.0, inv_gamma) * 255.0);

    Mat result;
    LUT(org, lut_matrix, result);

    return result;
}

Mat Vision::CLAHE_algorithm(Mat org)    //Normalize frame histogram
{
    Mat lab_image, dst;
    vector<Mat> lab_planes(3);

    cvtColor(org, lab_image, CV_BGR2Lab);
    split(lab_image, lab_planes);

    //Apply CLAHE to the luminosity plane of the LAB frame
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8,8));
    clahe->apply(lab_planes[0], dst);

    //Merge back the planes
    dst.copyTo(lab_planes[0]);
    merge(lab_planes, lab_image);

    cvtColor(lab_image, dst, CV_Lab2RGB);

    return dst;
}

void Vision::proccess_frame(Mat orig, Mat dest) //Apply enhancement algorithms
{
    dest = orig.clone();
    //Gamma correction
    dest = adjust_gamma(1.0, dest);
    //Apply histogram normalization
    dest = CLAHE_algorithm(dest);
    //Apply gaussian blur
    GaussianBlur(dest, dest, Size(5,5), 2);
}

Mat Vision::detect_colors(Mat vision_frame, vector<int> low, vector<int> upper) //Detect colors in [low,upper] range
{
    Mat mask;
    //Generate mask with the points in the range
    inRange(vision_frame, Scalar(low[0],low[1],low[2]), Scalar(upper[0],upper[1],upper[2]), mask);
    //Attempt to remove noise (or small objects)
    morphologyEx(mask, mask, MORPH_OPEN, Mat(), Point(-1, -1), 2);
    morphologyEx(mask, mask, MORPH_CLOSE, Mat(), Point(-1, -1), 2);

    return mask;
}

Mat Vision::setting_mode(Mat raw_frame, Mat vision_frame, vector<int> low, vector<int> upper)   //Detect colors in [low,upper] range
{
    Mat mask, res;

    mask = detect_colors(vision_frame, low, upper);
    cvtColor(raw_frame, raw_frame, CV_BGR2RGB);
    raw_frame.copyTo(res, mask);

    return res;
}

void Vision::run()
{
    int delay = (1000/this->FPS);
    int i = 0;
    Mat res;
    double elapsed_secs;
    clock_t begin, end;

    while(!stop){
        begin = clock();

        if(!cam.read(raw_frame)){
            stop = true;
            continue;
        }

        rows = raw_frame.rows;
        cols = raw_frame.cols;

        vision_frame = raw_frame.clone();
        proccess_frame(raw_frame, vision_frame);

        switch(mode){
            case 0: //Visualization mode
               // detect_robots(vision_frame, robots);
                cvtColor(vision_frame, vision_frame, CV_BGR2RGB);
                img = QImage((uchar*)(vision_frame.data), vision_frame.cols, vision_frame.rows, QImage::Format_RGB888);
                break;
            case 1: //Set color mode
                res = setting_mode(raw_frame, vision_frame, low, upper);
                img = QImage((uchar*)(res.data), res.cols, res.rows, QImage::Format_RGB888);
                break;
            default:
                break;
         }

        end = clock();
        elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        FPS = 1/elapsed_secs;

        emit processedImage(img);
        if(i%10 == 0){
            emit framesPerSecond(FPS);
            i = 0;
        }

        msleep(delay);
        i++;
    }
}

bool Vision::open_camera(int camid)
{
    if(!cam.isOpened()){
        cam.open(camid);
    }

    if(cam.isOpened()){
        FPS = 60;
        return true;
    }

    this->camid = camid;

    return false;
}

void Vision::Play()
{
    if(!isRunning()){
        if(isStopped())
            stop = false;
        start(LowPriority);
    }
}

vector<Robot> Vision::get_robots()
{
    return robots;
}

void Vision::set_robots(vector<Robot> robots)
{
    cout << robots[2].get_upper_color()[0] <<endl;
    this->robots = robots;
}

void Vision::set_mode(int m)
{
    mode = m;
}

void Vision::Stop()
{
    stop = true;
}

bool Vision::isStopped() const
{
    return this->stop;
}

bool Vision::is_open()
{
    return cam.isOpened();
}

void Vision::msleep(int ms)
{
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000 * 1000};
    nanosleep(&ts, NULL);
}

void Vision::release_cam()
{
    cam.release();
}

int Vision::get_camID()
{
    return this->camid;
}

Vision::~Vision(){
    mutex.lock();
    stop = true;
    cam.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}
