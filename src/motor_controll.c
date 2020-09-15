#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#include "motor_controll.h"

#define PWM_FREQ 200000

#define FL_MOTOR 32
#define FR_MOTOR 17

#define RL_MOTOR 25
#define RR_MOTOR 16

void set_motor_speed(enum motor_id id, float power_factor, float speed){

    switch (id) {
        case front_left:
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, power_factor * speed);
            break;
        case front_right:
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0B, power_factor * speed);
            break;
        
        case rear_left:
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM1A, power_factor * speed);
            break;

        case rear_right:
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM1B, power_factor * speed);
            break;

        default:
            break;
    }
}

void motor_control(void){

    /*Motor Controll pin initialization*/

    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, FL_MOTOR);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, FR_MOTOR);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, RL_MOTOR);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, RR_MOTOR);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = PWM_FREQ;    
    pwm_config.cmpr_a = 0;  
    pwm_config.cmpr_b = 0;   
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    
    float spd = 0;

    for(;;){
        set_motor_speed(front_right, 1.0, spd + 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }



    vTaskDelete(NULL);
}