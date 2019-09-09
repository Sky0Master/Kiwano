// Copyright (c) 2016-2018 Kiwano - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ActionWalk.h"
#include "../Actor.h"

namespace kiwano
{
	ActionWalk::ActionWalk(Duration duration, bool rotating, Float32 start, Float32 end, EaseFunc func)
		: ActionTween(duration, func)
		, start_(start)
		, end_(end)
		, rotating_(rotating)
		, length_(0.f)
	{
	}

	ActionWalk::ActionWalk(Duration duration, Geometry const& path, bool rotating, Float32 start, Float32 end, EaseFunc func)
		: ActionWalk(duration, rotating, start, end, func)
	{
		path_ = path;
	}

	ActionPtr ActionWalk::Clone() const
	{
		ActionWalkPtr clone = new ActionWalk(dur_, rotating_, start_, end_, ease_func_);
		if (clone)
		{
			clone->SetPath(path_);
		}
		return clone;
	}

	ActionPtr ActionWalk::Reverse() const
	{
		ActionWalkPtr reverse = new ActionWalk(dur_, rotating_, end_, start_, ease_func_);
		if (reverse)
		{
			reverse->SetPath(path_);
		}
		return reverse;
	}

	void ActionWalk::Init(ActorPtr target)
	{
		if (!path_)
		{
			Done();
			return;
		}

		start_pos_ = target->GetPosition();
		length_ = path_.GetLength();
	}

	void ActionWalk::UpdateTween(ActorPtr target, Float32 percent)
	{
		Float32 distance = length_ * std::min(std::max((end_ - start_) * percent + start_, 0.f), 1.f);

		Point point, tangent;
		if (path_.ComputePointAtLength(distance, point, tangent))
		{
			target->SetPosition(start_pos_ + point);

			if (rotating_)
			{
				Float32 ac = math::Acos(tangent.x);
				Float32 rotation = (tangent.y < 0.f) ? 360.f - ac : ac;
				target->SetRotation(rotation);
			}
		}
	}

	void ActionWalk::BeginPath()
	{
		sink_.BeginPath();
	}

	void ActionWalk::EndPath(bool closed)
	{
		sink_.EndPath(closed);
		path_ = sink_.GetGeometry();
	}

	void ActionWalk::AddLine(Point const& point)
	{
		sink_.AddLine(point);
	}

	void ActionWalk::AddLines(Vector<Point> const& points)
	{
		sink_.AddLines(points);
	}

	void ActionWalk::AddBezier(Point const& point1, Point const& point2, Point const& point3)
	{
		sink_.AddBezier(point1, point2, point3);
	}

	void ActionWalk::AddArc(Point const& point, Size const& radius, Float32 rotation, bool clockwise, bool is_small)
	{
		sink_.AddArc(point, radius, rotation, clockwise, is_small);
	}

	void ActionWalk::ClearPath()
	{
		path_.SetGeometry(nullptr);
	}

}