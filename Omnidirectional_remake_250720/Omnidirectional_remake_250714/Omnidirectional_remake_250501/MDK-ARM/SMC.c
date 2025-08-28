#include "SMC.h"

SMC_Object   smc_yaw;
/**
  * @brief SMC³õÊ¼»¯
  */
void SMC_Init(SMC_Object* smc, float C, float K, float error_eps, float J,float epsilon)
{
	smc->C = C;
	smc->K = K;
	smc->J = J;
	smc->error_eps = error_eps;
	smc->epsilon  = epsilon;
	
}

float  Signal(float y)
{
	if (y > 0)
		return 1;
	else if (y == 0)
		return 0;
	else
		return -1;
}

float Sat(float y)
{
	if (fabs(y) <= 1)
		return y;
	else
		return Signal(y);
}
	

float SMC_2(float target,float feedback_angle,float feedback_anglespeed,SMC_Object* smc)
{
	smc->error = feedback_angle - target;
	
	smc->ddref = (target - smc->ref_last) - smc->dref ;
	smc->dref  = (target - smc->ref_last) ;
	smc->error_integral += smc->error;
	if(fabs(smc->error) < smc->error_eps)
	{
		smc ->out = 0;
		return 0 ;
	}
	
	smc->s   = smc->C1 * smc->error + (feedback_anglespeed - smc->dref) + smc->C2 * smc->error_integral;
	smc->out = smc->J * (smc->ddref -
	                     smc->C * (feedback_anglespeed - smc->dref) -
	                     smc->epsilon * Sat(smc->s) - 
											 smc->K * smc->s);
	
	smc->ref_last = target;
	smc->error_last = smc->error;
	return smc->out;
}

float SMC(float target,float feedback_angle,float feedback_anglespeed,SMC_Object* smc)
{
	smc->error = feedback_angle - target;
	smc->ddref = (target - smc->ref_last) - smc->dref ;
	smc->dref  = (target - smc->ref_last) ;
	
	if(fabs(smc->error) < smc->error_eps)
	{
		smc ->out = 0;
		return 0 ;
	}
	
	smc->s   = smc->C * smc->error + (feedback_anglespeed - smc->dref) ;
	smc->out = smc->J * (smc->ddref -
	                     smc->C * (feedback_anglespeed - smc->dref) -
	                     smc->epsilon * Sat(smc->s) - 
											 smc->K * smc->s);
	
	smc->ref_last = target;
	return smc->out;
}