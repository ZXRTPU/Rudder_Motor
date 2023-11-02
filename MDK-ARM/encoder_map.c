#include "encoder_map.h"

//�Ե�ǰ�Ƕȼ�rotor_angle��������׼�������ϣ����ص�ǰ�Ƕ�
int16_t encoder_map_8191(int16_t ZERO_POS,int16_t rotor_angle)
{
	int16_t k=0;
	 k=ZERO_POS;
	
	//n=uint16_t rotor_angle;
  int16_t n=0;
	n=rotor_angle;
	
	if(n>(k+4096))
	{
		n=n-8192;
	}
	else if((k<=n)&&(n<=(k+4096)))
	{
		n=n-k;
	}
	else if(n<(k-4096))
	{
		n=n+8192;
	}
	else if(((k-4096)<=n)&&(n<k))
	{
		n=n-k;
	}

	return n;
}
