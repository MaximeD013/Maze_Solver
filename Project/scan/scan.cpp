#include "scan.h"

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
      printf("%d\n", _motorX.position());      
    }
    if (_motorY.state().count("stalled"))
    {
      _motorY.stop_action();
      _motorY.stop();
      _motorY.reset();
      printf("%d\n", _motorY.position());  
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
      maxX = _motorX.position() + 20;
      printf("%d\n", maxX);
    }
    if (_motorY.state().count("stalled"))
    {
      _motorY.stop_action();
      _motorX.stop();
      maxY = _motorY.position() - 5;
      printf("%d\n", maxY);
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
    RunMotorX(maxX);
    if(RunMotorY() == false)
    {
      return;
    }      
    RunMotorX(0);
    if(RunMotorY() == false)
    {
      return;
    } 
  }
}

void control::RunMotorX(int position)
{
  _motorX.set_position_sp(position);
  _motorX.run_to_abs_pos();
  this_thread::sleep_for(chrono::milliseconds(300));
  while (_motorX.state().count("running"))
  {
    this_thread::sleep_for(chrono::milliseconds(10));
    colorscan();
  }
}

bool control::RunMotorY()
{
  _motorY.set_position_sp(1000);
  _motorY.run_to_rel_pos();
  this_thread::sleep_for(chrono::milliseconds(300));
  while (_motorY.state().count("running"))
  {
    this_thread::sleep_for(chrono::milliseconds(10));
    if (_motorY.position() >= maxY)
    {     
      _motorY.stop_action();
      _motorY.stop();
      return false;
    }
  }
  return true;
}

void control::colorscan()
{
  int value = _sensor.reflected_light_intensity();
  if(value > 19)
  {
    printf("White - %d    X: %d     Y:    %d\n", value, _motorX.position(), _motorY.position());
  }
  else if(value < 20)
  {
    printf("Black - %d    X: %d     Y:    %d\n", value, _motorX.position(), _motorY.position());
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

  c.scan();
  // sound::tone({
  //           {392, 350, 100}, {392, 350, 100}, {392, 350, 100}, {311.1, 250, 100},
  //           {466.2, 25, 100}, {392, 350, 100}, {311.1, 250, 100}, {466.2, 25, 100},
  //           {392, 700, 100}, {587.32, 350, 100}, {587.32, 350, 100},
  //           {587.32, 350, 100}, {622.26, 250, 100}, {466.2, 25, 100},
  //           {369.99, 350, 100}, {311.1, 250, 100}, {466.2, 25, 100}, {392, 700, 100},
  //           {784, 350, 100}, {392, 250, 100}, {392, 25, 100}, {784, 350, 100},
  //           {739.98, 250, 100}, {698.46, 25, 100}, {659.26, 25, 100},
  //           {622.26, 25, 100}, {659.26, 50, 400}, {415.3, 25, 200}, {554.36, 350, 100},
  //           {523.25, 250, 100}, {493.88, 25, 100}, {466.16, 25, 100}, {440, 25, 100},
  //           {466.16, 50, 400}, {311.13, 25, 200}, {369.99, 350, 100},
  //           {311.13, 250, 100}, {392, 25, 100}, {466.16, 350, 100}, {392, 250, 100},
  //           {466.16, 25, 100}, {587.32, 700, 100}, {784, 350, 100}, {392, 250, 100},
  //           {392, 25, 100}, {784, 350, 100}, {739.98, 250, 100}, {698.46, 25, 100},
  //           {659.26, 25, 100}, {622.26, 25, 100}, {659.26, 50, 400}, {415.3, 25, 200},
  //           {554.36, 350, 100}, {523.25, 250, 100}, {493.88, 25, 100},
  //           {466.16, 25, 100}, {440, 25, 100}, {466.16, 50, 400}, {311.13, 25, 200},
  //           {392, 350, 100}, {311.13, 250, 100}, {466.16, 25, 100},
  //           {392.00, 300, 150}, {311.13, 250, 100}, {466.16, 25, 100}, {392, 700}
  //           },
  //           true
  //           );
  
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