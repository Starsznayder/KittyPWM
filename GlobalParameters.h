#ifndef GLOBALPARAMETERS_H
#define GLOBALPARAMETERS_H

#include <Ethernet.h>
#include <EEPROM.h>
#define NUM_PWM_PINS 2

struct PermanentParams
{
  uint8_t referencePhaseIndex[NUM_PWM_PINS];
  int32_t targetPower[NUM_PWM_PINS];
  uint8_t updateStepLo100[NUM_PWM_PINS];
  uint8_t updateStepGt100[NUM_PWM_PINS];
  uint8_t ipAsByte[4];
  uint8_t maskAsByte[4];
  uint8_t gatewayAsByte[4];
  uint8_t multicastipAsByte[4];
  bool enableMulticast;
  uint32_t multicastport;

  IPAddress ip() {return IPAddress(ipAsByte[0], ipAsByte[1], ipAsByte[2], ipAsByte[3]);};
  IPAddress mask() {return IPAddress(maskAsByte[0], maskAsByte[1], maskAsByte[2], maskAsByte[3]);};
  IPAddress gateway() {return IPAddress(gatewayAsByte[0], gatewayAsByte[1], gatewayAsByte[2], gatewayAsByte[3]);};
  IPAddress multicastip() {return IPAddress(multicastipAsByte[0], multicastipAsByte[1], multicastipAsByte[2], multicastipAsByte[3]);};
  void ip(const IPAddress& val) {for (int i = 0; i < 4; ++i) ipAsByte[i] = val[i];};
  void mask(const IPAddress& val) {for (int i = 0; i < 4; ++i) maskAsByte[i] = val[i];};
  void gateway(const IPAddress& val) {for (int i = 0; i < 4; ++i) gatewayAsByte[i] = val[i];};
  void multicastip(const IPAddress& val) {for (int i = 0; i < 4; ++i) multicastipAsByte[i] = val[i];};

  PermanentParams() : referencePhaseIndex({2,0}),
                      targetPower({1000,0,}),
                      updateStepLo100({1,0}),
                      updateStepGt100({20,0}),
                      ipAsByte({0, 0, 0, 0}),
                      maskAsByte({0, 0, 0, 0}),
                      gatewayAsByte({0, 0, 0, 0}),
                      multicastipAsByte({235, 0, 0, 51}),
                      enableMulticast(true),
                      multicastport(50111) {}

  bool operator==(const PermanentParams& m)
  {
    for (int i = 0; i < NUM_PWM_PINS; ++i)
		{
      if (referencePhaseIndex[i] != m.referencePhaseIndex[i] ||
          targetPower[i] != m.targetPower[i] ||
          updateStepLo100[i] != m.updateStepLo100[i] ||
          updateStepGt100[i] != m.updateStepGt100[i])
      {
        return false;
      }
		}

    for (int i = 0; i < 4; ++i)
		{
      if (ipAsByte[i] != m.ipAsByte[i] ||
          maskAsByte[i] != m.maskAsByte[i] ||
          gatewayAsByte[i] != m.gatewayAsByte[i] ||
          multicastipAsByte[i] != m.multicastipAsByte[i])
      {
        return false;
      }
		}

    return enableMulticast == m.enableMulticast && multicastport == m.multicastport;
  }

  bool operator!=(const PermanentParams& m)
  {
    return !(*this == m); 
  }
};

class Parameters
{
public:
const int pwmPinMap[NUM_PWM_PINS];
int pwmVal[NUM_PWM_PINS];
PermanentParams config;

void updateConfig(const PermanentParams& _config)
{
  if(config != _config)
  {
    config = _config;
    saveToEEPROM();
  }
}

static Parameters & get()
{
    static Parameters singleton;
    return singleton;
}

void setPwmVal(int idx, int32_t newVal)
{
  if (idx >= NUM_PWM_PINS)
  {
		return;
  }

	if (newVal > 255)
	{
		newVal = 255;
	}
	
	if (newVal < 0)
	{
		newVal = 0;
	}

	if (newVal != pwmVal[idx])
	{
		analogWrite(Parameters::get().pwmPinMap[idx], newVal);
		pwmVal[idx] = newVal;
	}
} 

private:
    static uint32_t getPrefix() {return 3459246;}

    void saveToEEPROM()
    {
      byte* ptr = (byte*)(&config);
      uint32_t prefix = getPrefix();
      byte* prefixPtr = (byte*)&prefix;
      EEPROM.put(EEPROM.begin(), prefix);
      EEPROM.put(EEPROM.begin() + sizeof(uint32_t), config);
    } 

    Parameters() : pwmPinMap({3,5}),
                   pwmVal({0,0})
	{
    uint32_t prefix;
    EEPROM.get(EEPROM.begin(), prefix);

    if (prefix == getPrefix())
    {
      PermanentParams _config;
      EEPROM.get(EEPROM.begin() + sizeof(uint32_t), config);
    }

		for (int i = 0; i < NUM_PWM_PINS; ++i)
		{
      pwmVal[i] = 0;
			pinMode(pwmPinMap[i], OUTPUT); 
			analogWrite (pwmPinMap[i], 0);
		}  
	}
	Parameters(const Parameters &);
};

#endif