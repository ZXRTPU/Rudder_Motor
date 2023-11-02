#include "pid.h"
#include "main.h"

#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }
		
#define LIMIT_MIN_MAX(x,min,max) (x) = (((x)<=(min))?(min):(((x)>=(max))?(max):(x)))		//Խ���򸳱߽�ֵ

//=====================================pid��ʼ���ṹ��===================================
void pid_init(pid_struct_t *pid,   fp32 PID[3], fp32 max_out, fp32 max_iout)   //pid�ṹ���ʼ���������������ֱ�����������ֵ���������ֵ
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
   
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}


//=====================================��ͨpid���ֵ���㺯��===================================
fp32 pid_calc(pid_struct_t *pid, fp32 fdb, fp32 set)   //�ڶ�������Ϊ����ֵ������������ΪĿ��ֵ   
{
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = fdb;
    pid->error[0] = set - fdb;

    pid->Pout = pid->Kp * pid->error[0];
    pid->Iout += pid->Ki * pid->error[0];

    pid->Dbuf[1] = pid->Dbuf[0];
    pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
    pid->Dout = pid->Kd * pid->Dbuf[0];
    LimitMax(pid->Iout, pid->max_iout);
    pid->out = pid->Pout + pid->Iout + pid->Dout;
    LimitMax(pid->out, pid->max_out);
    return pid->out;
}

//=====================================GM6020���pid���ֵ���㺯��===================================
//==========================˵����GM6020�����ͨ��CAN���߿��Ƶ�ѹ
float pid_pitch_calc(pid_struct_t *pid, float ref, float fdb)//ref��Ŀ��ֵ,fdb�ǵ��������ٶȷ���ֵ
{
  float err;
  pid->set = ref;
  pid->fdb = fdb;
  pid->error[1] = pid->error[0];//err[1]����һ�μ�������Ĳ�ֵ
	
	err = pid->set - pid->fdb;
	
	//���㴦��
	if(err > 8191/2)
	{
		err -= 8191;
	}
	else if(err < -8191/2)
	{
		err += 8191;
	}
	
	
  pid->error[0] = err;//err[0]����һ�ε�Ԥ���ٶȺ�ʵ���ٶȵĲ�ֵ,������ֵ�ǿ����Ǹ�����
	
  pid->Pout  = pid->Kp * pid->error[0];//40 3 0�Ǳ�׼ֵ��������ӵ�watch1����
  pid->Iout += pid->Ki * pid->error[0];
  pid->Dout  = pid->Kd * (pid->error[0] - pid->error[1]);
  LimitMax(pid->out, pid->max_out);
  
  pid->out = pid->Pout + pid->Iout + pid->Dout;
  LimitMax(pid->out, pid->max_out);
  return pid->out;//������صı�����ת�ٺ�ת�ص���������ֻ�ܷ���ѹֵ(-30000��30000)���е�������PID
}