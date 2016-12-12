#include "scan.h"

int maxX;
int maxY;
int speedX = 300;
int speedY = -40;


control::control() :
  _motorX(OUTPUT_A),
  _motorY(OUTPUT_B),
  _sensor(INPUT_1),
  _state(state_idle),
  _terminate(false)
{
}

control::~control()
{
  reset();
}

void control::calibrate()
{
  // nulpunten zoeken
  _motorX.set_speed_sp(speedX);
  _motorY.set_speed_sp(speedY);
  //_motorY.set_speed_sp(-speed);
  _state = state_driving;
  _motorX.run_forever();
  _motorY.run_forever();
  this_thread::sleep_for(chrono::milliseconds(300));
  while (_state == state_driving)
  {
    this_thread::sleep_for(chrono::milliseconds(10));
    if (_motorX.state().count("stalled"))
    {
      _motorX.stop_action();      
      _motorX.stop();      
      _motorX.reset();      
    }
    if (_motorY.state().count("stalled"))
    {
      _motorY.stop_action();
      _motorY.stop();
      _motorY.reset();
    }
    if (_motorX.speed() == 0 && _motorY.speed() == 0)
    {
      _state = state_idle;
    }
  }
  //maxpunten zoeken
  speedX = -speedX;
  speedY = -speedY;
  _motorX.set_speed_sp(speedX);
  _motorY.set_speed_sp(speedY);
  _state = state_driving;
  _motorX.run_forever();
  _motorY.run_forever();
  this_thread::sleep_for(chrono::milliseconds(300));
  while (_state == state_driving)
  {
    this_thread::sleep_for(chrono::milliseconds(10));
    if (_motorX.state().count("stalled"))
    {
      _motorX.stop_action();      
      _motorX.stop();
      maxX = _motorX.position_sp();
    }
    if (_motorY.state().count("stalled"))
    {
      _motorY.stop_action();
      _motorX.stop();
      maxY = _motorY.position_sp();
    }
    if (_motorX.speed() == 0 && _motorY.speed() == 0)
    {
      _motorX.set_position_sp(0);
      _motorY.set_position_sp(0);
      _motorX.run_to_abs_pos();
      _motorY.run_to_abs_pos();
      this_thread::sleep_for(chrono::milliseconds(300));
        while (_motorX.state().count("running") || _motorY.state().count("running"))
        {
          this_thread::sleep_for(chrono::milliseconds(10));
        }
      _state = state_idle;
    }
  }
}

void control::scan()
{
  _motorX.set_speed_sp(speedX);
  _motorY.set_speed_sp(speedY);
  while (1)
  {    
    _state = state_driving;
    _motorX.run_forever();
    this_thread::sleep_for(chrono::milliseconds(300));
    while (_state == state_driving)
    {
      this_thread::sleep_for(chrono::milliseconds(10));
      colorscan();
      if (_motorX.speed() == 0)
      {
        _motorX.stop_action();      
        _motorX.stop();
        _motorY.set_position_sp(180);
        _motorY.run_to_rel_pos();
        this_thread::sleep_for(chrono::milliseconds(300));
        while (_motorY.state().count("running"))
        {
          this_thread::sleep_for(chrono::milliseconds(10));
          if (_motorY.state().count("stalled"))
          {
            _motorX.stop_action();      
            _motorX.stop();
            _motorY.stop_action();
            _motorY.stop();
            calibrate();
            return;
          }          
        }
        this_thread::sleep_for(chrono::milliseconds(300));        
        if (_motorY.speed() == 0)
        {
          speedX = -speedX;
          _motorX.set_speed_sp(speedX);
          _state = state_idle;
        }
      }
    }
  }
}

void control::colorscan()
{
  int value = _sensor.reflected_light_intensity();
  if(value > 23)
  {
    //sound::speak("wit", true);
    printf("White       %d\n", value);
  }
  else if(value < 24)
  {
    //sound::speak("zwart", true);
    printf("Black       %d\n", value);
  }
  
}

void control::test()
{
  int speedY = 300;
  _motorX.set_speed_sp(speedY);
  _motorX.set_position_sp(-400);
  _motorX.run_to_abs_pos();
  this_thread::sleep_for(chrono::milliseconds(300));
        while (_motorX.state().count("running"))
        {
          this_thread::sleep_for(chrono::milliseconds(10));
        }
        _motorX.set_position_sp(0);
        _motorX.run_to_abs_pos();
        this_thread::sleep_for(chrono::milliseconds(300));
        while (_motorX.state().count("running"))
        {
          this_thread::sleep_for(chrono::milliseconds(10));
        }
}

void control::stop()
{
  _motorX.stop_action();      
  _motorX.stop();
  _motorY.stop_action();
  _motorY.stop();
  _state = state_idle;
}

void control::reset()
{
  if (_motorX.connected())
    _motorX.reset();

  if (_motorY.connected())
    _motorY.reset();

  _state = state_idle;
}

bool control::initialized() const
{
  return (_motorX .connected() &&
          _motorY.connected());
}

int main()
{
  control c;
  c.calibrate();
  //c.test();
  //c.scan();
  
}

// Color detected by the sensor, categorized by overall value.
  //   - 0: No color
  //   - 1: Black
  //   - 2: Blue
  //   - 3: Green
  //   - 4: Yellow
  //   - 5: Red
  //   - 6: White
  //   - 7: Brown