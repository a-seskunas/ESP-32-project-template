#pragma once

#include "AppSensor.hpp"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define MOTOR_PWM_CHANNEL_LEFT LEDC_CHANNEL_1
#define MOTOR_PWM_CHANNEL_RIGHT LEDC_CHANNEL_2
#define MOTOR_PWM_TIMER LEDC_TIMER_1
#define MOTOR_PWM_BIT_NUM LEDC_TIMER_10_BIT

#define RIGHT_PWM_PIN GPIO_NUM_19
#define LEFT_PWM_PIN GPIO_NUM_21
#define RIGHT_FORWARD GPIO_NUM_22
#define RIGHT_BACK GPIO_NUM_23
#define LEFT_FORWARD GPIO_NUM_33
#define LEFT_BACK GPIO_NUM_32


namespace app
{
    class AppMotor : public AppSensor
    {
	    private:
	        float m_power;

	    protected:
		void handleNewState(void) override
		{
		    // The Motor state has been changed, so handle it here
		    motor_pwm_set(m_state["power"], m_state["power"]);
		    m_state["power"] = motor_pwm_get();

		    if (m_state["turn"] == "left")
		    {
			    turn_left();
	            }
		    else if (m_state["turn"] == "right")
			    turn_right();
		}

	    public:
		AppMotor() : AppSensor("Motors")
		{
		}

		void init(void) override
		{
		   gpio_reset_pin(RIGHT_FORWARD);
		   gpio_reset_pin(LEFT_FORWARD);
		   gpio_reset_pin(RIGHT_BACK);
		   gpio_reset_pin(LEFT_BACK);

		   gpio_set_direction(RIGHT_FORWARD, GPIO_MODE_OUTPUT);
		   gpio_set_direction(RIGHT_BACK, GPIO_MODE_OUTPUT);
		   gpio_set_direction(LEFT_FORWARD, GPIO_MODE_OUTPUT);
		   gpio_set_direction(LEFT_BACK, GPIO_MODE_OUTPUT);
		   gpio_set_direction(LEFT_PWM_PIN, GPIO_MODE_OUTPUT);
		   gpio_set_direction(RIGHT_PWM_PIN, GPIO_MODE_OUTPUT);

		   ledc_channel_config_t ledc_channel_left;
		   ledc_channel_left.gpio_num = LEFT_PWM_PIN;
		   ledc_channel_left.speed_mode = LEDC_HIGH_SPEED_MODE;
		   ledc_channel_left.channel = MOTOR_PWM_CHANNEL_LEFT;
		   ledc_channel_left.intr_type = LEDC_INTR_DISABLE;
		   ledc_channel_left.timer_sel = MOTOR_PWM_TIMER;
		   ledc_channel_left.duty = 0;
		   ledc_channel_left.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;

		   ledc_channel_config_t ledc_channel_right;
		   ledc_channel_right.gpio_num = RIGHT_PWM_PIN;
		   ledc_channel_right.speed_mode = LEDC_HIGH_SPEED_MODE;
		   ledc_channel_right.channel = MOTOR_PWM_CHANNEL_RIGHT;
		   ledc_channel_right.intr_type = LEDC_INTR_DISABLE;
		   ledc_channel_right.timer_sel = MOTOR_PWM_TIMER;
		   ledc_channel_right.duty = 0;
		   ledc_channel_right.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;

		   ledc_timer_config_t ledc_timer;
		   ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
		   ledc_timer.duty_resolution = MOTOR_PWM_BIT_NUM;
		   ledc_timer.timer_num = MOTOR_PWM_TIMER;
		   ledc_timer.freq_hz = 1200;
		   ledc_timer.clk_cfg = LEDC_AUTO_CLK;
		   ledc_timer.deconfigure = false;

		   ledc_timer_config(&ledc_timer);
		   ledc_channel_config(&ledc_channel_left);
		   ledc_channel_config(&ledc_channel_right);
		}

		void motor_pwm_set(float left_duty_fraction, float right_duty_fraction) 
		{
		   m_power = left_duty_fraction; 

		   gpio_set_level(RIGHT_FORWARD, 1);
		   gpio_set_level(RIGHT_BACK, 0);
		   gpio_set_level(LEFT_FORWARD, 1);
		   gpio_set_level(LEFT_BACK, 0);

		   uint32_t left_duty = left_duty_fraction;
		   uint32_t right_duty = right_duty_fraction;

		   ledc_set_duty(LEDC_HIGH_SPEED_MODE, MOTOR_PWM_CHANNEL_LEFT, left_duty);
		   ledc_update_duty(LEDC_HIGH_SPEED_MODE, MOTOR_PWM_CHANNEL_LEFT);

		   ledc_set_duty(LEDC_HIGH_SPEED_MODE, MOTOR_PWM_CHANNEL_RIGHT, right_duty);
		   ledc_update_duty(LEDC_HIGH_SPEED_MODE, MOTOR_PWM_CHANNEL_RIGHT);
		}

		uint32_t motor_pwm_get() 
		{
		   return ledc_get_duty(LEDC_HIGH_SPEED_MODE, MOTOR_PWM_CHANNEL_LEFT);
		}

		void turn_left() 
		{
                   motor_pwm_set(0, 500);
		}

		void turn_right()
		{
                   motor_pwm_set(500, 0);
		}
    };
}
