/*
 * app.h
 *
 *  Created on: Dec 24, 2022
 *      Author: ibrhm
 */

#ifndef INC_APP_H_
#define INC_APP_H_
#include <stdint.h>
#include <main.h>
#include <stdbool.h>
#include "lcd.h"
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
enum{
	DISP_PAUSED,
	DISP_SET_TIME,
	DISP_CLEAR,
	DISP_SHOW_TIME,
	DISP_SHOW_PRODUCTIVE_TIME
};
typedef enum{
    INC_TIME,
    DEC_TIME,
    TIME_TICK,
	BUZZER_TICK,
    START_PAUSE,
    ABRT,
	TIME_BUZ_TICK
}protimer_signal_t;
typedef enum{
    IDLE,
    TIME_SET,
    COUNTDOWN,
    PAUSE,
    STAT
}protimer_state_t;
typedef struct{
    uint8_t sig;
    uint8_t is_button_pressed;
    uint8_t is_button_state_idle;
}protimer_button_event_t;
typedef struct tim_tick_event_signal{
	int sig_thick;
	int ss;
	int bs;
	int is_time_idle;
}tim_tick_event_signal_t;
typedef struct{
	protimer_button_event_t super;
	tim_tick_event_signal_t super_thick;
}pro_timer_general_event_t;
void display_time(int time_val);
void display_message(const char * message);
void display_clear(void);
void set_display_cursor(int x, int y);
void do_beep(void);
void btn_event_handler(uint16_t GPIO_Pin,protimer_button_event_t *timtick_evt);
typedef bool(*display_event)(uint16_t display_evt_type,uint32_t time_info);//gelen evt tipine göre displayler ilgili işşlemleri yapacak.
bool display_event_handler(uint16_t display_evt_type,uint32_t time_info);
/*typedef enum{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION
}event_status_t;*/
typedef struct{
    uint32_t curr_time;
    uint32_t elapsed_time;
    uint32_t pro_time;
    protimer_state_t active_state;
    display_event display_info_msg;
}protimer_t;
void protimer_state_handler_idle(protimer_t *mobj,pro_timer_general_event_t *e);
void protimer_state_handler_timeset(protimer_t *mobj,pro_timer_general_event_t *e);
void protimer_state_handler_countdown(protimer_t *mobj,pro_timer_general_event_t *e);
void protimer_state_handler_pause(protimer_t *mobj,pro_timer_general_event_t *e);
void protimer_state_handler_stat(protimer_t *mobj,pro_timer_general_event_t *e);
bool isevent_thick(tim_tick_event_signal_t *e);
void copy_button_evt_to_general_evt(protimer_button_event_t *evt,pro_timer_general_event_t *general_evt);
void copy_tick_info_to_general_struct(tim_tick_event_signal_t *time_signal,pro_timer_general_event_t *general_evt);
typedef enum thick_event_type{
	EVENT_THICK,
	EVENT_BUZZER_THICK
}thick_event_type_t;

//Main Application Structure

//GENERİC EVENT STRUCTURE

//FOR USER GENERATED EVENTS



void pro_timer_init(protimer_t *mobj);
protimer_t * create_protimer_t_obj(void);
void pro_timer_state_machine(protimer_t *mobj,pro_timer_general_event_t *e);






#endif /* INC_APP_H_ */
