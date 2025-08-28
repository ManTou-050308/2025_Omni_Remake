//
// Created by RM UI Designer
//

#include "ui_g_dongtai_2.h"
#include "hardware_config.h"
#include "Supercap.h"
#include "holder.h"
#define FRAME_ID 0
#define GROUP_ID 2
#define START_ID 2
#define OBJ_NUM 2
#define FRAME_OBJ_NUM 2

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_g_dongtai_2;
ui_interface_rect_t *ui_g_dongtai_chaodiantiao = (ui_interface_rect_t *)&(ui_g_dongtai_2.data[0]);
ui_interface_arc_t *ui_g_dongtai_toufangxiang = (ui_interface_arc_t *)&(ui_g_dongtai_2.data[1]);

void _ui_init_g_dongtai_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_dongtai_2.data[i].figure_name[0] = FRAME_ID;
        ui_g_dongtai_2.data[i].figure_name[1] = GROUP_ID;
        ui_g_dongtai_2.data[i].figure_name[2] = i + START_ID;
        ui_g_dongtai_2.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_g_dongtai_2.data[i].operate_tpyel = 0;
    }

    ui_g_dongtai_chaodiantiao->figure_tpye = 1;
    ui_g_dongtai_chaodiantiao->layer = 0;
    ui_g_dongtai_chaodiantiao->start_x = 60;
    ui_g_dongtai_chaodiantiao->start_y = 810;
    ui_g_dongtai_chaodiantiao->color = 8;
    ui_g_dongtai_chaodiantiao->width = 50;
    ui_g_dongtai_chaodiantiao->end_x = 455;
    ui_g_dongtai_chaodiantiao->end_y = 830;

    ui_g_dongtai_toufangxiang->figure_tpye = 4;
    ui_g_dongtai_toufangxiang->layer = 0;
    ui_g_dongtai_toufangxiang->rx = 85;
    ui_g_dongtai_toufangxiang->ry = 85;
    ui_g_dongtai_toufangxiang->start_x = 965;
    ui_g_dongtai_toufangxiang->start_y = 118;
    ui_g_dongtai_toufangxiang->color = 2;
    ui_g_dongtai_toufangxiang->width = 20;
    ui_g_dongtai_toufangxiang->start_angle = 345;
    ui_g_dongtai_toufangxiang->end_angle = 20;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_dongtai_2);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_2, sizeof(ui_g_dongtai_2));
}

void _ui_update_g_dongtai_2() {
	float angle_1;
	float angle_2;
	float buffer;
	buffer = Super_Cap.cap_state.Voltage - 15;
	
	if(buffer < 0) buffer = 0;
	else if (buffer > 10) buffer = 10;
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_dongtai_2.data[i].operate_tpyel = 2;
    }
		ui_g_dongtai_chaodiantiao->end_x = 60 + (455 - 60) * (buffer/10);
		if(buffer > 7)ui_g_dongtai_chaodiantiao->color = 2;
		else if(buffer <2 )ui_g_dongtai_chaodiantiao->color = 5;
	
		else ui_g_dongtai_chaodiantiao->color = 8;
//		ui_g_dongtai_toufangxiang->start_angle = 340 ;
//    ui_g_dongtai_toufangxiang->end_angle = 20 ;
		
		angle_1 = 20 + Holder.Motors6020.motor[Yaw_6020].Data.Angle;
		angle_2 = 340 + Holder.Motors6020.motor[Yaw_6020].Data.Angle;
		if(angle_1 > 360) angle_1 -= 360;
		if(angle_1 < 0) angle_1 += 360;
		if(angle_2 > 360) angle_2 -= 360;
		if(angle_2 < 0) angle_2 += 360;
 
		ui_g_dongtai_toufangxiang->end_angle = angle_1 ;
		ui_g_dongtai_toufangxiang->start_angle = angle_2;
//		if(ui_g_dongtai_toufangxiang->start_angle > 360) ui_g_dongtai_toufangxiang->start_angle -= 360;
//		if(ui_g_dongtai_toufangxiang->end_angle > 360) ui_g_dongtai_toufangxiang->end_angle -= 360;
//		if(ui_g_dongtai_toufangxiang->start_angle < 0) ui_g_dongtai_toufangxiang->start_angle += 360;
//		if(ui_g_dongtai_toufangxiang->end_angle < 0) ui_g_dongtai_toufangxiang->end_angle += 360;
		
    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_dongtai_2);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_2, sizeof(ui_g_dongtai_2));
}

void _ui_remove_g_dongtai_2() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_dongtai_2.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_dongtai_2);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_2, sizeof(ui_g_dongtai_2));
}
