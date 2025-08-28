//
// Created by RM UI Designer
//

#include "ui_g_static_0.h"
#include "string.h"

#define FRAME_ID 0
#define GROUP_ID 1
#define START_ID 0

ui_string_frame_t ui_g_static_0;

ui_interface_string_t* ui_g_static_level_bioaqian = &ui_g_static_0.option;

void _ui_init_g_static_0() {
    ui_g_static_0.option.figure_name[0] = FRAME_ID;
    ui_g_static_0.option.figure_name[1] = GROUP_ID;
    ui_g_static_0.option.figure_name[2] = START_ID;
    ui_g_static_0.option.operate_tpyel = 1;
    ui_g_static_0.option.figure_tpye = 7;
    ui_g_static_0.option.layer = 0;
    ui_g_static_0.option.font_size = 30;
    ui_g_static_0.option.start_x = 722;
    ui_g_static_0.option.start_y = 840;
    ui_g_static_0.option.color = 8;
    ui_g_static_0.option.str_length = 6;
    ui_g_static_0.option.width = 3;
    strcpy(ui_g_static_level_bioaqian->string, "Level:");

    ui_proc_string_frame(&ui_g_static_0);
    SEND_MESSAGE((uint8_t *) &ui_g_static_0, sizeof(ui_g_static_0));
}

void _ui_update_g_static_0() {
    ui_g_static_0.option.operate_tpyel = 2;

    ui_proc_string_frame(&ui_g_static_0);
    SEND_MESSAGE((uint8_t *) &ui_g_static_0, sizeof(ui_g_static_0));
}

void _ui_remove_g_static_0() {
    ui_g_static_0.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_static_0);
    SEND_MESSAGE((uint8_t *) &ui_g_static_0, sizeof(ui_g_static_0));
}
