// Copyright (c) 2018-2019 Kiwano - Nomango
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
#include <kiwano-physics/helper.h>

namespace kiwano
{
namespace physics
{
class Body;

KGE_DECLARE_SMART_PTR(Fixture);

/**
 * \addtogroup Physics
 * @{
 */

/// \~chinese
/// @brief 物理夹具
class Fixture : public virtual ObjectBase
{
public:
    /// \~chinese
    /// @brief 夹具参数
    struct Param
    {
        float density     = 0.0f;   ///< 密度
        float friction    = 0.2f;   ///< 摩擦力
        float restitution = 0.0f;   ///< 弹性恢复
        bool  is_sensor   = false;  ///< 是否是接触传感器

        Param() {}

        Param(float density, float friction = 0.2f, float restitution = 0.f, bool is_sensor = false)
            : density(density)
            , friction(friction)
            , restitution(restitution)
            , is_sensor(is_sensor)
        {
        }
    };

    /// \~chinese
    /// @brief 创建圆形夹具
    /// @param body 添加夹具的物体
    /// @param param 夹具参数
    /// @param radius 圆形半径
    /// @param offset 偏移量
    static FixturePtr CreateCircle(Body* body, Param const& param, float radius, Point const& offset = Point());

    /// \~chinese
    /// @brief 创建矩形夹具
    /// @param body 添加夹具的物体
    /// @param param 夹具参数
    /// @param size 矩形大小
    /// @param offset 偏移量
    /// @param rotation 旋转角度
    static FixturePtr CreateRect(Body* body, Param const& param, Size const& size, Point const& offset = Point(),
                                 float rotation = 0.f);

    /// \~chinese
    /// @brief 创建多边形夹具
    /// @param body 添加夹具的物体
    /// @param param 夹具参数
    /// @param vertexs 多边形顶点
    static FixturePtr CreatePolygon(Body* body, Param const& param, Vector<Point> const& vertexs);

    /// \~chinese
    /// @brief 创建边夹具
    /// @param body 添加夹具的物体
    /// @param param 夹具参数
    /// @param p1 边的起点
    /// @param p2 边的终点
    static FixturePtr CreateEdge(Body* body, Param const& param, Point const& p1, Point const& p2);

    /// \~chinese
    /// @brief 创建链条夹具
    /// @param body 添加夹具的物体
    /// @param param 夹具参数
    /// @param vertexs 链条顶点
    /// @param loop 是否连接链条的起点和终点
    static FixturePtr CreateChain(Body* body, Param const& param, Vector<Point> const& vertexs, bool loop = false);

    Fixture();

    virtual ~Fixture();

    /// \~chinese
    /// @brief 是否有效
    bool IsValid() const;

    /// \~chinese
    /// @brief 获取夹具所在的物体
    Body* GetBody() const;

    /// \~chinese
    /// @brief 是否是接触传感器
    bool IsSensor() const;

    /// \~chinese
    /// @brief 设置夹具是否是接触传感器
    /// @details 接触传感器只会产生物理接触，而不会影响物体运动
    void SetSensor(bool sensor);

    /// \~chinese
    /// @brief 获取夹具的质量数据
    void GetMassData(float* mass, Point* center, float* inertia) const;

    /// \~chinese
    /// @brief 获取密度
    float GetDensity() const;

    /// \~chinese
    /// @brief 设置密度
    void SetDensity(float density);

    /// \~chinese
    /// @brief 获取摩擦力 [N]
    float GetFriction() const;

    /// \~chinese
    /// @brief 设置摩擦力 [N]
    void SetFriction(float friction);

    /// \~chinese
    /// @brief 获取弹性恢复
    float GetRestitution() const;

    /// \~chinese
    /// @brief 设置弹性恢复
    void SetRestitution(float restitution);

    /// \~chinese
    /// @brief 点测试
    bool TestPoint(const Point& p) const;

    b2Fixture* GetB2Fixture() const;

    void SetB2Fixture(b2Fixture* fixture);

    bool operator==(const Fixture& rhs) const;
    bool operator!=(const Fixture& rhs) const;

private:
    b2Fixture* fixture_;
};

/// \~chinese
/// @brief 物理夹具列表
class FixtureList
{
    template <typename _Ty>
    class IteratorImpl : public std::iterator<std::forward_iterator_tag, _Ty>
    {
        using herit = std::iterator<std::forward_iterator_tag, _Ty>;

    public:
        using reference = typename herit::reference;
        using pointer   = typename herit::pointer;

        IteratorImpl(pointer elem)
            : elem_(elem)
        {
        }

        inline reference operator*() const
        {
            return const_cast<typename herit::reference>(*elem_);
        }

        inline pointer operator->() const
        {
            return std::pointer_traits<typename herit::pointer>::pointer_to(**this);
        }

        inline IteratorImpl& operator++()
        {
            b2Fixture* next = elem_->GetB2Fixture()->GetNext();

            elem_ = static_cast<Fixture*>(next->GetUserData());
            return *this;
        }

        inline IteratorImpl operator++(int)
        {
            IteratorImpl old = *this;

            operator++();
            return old;
        }

        inline bool operator==(const IteratorImpl& rhs) const
        {
            return elem_ == rhs.elem_;
        }

        inline bool operator!=(const IteratorImpl& rhs) const
        {
            return !operator==(rhs);
        }

    private:
        pointer elem_;
    };

public:
    using value_type     = Fixture;
    using iterator       = IteratorImpl<value_type>;
    using const_iterator = IteratorImpl<const value_type>;

    inline FixtureList()
        : first_(nullptr)
    {
    }

    inline FixtureList(value_type* first)
        : first_(first)
    {
    }

    inline const value_type& front() const
    {
        return *first_;
    }

    inline value_type& front()
    {
        return *first_;
    }

    inline iterator begin()
    {
        return iterator(first_);
    }

    inline const_iterator begin() const
    {
        return cbegin();
    }

    inline const_iterator cbegin() const
    {
        return const_iterator(first_);
    }

    inline iterator end()
    {
        return iterator(nullptr);
    }

    inline const_iterator end() const
    {
        return cend();
    }

    inline const_iterator cend() const
    {
        return const_iterator(nullptr);
    }

private:
    value_type* first_;
};

/** @} */

inline bool Fixture::IsSensor() const
{
    KGE_ASSERT(fixture_);
    return fixture_->IsSensor();
}

inline void Fixture::SetSensor(bool sensor)
{
    KGE_ASSERT(fixture_);
    fixture_->SetSensor(sensor);
}

inline float Fixture::GetDensity() const
{
    KGE_ASSERT(fixture_);
    return fixture_->GetDensity();
}

inline void Fixture::SetDensity(float density)
{
    KGE_ASSERT(fixture_);
    fixture_->SetDensity(density);
}

inline float Fixture::GetFriction() const
{
    KGE_ASSERT(fixture_);
    return fixture_->GetFriction();
}

inline void Fixture::SetFriction(float friction)
{
    KGE_ASSERT(fixture_);
    fixture_->SetFriction(friction);
}

inline float Fixture::GetRestitution() const
{
    KGE_ASSERT(fixture_);
    return fixture_->GetRestitution();
}

inline void Fixture::SetRestitution(float restitution)
{
    KGE_ASSERT(fixture_);
    fixture_->SetRestitution(restitution);
}

inline bool Fixture::IsValid() const
{
    return fixture_ != nullptr;
}

inline b2Fixture* Fixture::GetB2Fixture() const
{
    return fixture_;
}

inline void Fixture::SetB2Fixture(b2Fixture* fixture)
{
    fixture_ = fixture;
}

inline bool Fixture::operator==(const Fixture& rhs) const
{
    return fixture_ == rhs.fixture_;
}

inline bool Fixture::operator!=(const Fixture& rhs) const
{
    return fixture_ != rhs.fixture_;
}

}  // namespace physics
}  // namespace kiwano
