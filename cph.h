#ifndef CPH_H
#define CPH_H

#include <QThread>
#include <vector>
#include "robot.h" //Robot class
#include "utils.h" //Utils library


class CPH : QThread
{
    Q_OBJECT
private:
    bool stop;
protected:
    int dx;
    int dy;
    dMatrix pGrid;
    iMatrix tGrid;

    void run();
    void msleep(int ms);
signals:

public:
    CPH(int, int);
    double iterator();
    double get_neighborhood(int, int, int);
    void set_potential(int, int, double);
    double get_potential(int, int);
    int get_occupancy(int, int);
    Point convert_C_to_G(Point2d);
    void set_direction();
    void init_grid();
    void print_grid();
    void Play();
    void Stop();
    bool isStopped() const;
};

#endif // CPH_H
