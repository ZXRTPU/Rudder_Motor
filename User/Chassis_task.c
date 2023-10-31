#include "Chassis_task.h"
#include "cmsis_os.h"
#include "drv_can.h"
#include "rc_map.h"

pid_struct_t motor_pid_chassis[4];
motor_info_t  motor_info_chassis[8];       //电机信息结构体

fp32 motor_speed_pid [3]={30,0.5,10};   //用的原来的pid
fp32 motor_angle_pid[3]={20,0,0};

int_least16_t ZERO_pos[4]={0};//要获取零位校准时电机的编码值

/*
int16_t Temp_Vx;
int16_t Temp_Vy;

int fllowflag = 0;*/

volatile int16_t motor_speed_target[4];
volatile uint16_t motor_angle_target[4];

extern RC_ctrl_t rc_ctrl;

extern float powerdata[4];
extern uint16_t shift_flag;
int8_t chassis_mode = 1;//判断底盘状态，用于UI编写

//后定义的函数声明
void chassis_current_give();
void chassis_current_give_1() ;

double a=0;
int_least16_t b=0;

void rudder_motor_task(void const * argument)
{     
	/*
 		  for (uint8_t i = 0; i < 4; i++)
			{
        pid_init(&motor_pid_chassis[i], motor_speed_pid, 6000, 6000); //init pid parameter, kp=40, ki=3, kd=0, output limit = 16384
				
			} */
	  ZERO_pos[0]=0;
	  ZERO_pos[1]=0;
    ZERO_pos[2]=0;
	  ZERO_pos[3]=0;

    for(;;)				//底盘运动任务
    {     
			//校准电机零位时，可以先将下列函数注释掉
			chassis_current_give();
			
			/*
			for(a=0;a<4096;a+=512)
			{
				while(encoder_map_8191(ZERO_pos[2],motor_info_chassis[2].rotor_angle)!=a)
				{
					chassis_current_give_1();
				}
				
				osDelay(100);
				
			}*/
 					
            osDelay(1);

    }

}


//电机电流控制
void chassis_current_give() 
{
	
    uint8_t i=0;
        
    for(i=0 ; i<4; i++)
    {
			  pid_init(&motor_pid_chassis[i], motor_angle_pid, 10000, 10000);
			  motor_speed_target[i]=pid_pitch_calc(&motor_pid_chassis[i],
			                                        encoder_map_8191(ZERO_pos[i],motor_info_chassis[i].rotor_angle),
			                                         get_xy_angle_8191(ZERO_pos[2]));
			
			  a=get_xy_angle_8191(ZERO_pos[2]);
			  b=encoder_map_8191(ZERO_pos[2],motor_info_chassis[2].rotor_angle);
			
			  if((get_x_ch1()==0)&&(get_y_ch0()==0))
				{
					motor_speed_target[i]=0;
				}
			
			  pid_init(&motor_pid_chassis[i], motor_speed_pid, 6000, 6000);
        motor_info_chassis[i].set_current = pid_calc(&motor_pid_chassis[i], motor_info_chassis[i].rotor_speed,motor_speed_target[i]);
    }
    	
		set_motor_current_can2(0, motor_info_chassis[0].set_current, motor_info_chassis[1].set_current, motor_info_chassis[2].set_current, motor_info_chassis[3].set_current);

}

/*
void chassis_current_give_1() 
{
	
    uint8_t i=0;
        
    for(i=0 ; i<4; i++)
    {
			  pid_init(&motor_pid_chassis[i], motor_angle_pid, 10000, 10000);
			  motor_speed_target[i]=pid_pitch_calc(&motor_pid_chassis[i],
			                                        encoder_map_8191(ZERO_pos[i],motor_info_chassis[i].rotor_angle),
			                                         a);
			
			  if((get_x_ch1()==0)&&(get_y_ch0()==0))
				{
					motor_speed_target[i]=0;
				}
			
			  pid_init(&motor_pid_chassis[i], motor_speed_pid, 6000, 6000);
        motor_info_chassis[i].set_current = pid_calc(&motor_pid_chassis[i], motor_info_chassis[i].rotor_speed,motor_speed_target[i]);
    }
    	
		set_motor_current_can2(0, motor_info_chassis[0].set_current, motor_info_chassis[1].set_current, motor_info_chassis[2].set_current, motor_info_chassis[3].set_current);

}
*/

//速度限制函数
  void Motor_Speed_limiting(volatile int16_t *motor_speed,int16_t limit_speed)  
{
    uint8_t i=0;
    int16_t max = 0;
    int16_t temp =0;
    int16_t max_speed = limit_speed;
    fp32 rate=0;
    for(i = 0; i<4; i++)
    {
      temp = (motor_speed[i]>0 )? (motor_speed[i]) : (-motor_speed[i]);//求绝对值
		
      if(temp>max)
        {
          max = temp;
        }
     }	
	
    if(max>max_speed)
    {
          rate = max_speed*1.0/max;   //*1.0转浮点类型，不然可能会直接得0   *1.0 to floating point type, otherwise it may directly get 0
          for (i = 0; i < 4; i++)
        {
            motor_speed[i] *= rate;
        }

    }

}




