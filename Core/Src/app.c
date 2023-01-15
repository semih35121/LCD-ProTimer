/*
 * app.c
 *
 *  Created on: Dec 24, 2022
 *      Author: Semih
 */
#include <stdlib.h>
#include <stdbool.h>
#include "app.h"
#include <main.h>
#include <string.h>
int productive_time_taken;
int state_transition;
static find_digit_number(int time) {
	int digit_num = 0;
	while (time) {
		time /= 10;
		digit_num++;
	}
	return digit_num;
}
void do_beep(void) {
	//do something

}

bool isevent_thick(tim_tick_event_signal_t *e) {
	static uint32_t curr_time;
	static uint32_t curr_buzzer_time;
	bool returned_flag_status = false;
	if ((HAL_GetTick() - curr_time) > 1000) {
		(*e).ss = 1;
		(*e).sig_thick = TIME_TICK;
		curr_time = HAL_GetTick();
		returned_flag_status = true;
	}
	if ((HAL_GetTick() - curr_buzzer_time) > 2000) {
		(*e).bs = 1;
		(*e).sig_thick = BUZZER_TICK;
		curr_buzzer_time = HAL_GetTick();
		returned_flag_status = true;
	}
	if ((*e).bs && (*e).ss) {
		(*e).sig_thick = TIME_BUZ_TICK;
		return returned_flag_status;
	}
	return returned_flag_status;
}
bool display_event_handler(uint16_t display_evt_type, uint32_t time_info) {
	switch (display_evt_type) {
	case DISP_PAUSED: {
		lcd_put_cur(0, 4);//set_display_cursor(x, y); //PAUSED mesajı ilgili cursorda ayarlanması sağlanacak
		lcd_send_string("PAUSED\n");				//display_message("PAUSED\n");
		return true;
	}
	case DISP_SHOW_TIME: {
		lcd_put_cur(1,4);				//set_display_cursor(x, y); //Anlık zaman verisinin yazılacağı pozisiyon ayarlaması yapıldı
		uint32_t current_time_minutes = time_info / 60;
		uint32_t current_time_second = time_info % 60;
		char written_buffer[6] = { 0 };
		uint8_t digit_num_min = find_digit_number(current_time_minutes);
		uint8_t digit_number_sec = find_digit_number(current_time_second);
		if (digit_num_min == 1 && digit_number_sec == 1)
			sprintf(written_buffer, "0%d:0%d", current_time_minutes,
					current_time_second);
		else if (digit_num_min == 2 && digit_number_sec == 1)
			sprintf(written_buffer, "%d:0%d", current_time_minutes,
					current_time_second);
		else if (digit_num_min == 1 && digit_number_sec == 2)
			sprintf(written_buffer, "0%d:%d", current_time_minutes,
					current_time_second);
		else if (digit_num_min == 2 && digit_number_sec == 2)
			sprintf(written_buffer, "%d:%d", current_time_minutes,
					current_time_second);
		else if (digit_num_min == 0 && digit_number_sec == 0)
			sprintf(written_buffer, "0%d:0%d", 0, 0);
		else
			return false;
		lcd_send_string(written_buffer);				//display_message(written_buffer);
		return true;
	}
	case DISP_SET_TIME: {
		lcd_put_cur(0, 4);						//set_display_cursor(x, y); //İLGİLİ SET TİME YAZISININ pozisyon ayarlaması yapılacak
		lcd_send_string("SET_TIME");		//display_message("SET_TIME");
		lcd_put_cur(1, 4); // Zaman Ayarlamasının yazılcağı yer
		char written_buffer[6] = { 0 };
		sprintf(written_buffer, "%0d:%0d", 0, 0);
		lcd_send_string(written_buffer);//display_message(written_buffer);
		return true;

	}
	case DISP_CLEAR: {
		lcd_clear();		//display_clear();
		return true;
	}
	case DISP_SHOW_PRODUCTIVE_TIME: {
		if (!productive_time_taken) {
			uint32_t current_time_minutes = time_info / 60;
			uint32_t current_time_second = time_info % 60;
			uint8_t digit_num_min = find_digit_number(current_time_minutes);
			uint8_t digit_number_sec = find_digit_number(current_time_second);
			lcd_put_cur(0, 4);				//set_display_cursor(x, y); //İLGİLİ Productive_TIME YAZISININ pozisyon ayarlaması yapılacak
			lcd_send_string("Productive_Time");						//display_message("Productive_TIME");
			lcd_put_cur(1, 4);				//set_display_cursor(x, y); // Zaman Ayarlamasının yazılcağı yer
			char written_buffer[6] = { 0 };
			if (digit_num_min == 1 && digit_number_sec == 1)
				sprintf(written_buffer, "0%d:0%d", current_time_minutes,
						current_time_second);
			else if (digit_num_min == 2 && digit_number_sec == 1)
				sprintf(written_buffer, "%d:0%d", current_time_minutes,
						current_time_second);
			else if (digit_num_min == 1 && digit_number_sec == 2)
				sprintf(written_buffer, "0%d:%d", current_time_minutes,
						current_time_second);
			else if (digit_num_min == 2 && digit_number_sec == 2)
				sprintf(written_buffer, "%d:%d", current_time_minutes,
						current_time_second);
			else if (digit_num_min == 0 && digit_number_sec == 0)
				sprintf(written_buffer, "0%d:0%d", 0, 0);
			else
				return false;
			lcd_send_string(written_buffer);				//display_message(written_buffer);
			productive_time_taken++;
		}
		return true;

	}

	}

}

void copy_button_evt_to_general_evt(protimer_button_event_t *evt,
		pro_timer_general_event_t *general_evt) {
	evt=&((*general_evt).super);
}
void copy_tick_info_to_general_struct(tim_tick_event_signal_t *time_signal,
		pro_timer_general_event_t *general_evt) {
	general_evt->super_thick = *(time_signal);
}

void protimer_state_handler_idle(protimer_t *mobj, pro_timer_general_event_t *e) {

	if (!(e->super_thick.is_time_idle)) {
		static int buzzer_cnt;
		switch (e->super_thick.sig_thick) {
		case TIME_BUZ_TICK:
			do_beep();
			if (buzzer_cnt++ > 60) {
				char written_data[100] = "";
				sprintf(written_data, "%s%d",
						"Herhangi bir islem yapmadiniz lutfen islem yapiniz\r\n",
						buzzer_cnt);
				HAL_UART_Transmit(&huart1, (uint8_t*) written_data,
						strlen(written_data), 100);
				buzzer_cnt = 0;
			}

			break;
		default:
			break;
		}

	}
	uint8_t coming_signal = e->super.sig;
	if (((*e).super.is_button_pressed)) {
		switch (coming_signal) //Button Eventten gelen sinyaldir
		{
		case INC_TIME: {
			mobj->curr_time += 60;
			mobj->active_state = TIME_SET;
			break;
		}

		case START_PAUSE: {
			mobj->active_state = STAT;
			break;
		}

		default:
			break;
		}
		e->super.is_button_pressed=0;
		mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_clear();
	} else {
		mobj->curr_time = 0;
		mobj->elapsed_time = 0;
		mobj->display_info_msg(DISP_SET_TIME,mobj->curr_time);
	}


}
void protimer_state_handler_timeset(protimer_t *mobj,
		pro_timer_general_event_t *e) {
	uint8_t coming_signal = e->super.sig;
	if (!(e->super.is_button_pressed)
			/*|| (!((coming_signal == ABRT) || (coming_signal == START_PAUSE)))*/) {
			mobj->display_info_msg(DISP_SHOW_TIME,mobj->curr_time);				//display_time(mobj->curr_time);
	}
	if (e->super.is_button_pressed) {
		switch (e->super.sig) {
		case INC_TIME:
			mobj->curr_time += 60;

			mobj->display_info_msg(DISP_SHOW_TIME,mobj->curr_time)	;//display_time(mobj->curr_time);
			break;

		case DEC_TIME:
			if (mobj->curr_time >= 60) {
				mobj->curr_time -= 60;
				mobj->display_info_msg(DISP_SHOW_TIME,mobj->curr_time);//display_time(mobj->curr_time);

			} else {
				mobj->display_info_msg(DISP_SHOW_TIME,mobj->curr_time=0)	;//display_time(0);

			}
			break;
		case ABRT:
			mobj->active_state = IDLE;
			mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_clear();
			break;
		case START_PAUSE:
			if (mobj->curr_time >= 60){
				mobj->active_state = COUNTDOWN;
				mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_clear(); //TO İMPLEMENT EXIT ACTION

			}
			break;
		}
		e->super.is_button_pressed=0;
	}

	/*if (((coming_signal == ABRT) || (coming_signal == START_PAUSE))) {
		mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_clear(); //TO İMPLEMENT EXIT ACTION
	}*/

}

void protimer_state_handler_countdown(protimer_t *mobj,
		pro_timer_general_event_t *e) {
	if (!(e->super_thick.is_time_idle)) {
		switch (e->super_thick.sig_thick) {
		case TIME_TICK:
			--mobj->curr_time;
			++mobj->elapsed_time;
			mobj->display_info_msg(DISP_SHOW_TIME,mobj->curr_time);//display_time(mobj->curr_time);
			if (mobj->curr_time == 0) {
				mobj->active_state = IDLE;
				mobj->pro_time += mobj->elapsed_time;
				mobj->elapsed_time = 0;
				mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_time(mobj->curr_time);

			}

		case TIME_BUZ_TICK:
			--mobj->curr_time;
			++mobj->elapsed_time;
			mobj->display_info_msg(DISP_SHOW_TIME,mobj->curr_time)	;//display_time(mobj->curr_time);
			if (mobj->curr_time == 0) {
				mobj->active_state = IDLE;
				mobj->pro_time += mobj->elapsed_time;
				mobj->elapsed_time = 0;
				mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_time(mobj->curr_time);

			}
		}

	}
	if (e->super.is_button_pressed) {
			switch ((*e).super.sig) {
				case ABRT:{
					mobj->active_state=IDLE;
					mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_time(mobj->curr_time);

				}
				case START_PAUSE:{
					mobj->active_state=PAUSE;
					mobj->display_info_msg(DISP_CLEAR,mobj->curr_time);//display_time(mobj->curr_time);

				}
			}
			e->super.is_button_pressed=0;
	}
}
void protimer_state_handler_pause(protimer_t *mobj,
		pro_timer_general_event_t *e) {
	if (e->super.is_button_pressed) {
		switch ((*e).super.sig) {
		case INC_TIME: {
			mobj->curr_time += 60;
			mobj->active_state = TIME_SET;
			mobj->display_info_msg(DISP_CLEAR,mobj->curr_time)	;//display_clear();
			return;
		}
		case DEC_TIME: {
			if (mobj->curr_time >= 60)
				mobj->curr_time -= 60;
			else {
				mobj->curr_time = 0;
				do_beep();
				mobj->active_state = TIME_SET;
				mobj->display_info_msg(DISP_CLEAR,mobj->curr_time)	;//display_clear();
			}
			return;
		}
		case ABRT: {
			mobj->active_state = IDLE;
			mobj->display_info_msg(DISP_CLEAR,mobj->curr_time)	;//display_clear();
			return;
		}
		case START_PAUSE: {
			mobj->active_state = COUNTDOWN;
			mobj->display_info_msg(DISP_CLEAR,mobj->curr_time)	;//display_clear();
			return;
		}
		}
		e->super.is_button_pressed=0;

	}

	mobj->display_info_msg(DISP_PAUSED,mobj->curr_time)	;//display_message("PAUSED\n");



}
void protimer_state_handler_stat(protimer_t *mobj, pro_timer_general_event_t *e) {
	if (e->super_thick.is_time_idle) {
		mobj->display_info_msg(DISP_SHOW_PRODUCTIVE_TIME,mobj->pro_time);		//display_message("Productive Time");
																	//display_time(mobj->pro_time);
	} else if (!(e->super_thick.is_time_idle)
			&& (e->super_thick.sig_thick == TIME_TICK
					|| e->super_thick.sig_thick == TIME_BUZ_TICK)) {
		mobj->active_state = IDLE;
		productive_time_taken = 0;
		mobj->display_info_msg(DISP_CLEAR,mobj->curr_time)	;//display_clear();
	}

}

void btn_event_handler(uint16_t GPIO_Pin, protimer_button_event_t *timtick_evt) {
	int current_time = HAL_GetTick();
	bool is_pressed_second_button = true;
	int time_origin = 0;
	int second_button_pressed_timeout = 1000; //1000ms içinde basılmaz ise kendi state'i içinde kalacak

	switch (GPIO_Pin) {
	case GPIO_PIN_0:
		while (((is_pressed_second_button = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))
				== true)) {
			if ((HAL_GetTick() - current_time) > second_button_pressed_timeout)
				break;
		}
		if (is_pressed_second_button == false)
			timtick_evt->sig = ABRT;
		else
			timtick_evt->sig = INC_TIME;
		timtick_evt->is_button_pressed = 1;
		break;
	case GPIO_PIN_1:
		while (((is_pressed_second_button = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
				== true)) {
			if ((HAL_GetTick() - current_time) > second_button_pressed_timeout)
				break;
		}
		if (is_pressed_second_button == false)
			timtick_evt->sig = ABRT;
		else
			timtick_evt->sig = START_PAUSE;

		timtick_evt->is_button_pressed = 1;

		break;
	case GPIO_PIN_4:
		timtick_evt->sig = DEC_TIME;
		timtick_evt->is_button_pressed = 1;

	default:
		timtick_evt->is_button_pressed = 0;
		break;
	}

}

protimer_t* create_protimer_t_obj(void) {
	protimer_t *mobj;
	if ((mobj = ((protimer_t*) malloc(sizeof(protimer_t)))) == NULL)
		return NULL;
	return mobj;
}
void pro_timer_init(protimer_t *mobj) {
	(*mobj).pro_time = 0;
	(*mobj).active_state = IDLE;
	(*mobj).curr_time = 0;
	(*mobj).elapsed_time = 0;
	(*mobj).display_info_msg=display_event_handler;
}
void pro_timer_state_machine(protimer_t *mobj, pro_timer_general_event_t *e) {
	switch (mobj->active_state) {
	case IDLE: {
		protimer_state_handler_idle(mobj, e);
		break;

	}
	case TIME_SET: {
		protimer_state_handler_timeset(mobj, e);
		break;
	}
	case COUNTDOWN: {
		protimer_state_handler_countdown(mobj, e);
		break;
	}
	case PAUSE: {
		protimer_state_handler_pause(mobj, e);
		break;
	}
	case STAT: {
		protimer_state_handler_stat(mobj, e);
		break;
	}
	default:
		break;

	}
}

