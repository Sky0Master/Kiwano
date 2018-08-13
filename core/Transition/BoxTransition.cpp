#include "..\e2dtransition.h"
#include "..\e2dnode.h"

e2d::BoxTransition::BoxTransition(Scene* scene, float duration)
	: Transition(scene, duration)
{
}

bool e2d::BoxTransition::_init(Scene * prev)
{
	if (Transition::_init(prev))
	{
		_inLayerParam.opacity = 0;
		return true;
	}
	return false;
}

void e2d::BoxTransition::_update()
{
	Transition::_update();

	if (_delta <= 0.5)
	{
		_outLayerParam.contentBounds = D2D1::RectF(
			_windowSize.width * _delta,
			_windowSize.height * _delta,
			_windowSize.width * (1 - _delta),
			_windowSize.height * (1 - _delta)
		);
	}
	else
	{
		_outLayerParam.opacity = 0;
		_inLayerParam.opacity = 1;
		_inLayerParam.contentBounds = D2D1::RectF(
			_windowSize.width * (1 - _delta),
			_windowSize.height * (1 - _delta),
			_windowSize.width * _delta,
			_windowSize.height * _delta
		);
		if (_delta >= 1)
		{
			this->_stop();
		}
	}
}
