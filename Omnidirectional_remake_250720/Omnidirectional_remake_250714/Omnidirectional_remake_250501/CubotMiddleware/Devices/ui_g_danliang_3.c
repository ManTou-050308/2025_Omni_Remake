//
// Created by RM UI Designer
//

#include "ui_g_danliang_3.h"

#define FRAME_ID 0
#define GROUP_ID 3
#define START_ID 3
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_g_danliang_3;
ui_interface_line_t *ui_g_danliang_danliangzongxain = (ui_interface_line_t *)&(ui_g_danliang_3.data[0]);

void _ui_init_g_danliang_3() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_danliang_3.data[i].figure_name[0] = FRAME_ID;
        ui_g_danliang_3.data[i].figure_name[1] = GROUP_ID;
        ui_g_danliang_3.data[i].figure_name[2] = i + START_ID;
        ui_g_danliang_3.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_g_danliang_3.data[i].operate_tpyel = 0;
    }

    ui_g_danliang_danliangzongxain->figure_tpye = 0;
    ui_g_danliang_danliangzongxain->layer = 0;
    ui_g_danliang_danliangzongxain->start_x = 1535;
    ui_g_danliang_danliangzongxain->start_y = 393;
    ui_g_danliang_danliangzongxain->end_x = 1535;
    ui_g_danliang_danliangzongxain->end_y = 675;
    ui_g_danliang_danliangzongxain->color = 1;
    ui_g_danliang_danliangzongxain->width = 8;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_danliang_3);
    SEND_MESSAGE((uint8_t *) &ui_g_danliang_3, sizeof(ui_g_danliang_3));
}

void _ui_update_g_danliang_3() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_danliang_3.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_danliang_3);
    SEND_MESSAGE((uint8_t *) &ui_g_danliang_3, sizeof(ui_g_danliang_3));
}

void _ui_remove_g_danliang_3() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_danliang_3.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_danliang_3);
    SEND_MESSAGE((uint8_t *) &ui_g_danliang_3, sizeof(ui_g_danliang_3));
}
