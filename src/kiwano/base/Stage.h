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

#pragma once
#include "../macros.h"
#include "../common/Singleton.hpp"
#include "../2d/include-forwards.h"
#include "Component.h"

namespace kiwano
{
	class KGE_API Stage
		: public Singleton<Stage>
		, public Component
	{
		KGE_DECLARE_SINGLETON(Stage);

	public:
		// 切换场景
		void EnterScene(
			ScenePtr scene				/* 场景 */
		);

		// 切换场景
		void EnterScene(
			ScenePtr scene,				/* 场景 */
			TransitionPtr transition	/* 场景动画 */
		);

		// 获取当前场景
		ScenePtr GetCurrentScene();

		// 启用或禁用场景内的节点边界渲染功能
		void SetRenderBorderEnabled(bool enabled);

		// 显示调试信息
		void ShowDebugInfo(bool show = true);

	public:
		void SetupComponent() override {}

		void DestroyComponent() override {}

		void OnUpdate(Duration dt) override;

		void OnRender() override;

		void AfterRender() override;

		void HandleEvent(Event& evt) override;

	protected:
		Stage();

		virtual ~Stage();

	protected:
		bool			render_border_enabled_;
		ScenePtr		curr_scene_;
		ScenePtr		next_scene_;
		NodePtr			debug_node_;
		TransitionPtr	transition_;
	};
}
