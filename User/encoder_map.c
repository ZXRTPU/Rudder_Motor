#include "encoder_map.h"

//对当前角度即rotor_angle整定到标准编码盘上，返回当前角度
uint16_t encoder_map_8191(uint16_t ZERO_POS,uint16_t rotor_angle)
{
	uint16_t k=ZERO_POS;
	
	//n=uint16_t rotor_angle;
  uint16_t n=rotor_angle;
	
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
	else if(((k-4096)<=n)&&n<k)
	{
		n=n-k;
	}
	else
	{
		
	}
	
	return n;
}