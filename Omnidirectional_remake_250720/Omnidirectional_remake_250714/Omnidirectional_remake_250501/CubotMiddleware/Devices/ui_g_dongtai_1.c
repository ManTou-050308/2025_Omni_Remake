//
// Created by RM UI Designer
//

#include "ui_g_dongtai_1.h"
#include "string.h"
#include "hardware_config.h"
#include "referee.h"

#define FRAME_ID 0
#define GROUP_ID 2
#define START_ID 1

ui_string_frame_t ui_g_dongtai_1;

ui_interface_string_t* ui_g_dongtai_dengji = &ui_g_dongtai_1.option;

void _ui_init_g_dongtai_1() {
    ui_g_dongtai_1.option.figure_name[0] = FRAME_ID;
    ui_g_dongtai_1.option.figure_name[1] = GROUP_ID;
    ui_g_dongtai_1.option.figure_name[2] = START_ID;
    ui_g_dongtai_1.option.operate_tpyel = 1;
    ui_g_dongtai_1.option.figure_tpye = 7;
    ui_g_dongtai_1.option.layer = 0;
    ui_g_dongtai_1.option.font_size = 30;
    ui_g_dongtai_1.option.start_x = 943;
    ui_g_dongtai_1.option.start_y = 840;
    ui_g_dongtai_1.option.color = 8;
    ui_g_dongtai_1.option.str_length = 1;
    ui_g_dongtai_1.option.width = 3;
    strcpy(ui_g_dongtai_dengji->string, "0");

    ui_proc_string_frame(&ui_g_dongtai_1);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_1, sizeof(ui_g_dongtai_1));
}

void _ui_update_g_dongtai_1() {
    ui_g_dongtai_1.option.operate_tpyel = 2;

		sprintf(ui_g_dongtai_dengji->string, "%d",referee2024.robot_status_t.robot_level);
    ui_proc_string_frame(&ui_g_dongtai_1);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_1, sizeof(ui_g_dongtai_1));
}

void _ui_remove_g_dongtai_1() {
    ui_g_dongtai_1.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_dongtai_1);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_1, sizeof(ui_g_dongtai_1));
}
