//
// Created by RM UI Designer
//

#include "ui_g_danliang_1.h"
#include "string.h"

#define FRAME_ID 0
#define GROUP_ID 3
#define START_ID 1

ui_string_frame_t ui_g_danliang_1;

ui_interface_string_t* ui_g_danliang_half = &ui_g_danliang_1.option;

void _ui_init_g_danliang_1() {
    ui_g_danliang_1.option.figure_name[0] = FRAME_ID;
    ui_g_danliang_1.option.figure_name[1] = GROUP_ID;
    ui_g_danliang_1.option.figure_name[2] = START_ID;
    ui_g_danliang_1.option.operate_tpyel = 1;
    ui_g_danliang_1.option.figure_tpye = 7;
    ui_g_danliang_1.option.layer = 0;
    ui_g_danliang_1.option.font_size = 20;
    ui_g_danliang_1.option.start_x = 1511;
    ui_g_danliang_1.option.start_y = 547;
    ui_g_danliang_1.option.color = 8;
    ui_g_danliang_1.option.str_length = 3;
    ui_g_danliang_1.option.width = 2;
    strcpy(ui_g_danliang_half->string, "250");

    ui_proc_string_frame(&ui_g_danliang_1);
    SEND_MESSAGE((uint8_t *) &ui_g_danliang_1, sizeof(ui_g_danliang_1));
}

void _ui_update_g_danliang_1() {
    ui_g_danliang_1.option.operate_tpyel = 2;
		strcpy(ui_g_danliang_half->string, "250");
    ui_proc_string_frame(&ui_g_danliang_1);
    SEND_MESSAGE((uint8_t *) &ui_g_danliang_1, sizeof(ui_g_danliang_1));
}

void _ui_remove_g_danliang_1() {
    ui_g_danliang_1.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_danliang_1);
    SEND_MESSAGE((uint8_t *) &ui_g_danliang_1, sizeof(ui_g_danliang_1));
}
