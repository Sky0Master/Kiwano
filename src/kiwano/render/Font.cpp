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

#include <kiwano/render/Font.h>
#include <kiwano/render/Renderer.h>

namespace kiwano
{

FontPtr Font::Create(String const& file)
{
    FontPtr ptr = new (std::nothrow) Font;
    if (ptr)
    {
        if (!ptr->Load(file))
            return nullptr;
    }
    return ptr;
}

FontPtr Font::Create(Resource const& resource)
{
    FontPtr ptr = new (std::nothrow) Font;
    if (ptr)
    {
        if (!ptr->Load(resource))
            return nullptr;
    }
    return ptr;
}

Font::Font() {}

bool Font::Load(String const& file)
{
    try
    {
        Renderer::Instance().CreateFontCollection(*this, file);
    }
    catch (std::runtime_error&)
    {
        return false;
    }
    return true;
}

bool Font::Load(Resource const& resource)
{
    try
    {
        Renderer::Instance().CreateFontCollection(*this, resource);
    }
    catch (std::runtime_error&)
    {
        return false;
    }
    return true;
}

}  // namespace kiwano
