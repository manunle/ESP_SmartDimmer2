#include <Arduino.h>
#include <PubSubClient.h>
#include "ESPBASE.h"
#include "hw_timer.h"
#include "dimmer.h"


WiFiClient espClient;
PubSubClient mqttClient(espClient);
const byte mqttDebug = 1;
dimmer* Dimmer;
String sChipID;
long lastReconnectAttempt = 0;
unsigned long lastschedulecheck = 0;
int rbver = 0;
String DimmerTopic;
String StatusTopic;
unsigned long reporttimemilli = 600000;
unsigned long lastreporttime = 0;
String ScheduleTopic;
bool scheduleon;
void zcISR();
void dimtimerISR();

ESPBASE Esp;
void sendStatus();

void setup() {
  Serial.begin(115200);
  char cChipID[10];
  sprintf(cChipID,"%08X",ESP.getChipId());
  sChipID = String(cChipID);

  Esp.initialize();
  DimmerTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/command";
  StatusTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/status";
  ScheduleTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/schedule";
  Dimmer = new dimmer(12,13,config.DimmerName);
  Esp.setupMQTTClient();
  customWatchdog = millis();

  scheduleon = false;

  hw_timer_init(NMI_SOURCE,0);
  hw_timer_set_func(dimtimerISR);
  attachInterrupt(12,zcISR,RISING);
  Serial.println("Done with setup");
  Serial.println(config.ssid);
}

void setSchedule()
{
  if(suntime.valid && UnixTimestamp > 100000)
  {
    long tmnow = (DateTime.hour*60+DateTime.minute) * 60;
    Serial.println("now " + String(DateTime.hour) + ":" + String(DateTime.minute));
    long nexton = 86401;
    int noshed = -1;
  //  Serial.println(String(DateTime.hour) + ":"+String(DateTime.minute));
    for(int i=0;i<10;i++)
    {
      if(i < 2)
      {
        if(config.RSchedule[i].onatsunset)
        {
          config.RSchedule[i].onHour = suntime.setHour;
          config.RSchedule[i].onMin = suntime.setMin;
        }  
        if(config.RSchedule[i].offatsunrise)
        {
          config.RSchedule[i].offHour = suntime.riseHour;
          config.RSchedule[i].offMin = suntime.riseMin;
        }
      }  
      if(config.RSchedule[i].wdays[DateTime.wday-1])
      {
        if(Dimmer->getonTime() == 0 && Dimmer->getoffTime() == 0)
        {
          long tmschedon = (config.RSchedule[i].onHour*60+config.RSchedule[i].onMin) * 60;
          if(tmschedon > tmnow)
          {
            tmschedon = tmschedon - tmnow;
            if(tmschedon < nexton)
            {
              Serial.println("relay schedule " + String(i) + " " + String(config.RSchedule[i].onHour) + ":" + String(config.RSchedule[i].onMin) + " " + String(tmschedon));
              nexton = tmschedon;
              noshed = i;
            }
          }
        } 
      }
    }
    if(nexton < 86401)
    {
      Dimmer->setonTime(nexton);
      long offseconds = (config.RSchedule[noshed].offHour*60+config.RSchedule[noshed].offMin) * 60;
      Serial.println(String(config.RSchedule[noshed].offHour) + ":" + String(config.RSchedule[noshed].offMin) + " " + String(offseconds));
      offseconds = offseconds - tmnow;
      Serial.println(String(offseconds));
      if(offseconds < nexton)
        offseconds = offseconds + 86400;  // add 24 hours
      Serial.println(offseconds);
      Dimmer->setoffTime(offseconds);
      Serial.println("Set relay schedule to on " + String(Dimmer->getonTime()) + " off " + String(Dimmer->getoffTime()));
    }
  }
//  return false;
}

void loop() 
{
  Esp.loop();
  if(millis() > lastreporttime + reporttimemilli)
  {
    if(config.ReportTime)
    {
      Esp.mqttSend(String(DEVICE_TYPE) + "/" + config.DeviceName + "/time",String(DateTime.hour) + ":" + String(DateTime.minute) + ":" + String(DateTime.second),"");
      String sched = "Relay  on " + String(Dimmer->getonTime()) + " off " + String(Dimmer->getoffTime());
      Serial.println(sched);
      Esp.mqttSend(String(DEVICE_TYPE) + "/" + config.DeviceName + "/schedule",sched," now " + String((DateTime.hour*60+DateTime.minute)*60));
    }
    lastreporttime = millis();
  }
  if(millis() > lastschedulecheck + 60000)
  {
    lastschedulecheck = millis();
    setSchedule();
  }
}

String getSignalString()
{
  String signalstring = "";
  byte available_networks = WiFi.scanNetworks();
  signalstring = signalstring + sChipID + ":";
 
  for (int network = 0; network < available_networks; network++) {
    String sSSID = WiFi.SSID(network);
    if(network > 0)
      signalstring = signalstring + ",";
    signalstring = signalstring + WiFi.SSID(network) + "=" + String(WiFi.RSSI(network));
  }
  return signalstring;    
}

void sendStatus()
{
  String message = "";
  message = message + Dimmer->getName() + ",";
  if(Dimmer->getState() == 1)
    message = message + "on";
  else
    message = message + "off";
  Esp.mqttSend(StatusTopic,"",message);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);
  Serial.println("Got topic " + s_topic + " payload " + s_payload);

  if(s_topic == "SendStat")
  {
    sendStatus();
  }
  if (s_topic == DimmerTopic || s_topic == "AllLights" || s_topic == "computer/timer/event") 
  {
    if(s_payload == "signal")
    {
      Esp.mqttSend(StatusTopic,sChipID," WiFi: " + getSignalString());
    }
    if(s_payload == "TOGGLE")
    {
      Dimmer->toggle();
    }
    if(s_payload == "ON")
    {
      Dimmer->setState(1,1.0,255);
    }
    if(s_payload == "OFF")
    {
      Dimmer->setState(0,1.0,0);
    }
  }
}

void mqttSubscribe()
{
    if (Esp.mqttClient->connected()) {
      if (Esp.mqttClient->subscribe(DimmerTopic.c_str())) {
        Serial.println("Subscribed to " + DimmerTopic);
        Esp.mqttSend(StatusTopic,"","Subscribed to " + DimmerTopic);
        Esp.mqttSend(StatusTopic,verstr,","+Esp.MyIP()+" start");
      }
      if (Esp.mqttClient->subscribe("SendStat"))
      {
        Serial.println("Subscribed to SendStat");
      }
      if (Esp.mqttClient->subscribe("AllLights"))
      {
        Serial.println("Subscribed to AllLights");
      }
      if (Esp.mqttClient->subscribe("computer/timer/event"))
      {
        Serial.println("Subscribed to computer/timer/event");
      }
      char buff[100];
      ScheduleTopic.toCharArray(buff,100);
      if (Esp.mqttClient->subscribe((const char *)buff))
      {
        Serial.println("Subscribed to " + ScheduleTopic);
      }
    }
}

void mainTick()
{
  Dimmer->tick();
}

void zcISR()
{
  Dimmer->zeroCrossISR();
}

void dimtimerISR()
{
  Dimmer->dimTimerISR();
}
