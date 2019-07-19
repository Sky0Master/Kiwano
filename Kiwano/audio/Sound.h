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
#include <xaudio2.h>

namespace kiwano
{
	KGE_DECLARE_SMART_PTR(Sound);

	// ���ֶ���
	class KGE_API Sound
		: public Object
	{
	public:
		Sound();

		Sound(
			Resource const& res		/* ������Դ */
		);

		virtual ~Sound();

		// ��������Դ
		bool Load(
			Resource const& res		/* ������Դ */
		);

		// ����
		void Play(
			int loop_count = 0		/* ����ѭ������ (-1 Ϊѭ������) */
		);

		// ��ͣ
		void Pause();

		// ����
		void Resume();

		// ֹͣ
		void Stop();

		// �رղ�������Դ
		void Close();

		// �Ƿ����ڲ���
		bool IsPlaying() const;

		// ��ȡ����
		float GetVolume() const;

		// ��������
		void SetVolume(
			float volume	/* 1 Ϊԭʼ����, ���� 1 Ϊ�Ŵ�����, 0 Ϊ��С���� */
		);

	protected:
		bool	opened_;
		bool	playing_;
		UINT32	size_;
		BYTE*	wave_data_;
		IXAudio2SourceVoice* voice_;
	};
}