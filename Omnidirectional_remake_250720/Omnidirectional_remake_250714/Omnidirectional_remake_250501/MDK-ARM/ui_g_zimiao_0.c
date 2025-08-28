//
// Created by RM UI Designer
//

#include "ui_g_zimiao_0.h"
#include "brain.h"
#define FRAME_ID 0
#define GROUP_ID 4
#define START_ID 1
#define OBJ_NUM 2
#define FRAME_OBJ_NUM 2

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_g_zimiao_0;
ui_interface_line_t *ui_g_zimiao_zimiaoleft = (ui_interface_line_t *)&(ui_g_zimiao_0.data[0]);
ui_interface_line_t *ui_g_zimiao_zimiaoright = (ui_interface_line_t *)&(ui_g_zimiao_0.data[1]);


void _ui_init_g_zimiao_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_zimiao_0.data[i].figure_name[0] = FRAME_ID;
        ui_g_zimiao_0.data[i].figure_name[1] = GROUP_ID;
        ui_g_zimiao_0.data[i].figure_name[2] = i + START_ID;
        ui_g_zimiao_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_g_zimiao_0.data[i].operate_tpyel = 0;
    }

    ui_g_zimiao_zimiaoleft->figure_tpye = 0;
    ui_g_zimiao_zimiaoleft->layer = 1;
    ui_g_zimiao_zimiaoleft->start_x = 696;
    ui_g_zimiao_zimiaoleft->start_y = 343;
    ui_g_zimiao_zimiaoleft->end_x = 696;
    ui_g_zimiao_zimiaoleft->end_y = 747;
    ui_g_zimiao_zimiaoleft->color = 8;
    ui_g_zimiao_zimiaoleft->width = 5;

    ui_g_zimiao_zimiaoright->figure_tpye = 0;
    ui_g_zimiao_zimiaoright->layer = 1;
    ui_g_zimiao_zimiaoright->start_x = 1208;
    ui_g_zimiao_zimiaoright->start_y = 345;
    ui_g_zimiao_zimiaoright->end_x = 1208;
    ui_g_zimiao_zimiaoright->end_y = 749;
    ui_g_zimiao_zimiaoright->color = 8;
    ui_g_zimiao_zimiaoright->width = 5;




    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_zimiao_0);
    SEND_MESSAGE((uint8_t *) &ui_g_zimiao_0, sizeof(ui_g_zimiao_0));
}

void _ui_update_g_zimiao_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_zimiao_0.data[i].operate_tpyel = 2;
    }
		
//		if(Vision_Info.Hit_Mode != 0)
//		{
//			ui_g_zimiao_zimiaoleft->width = 5;
//			ui_g_zimiao_zimiaoleft->color = 8;
//			if(Vision_Info.Flag_Vision_Del_Small == 1)
//			{
//				ui_g_zimiao_zimiaoright->width = 5;
//				ui_g_zimiao_zimiaoright->color = 8;
//			}
//		}
		
		if(Vision_Info.Hit_Mode ==0)
		{
			ui_g_zimiao_zimiaoleft->width = 5;
			ui_g_zimiao_zimiaoright->width = 5;
			if(Vision_Info.Flag_Vision_Del_Small == 1)
			{
				ui_g_zimiao_zimiaoleft->color = 2;
				ui_g_zimiao_zimiaoright->color = 2;
			}
			else
			{
				ui_g_zimiao_zimiaoleft->color = 8;
				ui_g_zimiao_zimiaoright->color = 8;
			}
		}
		else
		{
			ui_g_zimiao_zimiaoleft->width = 0;
			ui_g_zimiao_zimiaoright->width = 0;
			ui_g_zimiao_zimiaoleft->color = 8;
			ui_g_zimiao_zimiaoright->color = 8;
		}
		
		

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_zimiao_0);
    SEND_MESSAGE((uint8_t *) &ui_g_zimiao_0, sizeof(ui_g_zimiao_0));
}

void _ui_remove_g_zimiao_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_g_zimiao_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_g_zimiao_0);
    SEND_MESSAGE((uint8_t *) &ui_g_zimiao_0, sizeof(ui_g_zimiao_0));
}
