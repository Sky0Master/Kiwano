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
#include <kiwano/core/Common.h>
#include <kiwano/macros.h>

namespace kiwano
{
/**
 * \~chinese
 * @brief 引用计数器
 */
class KGE_API RefCounter : protected Noncopyable
{
public:
    /// \~chinese
    /// @brief 增加引用计数
    void Retain();

    /// \~chinese
    /// @brief 减少引用计数
    void Release();

    /// \~chinese
    /// @brief 获取引用计数
    long GetRefCount() const;

protected:
    RefCounter();

    virtual ~RefCounter();

private:
    long ref_count_;
};

inline long RefCounter::GetRefCount() const
{
    return ref_count_;
}
}  // namespace kiwano
