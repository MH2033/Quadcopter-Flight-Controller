#ifndef MOTOR_CONTROLL_H
#define MOTOR_CONTROLL_H

enum motor_id{
    front_left,
    front_right,
    rear_left,
    rear_right
};

struct motor_data{
    double fl;
    double fr;
    double rl;
    double rr;
};

void motor_control(void *);

#endif