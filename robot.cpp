#include <iostream>
#include <string>
#include <sstream>
#include "utils.h"
#include "robot.h"

using namespace std;

Robot::Robot(){
    channel = -1;
    centroid = Point(-1, -1);
    centroid_cm = Point2d(0.0, 0.0);
    team_cent = Point(-1, -1);
    color_cent = Point(-1, -1);
    nick = "NULL";
    role = "NULL";
    ID = "NULL";
    low_color_team.assign(3, 0);
    upper_color_team.assign(3, 255);
    low_color.assign(3, 0);
    upper_color.assign(3, 255);
    pos_hist.push_back(Point(-1, -1));
    last_angle = loss_rate = 0.0;
    detected = false;
}

bool Robot::encoders_reading(Serial *serial, pair<double, double> &vels){
    string command("v000w000"), data_read, line, linear_vel, angular_vel;
    stringstream serial_stream;
    QByteArray data_read_bytes;

    serial->write_data(command);

    if(serial->bytes_available() > 0){
        data_read_bytes = serial->read_data();
        data_read = string(data_read_bytes.constData(), data_read_bytes.length());
        serial_stream = stringstream(data_read);

        getline(serial_stream, linear_vel, '\n');
        getline(serial_stream, angular_vel, '\n');

        serial_stream.flush();
        serial->flush();

        vels.first = stod(linear_vel);
        vels.second = stod(angular_vel);

        return true;
    }
    return false;
}

void Robot::set_angle(double angle)
{
    last_angle = this->angle;
    this->angle = angle;
}

double Robot::get_angle()
{
    return this->angle;
}

double Robot::get_last_angle()
{
    return this->last_angle;
}

void Robot::set_centroid(Point p)
{
    this->centroid = p;
    centroid_cm.x = centroid.x * X_CONV_CONST;
    centroid_cm.y = centroid.y * Y_CONV_CONST;

    add_pos_hist(p);
}

Point Robot::get_centroid()
{
    return this->centroid;
}


Point2d Robot::get_pos(){
    return centroid_cm;
}

void Robot::set_line_slope(Point p){
    this->line_slope = p;
}

Point Robot::get_line_slope(){
    return line_slope;
}

void Robot::add_pos_hist(Point p){
    if(pos_hist.size() == 5){
        pos_hist.pop_back();
    }
    pos_hist.push_back(p);
}

Point Robot::get_from_pos_hist(int rank){
    return pos_hist[pos_hist.size() - (rank + 1)];
}

void Robot::was_detected(bool detected){
    this->detected = detected;

    if((n_loss + n_detected)%500 == 0){
        loss_rate = n_loss / 500;
    }

    if(!detected){
        n_loss++;
    }else n_detected++;
}

bool Robot::is_detected(){
    return this->detected;
}

double Robot::get_loss_rate(){
    return loss_rate;
}

void Robot::set_color_cent(Point p)
{
    this->color_cent = p;
}

Point Robot::get_color_cent()
{
    return this->color_cent;
}

void Robot::set_team_cent(Point p)
{
    this->team_cent = p;
}

Point Robot::get_team_cent()
{
    return this->team_cent;
}

void Robot::set_channel(int channel)
{
    this->channel = channel;
}

void Robot::set_role(string role)
{
    this->role = role;
}

void Robot::set_nick(string nick)
{
    this->nick = nick;
}

void Robot::set_ID(string ID)
{
    this->ID = ID;
}

void Robot::set_team_low_color(vector<int> low_color){
    this->low_color_team = low_color;
}

void Robot::set_team_upper_color(vector<int> upper_color){
    this->upper_color_team = upper_color;
}

vector<int> Robot::get_team_low_color(){
    return this->low_color_team;
}

vector<int> Robot::get_team_upper_color(){
    return this->upper_color_team;
}

void Robot::set_low_color(vector<int> low_color)
{
    this->low_color = low_color;
}

void Robot::set_upper_color(vector<int> upper_color)
{
    this->upper_color = upper_color;
}

vector<int> Robot::get_low_color()
{
    return this->low_color;
}

vector<int> Robot::get_upper_color()
{
    return this->upper_color;
}

string Robot::get_nick()
{
    return this->nick;
}

string Robot::get_role()
{
    return this->role;
}

string Robot::get_ID()
{
    return this->ID;
}

