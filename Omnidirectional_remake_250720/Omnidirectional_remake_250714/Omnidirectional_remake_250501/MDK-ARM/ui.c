#include "ui.h"
#include "driver_timer.h"
#include "referee.h"
#include "brain.h"
void UI_open()
{	
	if(referee2024.game_status_t.game_progress != 4) Load_Number = 0;
	
	ui_self_id = referee2024.robot_status_t.robot_id;
	if(tim14.UI_Time < 5000 && tim14.UI_Time > 10)
	{
		
		
		if(tim14.UI_Time % 500 == 10) 	ui_init_g_shizi();
		if(tim14.UI_Time % 500 == 33) 	_ui_init_g_roll_1();
		if(tim14.UI_Time % 500 == 50)		_ui_init_g_static_0();
		if(tim14.UI_Time % 500 == 100)	_ui_init_g_static_1();
		if(tim14.UI_Time % 500 == 120)	_ui_init_g_zimiao_0();
//		if(tim14.UI_Time % 500 == 150)	_ui_brain();		
		if(tim14.UI_Time % 500 == 200)	_ui_init_g_dongtai_1();
		if(tim14.UI_Time % 500 == 250)	_ui_init_g_dongtai_2();
		if(tim14.UI_Time % 500 == 350)	_ui_init_g_danliang_1();
		if(tim14.UI_Time % 500 == 300)	_ui_init_g_danliang_0();
		if(tim14.UI_Time % 500 == 400)	_ui_init_g_danliang_2();
		if(tim14.UI_Time % 500 == 450)	_ui_init_g_danliang_3();
		if(tim14.UI_Time % 500 == 480)	ui_init_g_shishidanliang();

	}
	else
	{
		
//		if(tim14.UI_Time % 200 == 10)		_ui_update_g_dongtai_0();
		if(tim14.UI_Time % 200 == 50)		_ui_update_g_dongtai_1();
		if(tim14.UI_Time % 200 == 100)	_ui_update_g_dongtai_2();
		if(tim14.UI_Time % 200 == 150)	ui_update_g_shishidanliang();
		if(tim14.UI_Time % 10  ==  0)	_ui_update_g_zimiao_0();
		if(tim14.UI_Time % 500  == 330)	  _ui_update_g_roll();
	}
}