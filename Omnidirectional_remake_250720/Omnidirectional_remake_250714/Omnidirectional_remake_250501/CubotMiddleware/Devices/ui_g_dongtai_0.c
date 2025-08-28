//
// Created by RM UI Designer
//

#include "ui_g_dongtai_0.h"
#include "string.h"
#include "hardware_config.h"
#include "brain.h"
#define FRAME_ID 0
#define GROUP_ID 6
#define START_ID 0

ui_string_frame_t ui_g_dongtai_0;

ui_interface_string_t* ui_g_dongtai_elastance = &ui_g_dongtai_0.option;

void _ui_init_g_dongtai_0() {
    ui_g_dongtai_0.option.figure_name[0] = FRAME_ID;
    ui_g_dongtai_0.option.figure_name[1] = GROUP_ID;
    ui_g_dongtai_0.option.figure_name[2] = START_ID;
    ui_g_dongtai_0.option.operate_tpyel = 1;
    ui_g_dongtai_0.option.figure_tpye = 7;
    ui_g_dongtai_0.option.layer = 0;
    ui_g_dongtai_0.option.font_size = 30;
    ui_g_dongtai_0.option.start_x =  939;
    ui_g_dongtai_0.option.start_y = 680;
    ui_g_dongtai_0.option.color = 2;
    ui_g_dongtai_0.option.str_length = 6;
    ui_g_dongtai_0.option.width = 3;
    strcpy(ui_g_dongtai_elastance->string, "0");

    ui_proc_string_frame(&ui_g_dongtai_0);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_0, sizeof(ui_g_dongtai_0));
}

void _ui_update_g_dongtai_0() {
    ui_g_dongtai_0.option.operate_tpyel = 2;
	
		if(referee2024.projectile_allowance_t.projectile_allowance_17mm < 50) ui_g_dongtai_elastance->color = 5;
		else ui_g_dongtai_elastance->color = 2;
	
		sprintf(ui_g_dongtai_elastance->string, "%d",referee2024.projectile_allowance_t.projectile_allowance_17mm   );
	
    ui_proc_string_frame(&ui_g_dongtai_0);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_0, sizeof(ui_g_dongtai_0));
}

void _ui_remove_g_dongtai_0() {
    ui_g_dongtai_0.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_dongtai_0);
    SEND_MESSAGE((uint8_t *) &ui_g_dongtai_0, sizeof(ui_g_dongtai_0));
}
