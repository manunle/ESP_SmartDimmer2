 #include <Arduino.h>

extern void sendStatus();
extern strDateTime DateTime;

volatile int zc = 0;
volatile int zcount = 0;
volatile long tmcount = 0;
volatile long cycletime = 0;

class dimmer
{
    byte state=0;
    byte zcPin;
    byte outPin;
    byte fade;
    byte curBrightness;
    byte tarBrightness; 
    byte zcState;
    double dbrightness;
    double dfaderate;   
    String name;
    long offTime;
    long onTime;    
public:
    dimmer(byte zcpin, byte outpin, String Name);
    void setState(byte State,double faderate,byte brightness); 
    void toggle();
    byte getState() {return state;};
    String getName() {return name;};
    long getoffTime() {return offTime;};
    long getonTime() {return onTime;};
    void setoffTime(long off) {offTime = off;};
    void setonTime(long on) {onTime = on;};
    void tick();
    void zeroCrossISR();
    void dimTimerISR();
};

dimmer::dimmer(byte zcpin,byte outpin,String Name)
{
  zcPin = zcpin;
  outPin = outpin;
  fade = 0;
  curBrightness = 0;
  tarBrightness = 0;
  dbrightness = 0.0;
  dfaderate = 1.0;
}

void dimmer::setState(byte State,double faderate,byte brightness)
{
  state = State;
  dfaderate = faderate;
  tarBrightness = brightness;
}

void dimmer::toggle()
{
  if(state == 0)
  {
    state = 1;
    tarBrightness = 255;
  }
  else
  {
    tarBrightness = 0;    
  }
  
}

void dimmer::tick()
{
  if(offTime > 0)
  {
    offTime--;
    if(offTime==0)
      setState(0,1.0,0);
  }
  if(onTime > 0)
  {
    onTime--;
    if(onTime==0)
      setState(1,1.0,255);
  }
}

void dimmer::zeroCrossISR()
{
  zc++;
  zcount++;
  if(zcState == 0)
  {
    zcState = 1;
    if(curBrightness < 255 && curBrightness > 0)
    {
      digitalWrite(outPin, 0);
    }
    int dimDelay = 30 * (255 - curBrightness) + 400;
    hw_timer_arm(dimDelay);
  }
}

void dimmer::dimTimerISR()
{
  tmcount++;
  if(fade == 1)
  {
    if(curBrightness > tarBrightness || (state == 0 && curBrightness > 0))
    {
      dbrightness = dbrightness-dfaderate;
      curBrightness = (int) dbrightness;
    }
    else if(curBrightness < tarBrightness && state == 1 && curBrightness < 255)
    {
      dbrightness = dbrightness + dfaderate;
      curBrightness = (int) dbrightness;
    }
  }
  else
  {
    if(state == 1)
    {
      curBrightness = tarBrightness;
      dbrightness = curBrightness;
    }
    else
    {
      curBrightness = 0;
      dbrightness = 0.0;
    }
  }
  if (curBrightness == 0) 
  {
    state = 0;
    digitalWrite(outPin, 0);
  }
  else if (curBrightness == 255) 
  {
    state = 1;
    digitalWrite(outPin, 1);
  }
  else 
  {
    digitalWrite(outPin, 1);
  }
  zcState = 0;
}