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

#include <kiwano/2d/FrameSequence.h>
#include <kiwano/core/Logger.h>

namespace kiwano
{

FrameSequencePtr FrameSequence::Create(Vector<FramePtr> const& frames)
{
    FrameSequencePtr ptr = new (std::nothrow) FrameSequence;
    if (ptr)
    {
        ptr->AddFrames(frames);
    }
    return ptr;
}

FrameSequence::FrameSequence() {}

FrameSequence::~FrameSequence() {}

void FrameSequence::AddFrame(FramePtr frame)
{
    KGE_ASSERT(frame && "FrameSequence::Add failed, NULL pointer exception");

    if (frame)
    {
        frames_.push_back(frame);
    }
}

void FrameSequence::AddFrames(Vector<FramePtr> const& frames)
{
    if (frames_.empty())
        frames_ = frames;
    else
    {
        frames_.reserve(frames_.size() + frames.size());
        for (const auto& texture : frames)
            AddFrame(texture);
    }
}

FramePtr FrameSequence::GetFrame(size_t index) const
{
    KGE_ASSERT(index < frames_.size());
    return frames_[index];
}

Vector<FramePtr> const& FrameSequence::GetFrames() const
{
    return frames_;
}

size_t FrameSequence::GetFramesCount() const
{
    return frames_.size();
}

FrameSequencePtr FrameSequence::Clone() const
{
    auto frame_seq = new (std::nothrow) FrameSequence;
    if (frame_seq)
    {
        frame_seq->AddFrames(frames_);
    }
    return frame_seq;
}

FrameSequencePtr FrameSequence::Reverse() const
{
    auto frame_seq = new (std::nothrow) FrameSequence;
    if (!frames_.empty())
    {
        for (auto iter = frames_.crbegin(), crend = frames_.crend(); iter != crend; ++iter)
        {
            if (*iter)
                frame_seq->AddFrame(*iter);
        }
    }
    return frame_seq;
}

}  // namespace kiwano
