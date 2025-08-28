//
// Created by RM UI Designer
//

#include "ui_g_roll_0.h"
#include "string.h"
#include "hardware_config.h"
#include "referee.h"
#include "holder.h"
#include "ins.h"

#define FRAME_ID 0
#define GROUP_ID 7
#define START_ID 0
#define myABS(x) ( (x)>0?(x):-(x) )
ui_string_frame_t ui_g_roll_1;

ui_interface_string_t* ui_g_dongtai_roll = &ui_g_roll_1.option;

void _ui_init_g_roll_1() {
    ui_g_roll_1.option.figure_name[0] = FRAME_ID;
    ui_g_roll_1.option.figure_name[1] = GROUP_ID;
    ui_g_roll_1.option.figure_name[2] = START_ID;
    ui_g_roll_1.option.operate_tpyel = 1;
    ui_g_roll_1.option.figure_tpye = 7;
    ui_g_roll_1.option.layer = 0;
    ui_g_roll_1.option.font_size = 30;
    ui_g_roll_1.option.start_x = 939;
    ui_g_roll_1.option.start_y = 760;
    ui_g_roll_1.option.color = 2;
    ui_g_roll_1.option.str_length = 1;
    ui_g_roll_1.option.width = 3;
    strcpy(ui_g_dongtai_roll->string, "0");

    ui_proc_string_frame(&ui_g_roll_1);
    SEND_MESSAGE((uint8_t *) &ui_g_roll_1, sizeof(ui_g_roll_1));
}

void _ui_update_g_roll() {
    ui_g_roll_1.option.operate_tpyel = 2;
	
		if(fabs( INS_attitude->roll)>10)ui_g_dongtai_roll->color = 5;
		else ui_g_dongtai_roll->color = 2;
	
		sprintf(ui_g_dongtai_roll->string, "%.2f",INS_attitude->roll);
	
    ui_proc_string_frame(&ui_g_roll_1);
    SEND_MESSAGE((uint8_t *) &ui_g_roll_1, sizeof(ui_g_roll_1));
}

void _ui_remove_g_roll_1() {
    ui_g_roll_1.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_roll_1);
    SEND_MESSAGE((uint8_t *) &ui_g_roll_1, sizeof(ui_g_roll_1));
}
