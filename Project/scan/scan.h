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
  void driveX(int speed, int time=0);
  void driveY(int speed, int time=0);
  void turn(int direction);
  void stop();
  void reset();

  bool initialized() const;

  void terminate() { _terminate = true; }

protected:
  medium_motor    _motorX;
  large_motor     _motorY;
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