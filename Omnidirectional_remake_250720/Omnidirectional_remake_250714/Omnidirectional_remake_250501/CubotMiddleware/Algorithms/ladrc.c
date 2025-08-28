#include "LADRC.h"
#include "filter.h"

LADRC_NUM   M6020_Angle[4];    //ϵͳ��Ҫ�����Ĳ��� 
LADRC_NUM   M3508_Speed; 
LADRC_NUM   M6020_Angle_Vision; 
LADRC_NUM   M6020_Angle_Yaw; 
LADRC_NUM   M2006_Speed; 

/**
   *@Brief default ������
   *@Brief ���ݾ��� ts��0.2~0.3֮�� Wo��Wcѡ�����С��������b0
   *@Date@WangShun 2022-05-28 2022-07-03����
---------------------------------------------------
		      LADRC default������									
---------------------------------------------------
---------------------------------------------------
	ts	|	h	|	r	|   wc   |   w0  |	b0
---------------------------------------------------
	0.1	|	h	|	r	|  100   |  400  |	b0
---------------------------------------------------
   0.157|	h	|	r	|   64   |  224~255  |	b0
---------------------------------------------------
   0.158|	h	|	r	|   63   |  253  |	b0
---------------------------------------------------
   0.159|	h	|	r	|   63   |  252  |	b0
---------------------------------------------------
	0.16|	h	|	r	|   63   |  250  |	b0
---------------------------------------------------
	0.17|	h	|	r	|   59   |  235  |	b0
---------------------------------------------------
	0.18|	h	|	r	|   56   |  222  |	b0
---------------------------------------------------
	0.2	|	h	|	r	|   50   |  200  |	b0
---------------------------------------------------
	0.21|	h	|	r	|   48   |  190  |	b0
---------------------------------------------------
	0.22|	h	|	r	|   45   |  182  |	b0
---------------------------------------------------
	0.23|	h	|	r	|   43   |  174  |	b0
---------------------------------------------------
	0.24|	h	|	r	|   42   |  167  |	b0
---------------------------------------------------
	0.25|	h	|	r	|   40   |  160  |	b0
---------------------------------------------------
	0.26|	h	|	r	|   38   |  154  |	b0
---------------------------------------------------
	0.27|	h	|	r	|   37   |  148  |	b0
---------------------------------------------------
	0.28|	h	|	r	|   36   |  144  |	b0
---------------------------------------------------
	0.29|	h	|	r	|   34   |  138  |	b0
---------------------------------------------------
	0.3	|	h	|	r	|   33   |  133  |	b0
---------------------------------------------------
	0.4	|	h	|	r	|   25   |  100  |	b0
---------------------------------------------------
	0.5	|	h	|	r	|   20   |   80  |	b0
---------------------------------------------------
--------------------------------------------------- 
*/
 

const double LADRC_Unit[5][7]=
{
	{0.002,22,63,252,45},//3508�Ƕ�
	{0.001,1800,6,0.5,0.0006,0.5},//6020�Ƕ�
	{0.001,1800,6,0.5,0.0006,0.5},//6020�Ƕ�
};
void LADRC_Init(LADRC_NUM *LADRC_TYPE,float h,float r,float kp,float kd,float b0,float beta,float ki,float kiPartDetachment)
{	
		LADRC_TYPE->h = h; //��ʱʱ�估ʱ�䲽��
		LADRC_TYPE->r = r; //�����ٶȲ���
		LADRC_TYPE->Kp = kp; //�۲�������
		LADRC_TYPE->Kd = kd; //�۲�������
		LADRC_TYPE->b0 = b0; //ϵͳ����
	  LADRC_TYPE->beta = beta; //״̬�����ʴ���	
		LADRC_TYPE->Ki = ki; //״̬�����ʴ���	
		LADRC_TYPE->KiPartDetachment = kiPartDetachment;
}

void LADRC_REST(LADRC_NUM *LADRC_TYPE)
{
		LADRC_TYPE->z1 = 0; //��ʱʱ�估ʱ�䲽��
    LADRC_TYPE->z2 = 0; //�����ٶȲ���
    LADRC_TYPE->z3 = 0; //�۲�������

}

void LADRC_TD(LADRC_NUM *LADRC_TYPE,float Expect)
{
    double fh= -LADRC_TYPE->r * LADRC_TYPE->r * (LADRC_TYPE->v1 - Expect) - 2 * LADRC_TYPE->r * LADRC_TYPE->v2;
    LADRC_TYPE->v1 += LADRC_TYPE->v2 * LADRC_TYPE->h;
    LADRC_TYPE->v2 += fh * LADRC_TYPE->h;//΢����
}

void LADRC_ESO(LADRC_NUM *LADRC_TYPE,double y,double u)
{	

	double z1k,z2k,z3k;
	z1k = LADRC_TYPE->z1;
	z2k = LADRC_TYPE->z2;
	z3k = LADRC_TYPE->z3;
	
	LADRC_TYPE->z1 = LADRC_TYPE->h * z2k + square(LADRC_TYPE->h) * z3k/2 - y * (3 * LADRC_TYPE->beta - 3) + z1k * (3 * LADRC_TYPE->beta - 2) + LADRC_TYPE->b0 * square(LADRC_TYPE->h) * u / 2;
	LADRC_TYPE->z2 = z2k + LADRC_TYPE->h * z3k + LADRC_TYPE->b0 * LADRC_TYPE->h * u + (y * square(LADRC_TYPE->beta - 1) * (LADRC_TYPE->beta + 5)) / (2 * LADRC_TYPE->h) - (z1k * square(LADRC_TYPE->beta - 1) * (LADRC_TYPE->beta + 5))/(2 * LADRC_TYPE->h);
	LADRC_TYPE->z3 = z3k - (y * cube(LADRC_TYPE->beta - 1))/square(LADRC_TYPE->h) + (z1k * cube(LADRC_TYPE->beta - 1))/square(LADRC_TYPE->h);
}

void LADRC_LF_Angle(LADRC_NUM *LADRC_TYPE,double Measure,double DevMeasure)
{
//    float Kp=LADRC_TYPE->wc * LADRC_TYPE->wc;
//    float Kd=2 * LADRC_TYPE->wc;
	/**
       *@Brief  ���Կ�����������kd = 2wc
       *@Before Kd=3*LADRC_TYPE->wc;
       *@Now    Kd=2*LADRC_TYPE->wc;
       *@WangShun  2022-04-27  ע��
       */
    double e1=LADRC_TYPE->v1 - Measure;
    double e2= - DevMeasure;
    double u0=LADRC_TYPE->Kp * e1 + LADRC_TYPE->Kd * e2;
		LADRC_TYPE->u0 = u0;
    LADRC_TYPE->u = u0 - LADRC_TYPE->z3 / LADRC_TYPE->b0;
	if(fabs(e1) < 	LADRC_TYPE->KiPartDetachment)
		LADRC_TYPE->Kipart += 	LADRC_TYPE->Ki * e1;
	else 	LADRC_TYPE->Kipart = 0;
		LADRC_TYPE->u += 	LADRC_TYPE->Kipart;
}

void LADRC_LF_Speed(LADRC_NUM *	LADRC_TYPE,double Measure)
{
//    float Kp=	LADRC_TYPE->wc * 	LADRC_TYPE->wc;
//    float Kd=2 * 	LADRC_TYPE->wc;
	/**
       *@Brief  ���Կ�����������kd = 2wc
       *@Before Kd=3*	LADRC_TYPE->wc;
       *@Now    Kd=2*	LADRC_TYPE->wc;
       *@WangShun  2022-04-27  ע��
       */
    double e1=	LADRC_TYPE->v1 - Measure;
    double e2= - 	LADRC_TYPE->z2;
    double u0=	LADRC_TYPE->Kp * e1 + LADRC_TYPE->Kd * e2;
			LADRC_TYPE->u0 = u0;
    	LADRC_TYPE->u = (u0-	LADRC_TYPE->z3) / 	LADRC_TYPE->b0;
}

void LADRC_Loop_Angle(LADRC_NUM *	LADRC_TYPE,double Expect_Value,double RealTimeOut/*output*/,double Measure/*����*/,double DevMeasure)
{

    LADRC_TD(	LADRC_TYPE,Expect_Value);
    LADRC_ESO(	LADRC_TYPE,Measure,RealTimeOut); 
    LADRC_LF_Angle(	LADRC_TYPE,Measure,DevMeasure);
}

void LADRC_Loop_Speed(LADRC_NUM *	LADRC_TYPE,double Expect_Value,double RealTimeOut/*output*/,double Measure/*����*/)
{

    LADRC_TD(	LADRC_TYPE,Expect_Value);
    LADRC_ESO(LADRC_TYPE,Measure,RealTimeOut); 
    LADRC_LF_Speed(	LADRC_TYPE,Measure);
}

