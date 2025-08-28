//
// Created by RM UI Designer
//

#include "ui_brain.h"

#define FRAME_ID 0
#define GROUP_ID 8
#define START_ID 0
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_g_braini_0;
ui_interface_round_t *ui_g_brain_zhunxing = (ui_interface_round_t *)&(ui_g_braini_0.data[0]);

void _ui_init_g_brain_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_braini_0.data[i].figure_name[0] = FRAME_ID;
        ui_g_braini_0.data[i].figure_name[1] = GROUP_ID;
        ui_g_braini_0.data[i].figure_name[2] = i + START_ID;
        ui_g_braini_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_g_braini_0.data[i].operate_tpyel = 0;
    }

    ui_g_brain_zhunxing->figure_tpye = 2;
    ui_g_brain_zhunxing->layer = 0;
    ui_g_brain_zhunxing->r = 1;
    ui_g_brain_zhunxing->start_x = 959;
    ui_g_brain_zhunxing->start_y = 478;
    ui_g_brain_zhunxing->color = 8;
    ui_g_brain_zhunxing->width = 10;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_braini_0);
    SEND_MESSAGE((uint8_t *) &ui_g_braini_0, sizeof(ui_g_braini_0));
}

void _ui_update_g_brain_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_braini_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_braini_0);
    SEND_MESSAGE((uint8_t *) &ui_g_braini_0, sizeof(ui_g_braini_0));
}

void _ui_remove_g_brain_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_braini_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_braini_0);
    SEND_MESSAGE((uint8_t *) &ui_g_braini_0, sizeof(ui_g_braini_0));
}