#include "scan.h"

control::control() :
  _motorX(OUTPUT_A),
  _motorY(OUTPUT_B),
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
	int speed = 500;
	_motorX.set_speed_sp(speed);
	_motorY.set_speed_sp(-25);
	//_motorY.set_speed_sp(-speed);
	_state = state_driving;
	_motorX.run_forever();
	_motorY.run_forever();
	this_thread::sleep_for(chrono::milliseconds(300));
	while (_state == state_driving)
	{
		this_thread::sleep_for(chrono::milliseconds(10));
		if (_motorX.speed() == 0 && _motorY.speed() == 0)
		{
			_motorX.stop_action();
			_motorY.stop_action();
			_motorX.stop();
			_motorY.stop();
			_motorX.reset();
			_motorY.reset();
			_state = state_idle;
		}
	}
	/*while (_state == state_driving)
	{
		if (_motorX.speed() == 0)
		{
			_motorX.stop_action();
			_state = state_idle;
		}
	}*/
	
}

void control::driveX(int speed, int time)
{
  _motorX.set_speed_sp(-speed);

  _state = state_driving;

  if (time > 0)
  {
    _motorX .set_time_sp(time).run_timed();

    while (_motorX.state().count("running"))
      this_thread::sleep_for(chrono::milliseconds(10));

    _state = state_idle;
  }
  else
  {
    _motorX.run_forever();
  }
}

void control::driveY(int speed, int time)
{
  _motorY.set_speed_sp(-speed);

  _state = state_driving;

  if (time > 0)
  {
    _motorY.set_time_sp(time).run_timed();

    while (_motorY.state().count("running"))
      this_thread::sleep_for(chrono::milliseconds(10));

    _state = state_idle;
  }
  else
  {
    _motorY.run_forever();
  }
}

void control::turn(int direction)
{
  if (_state != state_idle)
    stop();

  if (direction == 0)
    return;

  _state = state_turning;

  _motorX. set_position_sp( direction).set_speed_sp(500).run_to_rel_pos();
  _motorX.set_position_sp(-direction).set_speed_sp(500).run_to_rel_pos();

  while (_motorX.state().count("running") || _motorY.state().count("running"))
    this_thread::sleep_for(chrono::milliseconds(10));

  _state = state_idle;
}

void control::stop()
{
  _motorX .stop();
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
  //c.driveX(100, 0);
  c.calibrate();
}