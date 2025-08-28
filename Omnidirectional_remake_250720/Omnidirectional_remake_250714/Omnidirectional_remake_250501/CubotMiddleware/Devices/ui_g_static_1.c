//
// Created by RM UI Designer
//

#include "ui_g_static_1.h"

#define FRAME_ID 0
#define GROUP_ID 1
#define START_ID 1
#define OBJ_NUM 2
#define FRAME_OBJ_NUM 2

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_g_static_1;
ui_interface_rect_t *ui_g_static_chaodiankuang = (ui_interface_rect_t *)&(ui_g_static_1.data[0]);
ui_interface_round_t *ui_g_static_dipanjiyaun = (ui_interface_round_t *)&(ui_g_static_1.data[1]);

void _ui_init_g_static_1() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_static_1.data[i].figure_name[0] = FRAME_ID;
        ui_g_static_1.data[i].figure_name[1] = GROUP_ID;
        ui_g_static_1.data[i].figure_name[2] = i + START_ID;
        ui_g_static_1.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_g_static_1.data[i].operate_tpyel = 0;
    }

    ui_g_static_chaodiankuang->figure_tpye = 1;
    ui_g_static_chaodiankuang->layer = 0;
    ui_g_static_chaodiankuang->start_x = 31;
    ui_g_static_chaodiankuang->start_y = 781;
    ui_g_static_chaodiankuang->color = 5;
    ui_g_static_chaodiankuang->width = 5;
    ui_g_static_chaodiankuang->end_x = 483;
    ui_g_static_chaodiankuang->end_y = 858;

    ui_g_static_dipanjiyaun->figure_tpye = 2;
    ui_g_static_dipanjiyaun->layer = 0;
    ui_g_static_dipanjiyaun->r = 82;
    ui_g_static_dipanjiyaun->start_x = 965;
    ui_g_static_dipanjiyaun->start_y = 118;
    ui_g_static_dipanjiyaun->color = 8;
    ui_g_static_dipanjiyaun->width = 10;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_static_1);
    SEND_MESSAGE((uint8_t *) &ui_g_static_1, sizeof(ui_g_static_1));
}

void _ui_update_g_static_1() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_static_1.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_static_1);
    SEND_MESSAGE((uint8_t *) &ui_g_static_1, sizeof(ui_g_static_1));
}

void _ui_remove_g_static_1() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_static_1.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_static_1);
    SEND_MESSAGE((uint8_t *) &ui_g_static_1, sizeof(ui_g_static_1));
}
