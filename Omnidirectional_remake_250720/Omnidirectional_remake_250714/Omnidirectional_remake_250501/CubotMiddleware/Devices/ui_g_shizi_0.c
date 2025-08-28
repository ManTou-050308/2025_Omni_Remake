//
// Created by RM UI Designer
//

#include "ui_g_shizi_0.h"

#define FRAME_ID 0
#define GROUP_ID 0
#define START_ID 0
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_g_shizi_0;
ui_interface_line_t *ui_g_shizi_shizi1 = (ui_interface_line_t *)&(ui_g_shizi_0.data[0]);
ui_interface_line_t *ui_g_shizi_shizi2 = (ui_interface_line_t *)&(ui_g_shizi_0.data[1]);
ui_interface_line_t *ui_g_shizi_shizi3 = (ui_interface_line_t *)&(ui_g_shizi_0.data[2]);
ui_interface_line_t *ui_g_shizi_shizi4 = (ui_interface_line_t *)&(ui_g_shizi_0.data[3]);
ui_interface_round_t *ui_g_shizi_zhunxing = (ui_interface_round_t *)&(ui_g_shizi_0.data[4]);

void _ui_init_g_shizi_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_shizi_0.data[i].figure_name[0] = FRAME_ID;
        ui_g_shizi_0.data[i].figure_name[1] = GROUP_ID;
        ui_g_shizi_0.data[i].figure_name[2] = i + START_ID;
        ui_g_shizi_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_g_shizi_0.data[i].operate_tpyel = 0;
    }

    ui_g_shizi_shizi1->figure_tpye = 0;
    ui_g_shizi_shizi1->layer = 1;
    ui_g_shizi_shizi1->start_x = 959;
    ui_g_shizi_shizi1->start_y = 495;
    ui_g_shizi_shizi1->end_x = 959;
    ui_g_shizi_shizi1->end_y = 495;//ui_g_shizi_shizi1->end_y = 540;
    ui_g_shizi_shizi1->color = 3;
    ui_g_shizi_shizi1->width = 5;

    ui_g_shizi_shizi2->figure_tpye = 0;
    ui_g_shizi_shizi2->layer = 1;
    ui_g_shizi_shizi2->start_x = 959;
    ui_g_shizi_shizi2->start_y = 422;
    ui_g_shizi_shizi2->end_x = 959;
    ui_g_shizi_shizi2->end_y = 422;//ui_g_shizi_shizi2->end_y = 467;
    ui_g_shizi_shizi2->color = 3;
    ui_g_shizi_shizi2->width = 5;

    ui_g_shizi_shizi3->figure_tpye = 0;
    ui_g_shizi_shizi3->layer = 1;
    ui_g_shizi_shizi3->start_x = 903;
    ui_g_shizi_shizi3->start_y = 478;
    ui_g_shizi_shizi3->end_x = 903;//ui_g_shizi_shizi3->end_x = 948;
    ui_g_shizi_shizi3->end_y = 478;
    ui_g_shizi_shizi3->color = 3;
    ui_g_shizi_shizi3->width = 5;

    ui_g_shizi_shizi4->figure_tpye = 0;
    ui_g_shizi_shizi4->layer = 1;
    ui_g_shizi_shizi4->start_x = 974;
    ui_g_shizi_shizi4->start_y = 478;
    ui_g_shizi_shizi4->end_x = 974;//ui_g_shizi_shizi4->end_x = 1019;
    ui_g_shizi_shizi4->end_y = 478;
    ui_g_shizi_shizi4->color = 3;
    ui_g_shizi_shizi4->width = 5;

    ui_g_shizi_zhunxing->figure_tpye = 2;
    ui_g_shizi_zhunxing->layer = 0;
    ui_g_shizi_zhunxing->r = 10;
    ui_g_shizi_zhunxing->start_x = 940;
    ui_g_shizi_zhunxing->start_y = 500;
    ui_g_shizi_zhunxing->color = 3;
    ui_g_shizi_zhunxing->width = 5;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_shizi_0);
    SEND_MESSAGE((uint8_t *) &ui_g_shizi_0, sizeof(ui_g_shizi_0));
}

void _ui_update_g_shizi_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_shizi_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_shizi_0);
    SEND_MESSAGE((uint8_t *) &ui_g_shizi_0, sizeof(ui_g_shizi_0));
}

void _ui_remove_g_shizi_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_shizi_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_shizi_0);
    SEND_MESSAGE((uint8_t *) &ui_g_shizi_0, sizeof(ui_g_shizi_0));
}
