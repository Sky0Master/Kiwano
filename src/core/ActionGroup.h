// Copyright (c) 2016-2018 Easy2D - Nomango
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

#pragma once
#include "Action.h"

namespace easy2d
{
	// 动作组
	class E2D_API ActionGroup
		: public Action
	{
	public:
		using ActionList = IntrusiveList<ActionPtr>;

		ActionGroup();

		explicit ActionGroup(
			Array<ActionPtr> const& actions,
			bool sequence = true				// 按顺序执行或同时执行
		);

		virtual ~ActionGroup();

		// 添加动作
		void Add(
			ActionPtr const& action
		);

		// 添加多个动作
		void Add(
			Array<ActionPtr> const& actions
		);

		// 获取所有动作
		inline ActionList const& GetActions() { return actions_; }

		// 获取该动作的拷贝对象
		ActionPtr Clone() const override;

		// 获取该动作的倒转
		ActionPtr Reverse() const override;

	protected:
		// 初始化动作
		void Init(NodePtr const& target) override;

		// 更新动作
		void Update(NodePtr const& target, Duration dt) override;

	protected:
		bool		sequence_;
		ActionPtr	current_;
		ActionList	actions_;
	};


	// 顺序动作
	class E2D_DEPRECATED("ActionSequence is deprecated, use ActionGroup instead") E2D_API
		ActionSequence
		: public ActionGroup
	{
	public:
		ActionSequence();

		explicit ActionSequence(
			Array<ActionPtr> const& actions
		);

		virtual ~ActionSequence();
	};


	// 同步动作
	class E2D_DEPRECATED("ActionSpawn is deprecated, use ActionGroup instead") E2D_API
		ActionSpawn
		: public ActionGroup
	{
	public:
		ActionSpawn();

		explicit ActionSpawn(
			Array<ActionPtr> const& actions
		);

		virtual ~ActionSpawn();
	};
}
