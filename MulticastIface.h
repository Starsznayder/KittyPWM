#ifndef MULTICASTIFACE_H
#define MULTICASTIFACE_H

#include <Udp.h>
#include "GlobalParameters.h"

class MulticastIface
{
public:

	static MulticastIface & get()
	{
		static MulticastIface singleton;
		return singleton;
	}  

	void run()
	{
		Parameters & params = Parameters::get();
		if (params.config.enableMulticast)
		{
			int packetSize = Udp.parsePacket();
			if (packetSize >= sizeof(PowerInfoMSG) && packetSize <= sizeof(PowerInfoMSG) + 4)
			{
				uint8_t* ptr = new uint8_t[packetSize];  
				Udp.read((unsigned char*)ptr, packetSize);
				PowerInfoMSG m = *(PowerInfoMSG*)(ptr);
				if (m.isMagic())
				{
					updatePWMbyMulticastData(m);    
				}
				delete[] ptr;
			}
		}
	}

	void init()
	{
		Udp.beginMulticast(Parameters::get().config.multicastip(), Parameters::get().config.multicastport);
	}	

private:
	EthernetUDP Udp;

  MulticastIface(){}
	MulticastIface( const MulticastIface & );
	
	void updatePWMbyMulticastData(const PowerInfoMSG& m)
	{
		for(int i = 0; i < NUM_PWM_PINS; ++i)
		{
			int powerDiff = Parameters::get().config.targetPower[i] - m.L[Parameters::get().config.referencePhaseIndex[i]];
			if(powerDiff < 100 && powerDiff > 20)
			{
			    Parameters::get().setPwmVal(i, Parameters::get().pwmVal[i] + Parameters::get().config.updateStepLo100[i]);
			}
			else if(powerDiff > -100 && powerDiff < -20)
			{
			    Parameters::get().setPwmVal(i, Parameters::get().pwmVal[i] - Parameters::get().config.updateStepLo100[i]);
			}
			else if(powerDiff > 100)
			{
			    Parameters::get().setPwmVal(i, Parameters::get().pwmVal[i] + Parameters::get().config.updateStepGt100[i]);
			}
			else if(powerDiff < -100)
			{
			    Parameters::get().setPwmVal(i, Parameters::get().pwmVal[i] - Parameters::get().config.updateStepGt100[i]);
			}
		}
	}
	
};

#endif