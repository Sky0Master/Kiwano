#include "..\e2daction.h"
#include "..\e2dmanager.h"

e2d::Loop::Loop(Action * action, int times /* = -1 */)
	: _action(action)
	, _times(0)
	, _totalTimes(times)
{
	WARN_IF(action == nullptr, "Loop NULL pointer exception!");

	if (action)
	{
		_action = action;
		GC::retain(_action);
	}
}

e2d::Loop::~Loop()
{
	GC::safeRelease(_action);
}

e2d::Loop * e2d::Loop::clone() const
{
	if (_action)
	{
		return new (e2d::autorelease) Loop(_action->clone());
	}
	else
	{
		return nullptr;
	}
}

e2d::Loop * e2d::Loop::reverse() const
{
	if (_action)
	{
		return new (e2d::autorelease) Loop(_action->clone());
	}
	else
	{
		return nullptr;
	}
}

void e2d::Loop::_init()
{
	Action::_init();

	if (_action)
	{
		_action->_target = _target;
		_action->_init();
	}
}

void e2d::Loop::_update()
{
	Action::_update();

	if (_times == _totalTimes)
	{
		this->stop();
		return;
	}

	if (_action)
	{
		_action->_update();

		if (_action->_isDone())
		{
			++_times;

			Action::reset();
			_action->reset();
		}
	}
	else
	{
		this->stop();
	}
}

void e2d::Loop::reset()
{
	Action::reset();

	if (_action) _action->reset();
	_times = 0;
}

void e2d::Loop::_resetTime()
{
	if (_action) _action->_resetTime();
}
