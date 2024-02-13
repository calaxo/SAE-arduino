// LED.h
#ifndef EasyESPWebServer_h
#define EasyESPWebServer_h

#include <Arduino.h>

class EasyESPWebServer {
  private:
    int ledPin;
    unsigned char ledState;

  public:
    LED(int pin);
    void turnON();
    void turnOFF();
    int getState();
};

#endif
