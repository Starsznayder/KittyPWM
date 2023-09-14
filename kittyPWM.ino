#include <SPI.h>
#include "PowerInfoMSG.h"
#include "HTTPIface.h"
#include "MulticastIface.h"

static constexpr byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFD };

void setup()
{
  delay(100);
  //Serial.begin(9600);
  delay(100);
  Parameters& params = Parameters::get();

  if (params.config.ip() == INADDR_NONE)
  {
    while(!Ethernet.begin(mac))
    {
      delay(1000);
    }
  }
  else
  {
    Ethernet.begin(mac, params.config.ip());
    Ethernet.setGatewayIP(params.config.gateway());
    Ethernet.setSubnetMask(params.config.mask());
  }

  HTTPIface::get().init();
  MulticastIface::get().init();
  delay(100);
}

void loop()
{
  Parameters & params = Parameters::get();
  MulticastIface::get().run();
  HTTPIface::get().runWebsite();
}