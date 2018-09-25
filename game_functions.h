#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H
#include <QThread>
#include <vector>
#include "robot.h" //Robot class
#include "utils.h" //Utils library
#include "fuzzy.h"
#include "navigation.h"
#include "serial.h"

class Game_functions: public Navigation
{
    Q_OBJECT
private:
    Point2d meta_goalkeeper;
    Point2d meta_defender_root;
    Point2d meta_volante;
    Point2d meta;
    p2dVector team_pos;
    Point ball_pos_grid;
    pVector enemy_pos_grid; //posicao em grid dos inimigos
    pVector team_pos_grid; //posicao em grid do time
    p2dVector enemy_pos; //posicao em cm dos inimigos
    Point2d ball_pos; //posicao em cm da bola
    Point2d centroid_atk; //posicao em cm do centro da area de atk
    Point2d centroid_def; //posicao em cm do centro da area de def
    double line_root_defender = 15;
    double line_guerreiro = 50;
    pVector def_area;
    double line_volante = 25;
    pair<double, double> ball_vel;
    vector<Point2d> media_ball_v;
    vector<pair<float, float> > vels;
    static Serial serial;

    bool stop, game_functions_initialized, flag_finish_functions; //variavel de controle de thread
    bool team_chang;
    bool calc_Gandalf, calc_Presto, calc_Leona;
    bool vel_Computed;
    bool atk_situation_state;
    Selector selec_robot; //estrutura de selecao dos robos que vao entrar no fuzzy

protected:
    void msleep(int ms);
    void run(); //rotina que a thread executa
signals:
    void emitRobots(Selector);
public:
    Game_functions();
    void init_game_functions();
    void goalkeeper(Robot *, int, pair<float, float>*);
    void defender(Robot *, int, pair<float, float>*);
    void defensive_midfielder(Robot *, int, pair<float, float>*);
    void ofensive_midfielder(Robot *, int, pair<float, float>*);
    void striker(Robot *, int, pair<float, float>*);
    void guardian(Robot *, int, pair<float,float>*);
    void killer(Robot *, int, pair<float,float>*);
    void test(Robot *, int, pair<float,float>*);
    void avoid_penalties();
    void return2defense(Robot *);
    Point2d prevision_atk(Robot *);
    void return2goal();
    double ajusta_angulo(double);
    bool get_flag_finish();
    void zera_flag_finish();
    void set_def_area(pVector def_area);
    void set_ball_vel(pair<double, double>);
    void set_to_select(Robot, Robot, Robot);
    void set_enemy_pos(p2dVector);
    void set_team_pos(p2dVector);
    void set_ball_pos(Point2d);
    void set_centroid_atk(Point2d);
    void set_centroid_def(Point2d);
    void set_calc_Gandalf(bool);
    void set_calc_Presto(bool);
    void set_calc_Leona(bool);
    void set_line_root_def(double);
    void Play();
    bool is_running();
    void Stop();
    bool isStopped() const;
    void team_changed();
    Point2d get_meta_goalkeeper();
    Point2d get_meta_defender_root();
    Point2d get_meta_volante();
    Point2d get_meta();
    Point convert_C_to_G(Point2d);
    int flag_return;

    void robo_grid_position(Robot *, Robot *, Robot *);

    void killer_cpu(Robot *, int, pair<float, float>*);   // Univector Func
    void defender_root(Robot *, int, pair<float, float>*);   // Univector Func
    //Novas funções
    void fake9(Robot *, int, pair<float,float>*);
    Point2d meta_fake9;
    void volante(Robot *, int, pair<float, float>*);   // Univector Func
    //Fim novas funções

    void Set_atk_situation_state(bool);
    bool Get_atk_situation_state();

    void set_g_size(Point2d meta, Robot *robo);

    Point2d meta_cpu;
    Point2d get_meta_cpu();
    void set_meta_cpu(Point2d);

    void guerreiro(Robot *, int, pair<float, float>*);

    vector<Point2d> matrizCachaco;

    ~Game_functions();
};

#endif // GAME_FUNCTIONS_H


