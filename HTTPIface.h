#ifndef HTTPIFACE_H
#define HTTPIFACE_H

#include <Server.h>
#include <Client.h>
#include "GlobalParameters.h"
#include "CustomString.h"

class HTTPIface
{
public:

	static HTTPIface & get()
	{
		static HTTPIface singleton;
		return singleton;
	}  

  int parseVal(const char* prefix, uint8_t index, const uint8_t size)
  {
    char ioStr[size + 2];
    for (uint8_t i = 0; i < size; ++i)
    {
      ioStr[i] = prefix[i];
    }
    ioStr[size] = index + '0';
    ioStr[size + 1] = '=';
    return readString.parseInt(ioStr, size + 2);
  }

	void runWebsite()
	{
		Parameters & params = Parameters::get();
    const char queryPWMCompareStr[] = {"GET /?IO0="};
    const char queryIPCompareStr[] = {"GET /?IP0="};
		EthernetClient client = server.available();
		if (client) 
		{
			while (client.connected()) 
			{
				if (client.available()) 
				{
          if (readString.push_back(client.read()) == '\n') 
					{
            if(readString.strCmp(queryPWMCompareStr, sizeof(queryPWMCompareStr) - 1))
					  {
              readString.tokenize();
              PermanentParams config = params.config;
              for (int k = 0; k < NUM_PWM_PINS; ++k)
              {
                params.setPwmVal(k, parseVal("IO", k, 2));
                config.referencePhaseIndex[k] = parseVal("RPH", k, 3) - 1;
                config.targetPower[k] = parseVal("TGR", k, 3);
                config.updateStepLo100[k] = parseVal("USL", k, 3);
                config.updateStepGt100[k] = parseVal("USH", k, 3);
              }
              params.updateConfig(config);
            }
            else if(readString.strCmp(queryIPCompareStr, sizeof(queryIPCompareStr) - 1))
            {
              readString.tokenize();
              PermanentParams config = params.config;
              IPAddress ip = html2ip("IP");
              IPAddress mask = html2ip("MA");
              IPAddress gateway = html2ip("GT");
              IPAddress multicast = html2ip("MP");
              config.ip(ip);
              config.mask(mask);
              config.gateway(gateway);
              config.multicastip(multicast);
              config.multicastport = readString.parseInt("MPO=", 4);
              config.enableMulticast = readString.indexOf("EM=on", 5) > 0;
              params.updateConfig(config);
					  }
            else
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: html");
              client.println();
              client.println("<html>");
              client.println("<head>");
              client.println("<title>KITTY PWM</title>");
              client.println("</head>");
              client.println("<body>");
              client.println("<br />");
              client.println("**********************************************");
              client.println("<br />");
              client.println("****************KITTY PWM*******************");
              client.println("<br />");
              client.println("**********************************************");
              client.println("<br /><br />");
              client.print("<form>");

              pwm2html(client);
              client.println("<br />");
              client.println("<input type=submit name=Sub1 value=Submit /> </form><br />");
              client.println("<br />");
              client.print("<form>");

              client.println("**********************************************");
              client.println("<br /><br />");

              client.println("IP: ");
              ip2html(client, params.config.ip(), "IP");
              client.println("<br /><br />");

              client.println("Mask: ");
              ip2html(client, params.config.mask(),"MA");
              client.println("<br /><br />");

              client.println("Gateway: ");
              ip2html(client, params.config.gateway(), "GT");
              client.println("<br />");

              client.println("<br />");
              client.println("**********************************************");
              client.println("<br />");

              client.println("Enable multicast: <input type=checkbox name=EM ");
              if (params.config.enableMulticast)
              {
                client.print("checked");
              }
              client.print("/> <br />");

              client.println("<br />");
              client.println("Multicast: ");
              ip2html(client, params.config.multicastip(), "MP");
              
              client.print("port: ");
              client.print("<input type=number min=0 max=65535 name=MPO");
              client.print(" value=");
              client.print(String(params.config.multicastport));
              client.print(" />");
              client.println("<br /><br /><br />");

              client.println("<input type=submit name=Sub1 value=Submit /> </form><br />");
              client.println("</body>");
              readString.clear();
              break;
            }
            readString.clear();
					}
				}
			}
		client.stop();
		}
	}

	void init()
	{
		server.begin();
	}	

private:
	EthernetServer server;
  CustomString readString;

  HTTPIface() : server(80) {}
	HTTPIface( const HTTPIface & );

	void ip2html(const EthernetClient &c, const IPAddress& localIP, const char prefix[3])
	{
		for(int i = 0; i < 4; ++i)
	  {
			c.print("<input type=number min=0 max=255 name=");
			c.print(prefix);
			c.print(i);
			c.print(" value=");
			c.print(localIP[i]);
			c.print(" /> . ");
	  }   
	}

  void pwm2html(const EthernetClient &c)
  {
    const Parameters& params = Parameters::get();
    for(int i = 0; i < NUM_PWM_PINS; ++i)
    {
      c.print("CTL");
      c.print(i);

      c.print(": <input type=number min=0 max=255 name=IO");
      c.print(i);
      c.print(" value=");
      c.print(params.pwmVal[i]);
      c.print(" />");

      c.print(" Phase: <input type=number min=1 max=3 name=RPH");
      c.print(i);
      c.print(" value=");
      c.print(params.config.referencePhaseIndex[i] + 1);
      c.print(" />");

      c.print(" Power target: <input type=number min=-3200 max=3200 name=TGR");
      c.print(i);
      c.print(" value=");
      c.print(params.config.targetPower[i]);
      c.print(" />");

      c.print(" Step (LD): <input type=number min=0 max=255 name=USL");
      c.print(i);
      c.print(" value=");
      c.print(params.config.updateStepLo100[i]);
      c.print(" />");

      c.print(" Step (HD): <input type=number min=0 max=255 name=USH");
      c.print(i);
      c.print(" value=");
      c.print(params.config.updateStepGt100[i]);
      c.print(" /><br />");
    }        
  }
	
	IPAddress html2ip(const char* prefix)
	{
		uint8_t octs[4];
    char pt[4];
    pt[0] = prefix[0];
    pt[1] = prefix[1];
    pt[3] = '=';
		for(int i = 0; i < 4; ++i)
	  {
      pt[2] = i + '0';
      octs[i] = readString.parseInt(pt, 4);
	  }
		return IPAddress(octs[0], octs[1], octs[2], octs[3]);
	}
	
};

#endif