#include "heat_control.h"

#include "referee.h"

#include "load.h"
Heat_Info Muzzle;
 int test = 60; // 60 爆发  // 40 冷却
 float Heat_Future;
uint8_t IfOverHeat(void)
{
	
	
	Muzzle.on_time_heat = referee2024.power_heat_data_t.shooter_17mm_1_barrel_heat;	
	
//	if((referee2024.robot_status_t.robot_level * 5 + 35) == referee2024.robot_status_t.shooter_barrel_cooling_value || referee2024.robot_status_t.shooter_barrel_cooling_value > 60)
//		Heat_Future = referee2024.power_heat_data_t.shooter_17mm_1_barrel_heat ;
//	else
		Heat_Future = referee2024.power_heat_data_t.shooter_17mm_1_barrel_heat + 10;

	Heat_Future = referee2024.power_heat_data_t.shooter_17mm_1_barrel_heat ;

	if(Heat_Future >= referee2024.robot_status_t.shooter_barrel_heat_limit)
	{
		Muzzle.heat_status = 0;//热量不足
		
		return 0;
//		return 1;//测试
	}
	else
	{
		Muzzle.heat_status = 1;
		
		return 1;
	}
	
}

/* 热量计 */

float Heat_Control()
{
	Muzzle.Heat_cnt++;
	
	Muzzle.coolvalue_100 = referee2024.robot_status_t.shooter_barrel_cooling_value; // 100ms结算冷却值
		/* 掉速记热量损耗 */
	if((Loader.LeftFriction.motor3508.Data.SpeedRPM < Muzzle.Speed_Bride && Muzzle.Last_SpeedRPM_Left > Muzzle.Speed_Bride)) Muzzle.Number_100++;
	
	if(Muzzle.Heat_cnt%100==0)
	{
		Muzzle.Heat_Consumption = Muzzle.Heat_Consumption + (10 * (Muzzle.Number_100 -  Muzzle.Last_Number_100) - Muzzle.coolvalue_100) ;
		if(Muzzle.Heat_Consumption <0 )	Muzzle.Heat_Consumption = 0;
	}
	
	Muzzle.Last_SpeedRPM_Left = Loader.LeftFriction.motor3508.Data.SpeedRPM;
	Muzzle.Last_SpeedRPM_Right = Loader.RightFriction.motor3508.Data.SpeedRPM;
	Muzzle.Last_Level = referee2024.robot_status_t.robot_level;
	Muzzle.Last_Heat_limt = referee2024.robot_status_t.shooter_barrel_heat_limit;
	Muzzle.Last_Number_100 = Muzzle.Number_100;
	
	if(Muzzle.Heat_limt - Muzzle.Heat_Consumption <0)
	{		
		Muzzle.heat_status = 0;
		return 0 ;
	}
	else 
	{
		Muzzle.heat_status = 1;
		return  1;
	}
}

