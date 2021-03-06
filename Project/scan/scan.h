#pragma once 

#include "ev3dev.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace ev3dev;

class control
{
public:
  control();
  ~control();

  void calibrate();
  void scan();
  void RunMotorX(int position);
  bool RunMotorY();
  void colorscan();
  void test();
  void stop();
  void reset();
  bool initialized() const;

  void terminate() { _terminate = true; }

private:
  int maxX;
  int maxY;
  int speedX = 200;
  int speedY = -40;

protected:
  medium_motor    _motorX;
  large_motor     _motorY;
  color_sensor       _sensor;
  //large_motor     _motorZ;

  enum state
  {
    state_idle,
    state_driving,
    state_turning
  };

  state _state;
  bool  _terminate;
};