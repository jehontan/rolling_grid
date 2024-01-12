#pragma once

#include <cstdlib>
#include <cstddef>
#include <memory>
#include <tuple>

template <class ElementT>
class RollingGrid
{
    static_assert(std::is_pod<ElementT>::value, "ElementT must be POD");

public:
    explicit RollingGrid(size_t size_x, size_t size_y, size_t size_z);
    ~RollingGrid();

    void roll(int dx, int dy, int dz);
    void roll_clear(int dx, int dy, int dz, ElementT&& fill_value);
    void fill(size_t x0, size_t x1, size_t y0, size_t y1, size_t z0, size_t z1, ElementT value);
    
    ElementT& operator[](size_t idx);
    const ElementT& operator[](size_t idx) const;

    ElementT& operator()(size_t x, size_t y, size_t z);
    const ElementT& operator()(size_t x, size_t y, size_t z) const;

    inline size_t coord_to_idx(size_t x, size_t y, size_t z);

    const size_t m_size_x, m_size_y, m_size_z, m_size;

private:
    
    size_t m_ori_x, m_ori_y, m_ori_z;
    ElementT* m_buf;
};

template <class T>
using RollingGridPtr = std::shared_ptr<RollingGrid<T>>;

template <class T>
using RollingGridConstPtr = std::shared_ptr<const RollingGrid<T>>;

/////////// Implementation ///////////

template <class ElementT>
RollingGrid<ElementT>::RollingGrid(size_t size_x, size_t size_y, size_t size_z)
: m_size_x{size_x}
, m_size_y{size_y}
, m_size_z{size_z}
, m_size{size_x * size_y * size_z}
, m_ori_x{0}, m_ori_y{0}, m_ori_z{0}
{
    m_buf = (ElementT*) calloc(m_size, sizeof(ElementT));
}

template <class ElementT>
RollingGrid<ElementT>::~RollingGrid()
{
    free(m_buf);
}
template <class ElementT>
void RollingGrid<ElementT>::roll(int dx, int dy, int dz)
{
    // move the origin
    int ori_x = (static_cast<int>(m_ori_x) + dx) % static_cast<int>(m_size_x);
    m_ori_x = ori_x < 0 ? m_size_x + ori_x : ori_x;

    int ori_y = (static_cast<int>(m_ori_y) + dy) % static_cast<int>(m_size_y);
    m_ori_y = ori_y < 0 ? m_size_y + ori_y : ori_y;

    int ori_z = (static_cast<int>(m_ori_z) + dz) % static_cast<int>(m_size_z);
    m_ori_z = ori_z < 0 ? m_size_z + ori_z : ori_z;
}

template <class ElementT>
void RollingGrid<ElementT>::roll_clear(int dx, int dy, int dz, ElementT&& fill_value)
{
    roll(dx, dy, dz);

    size_t x0 = dx < 0 ? m_ori_x : 0;
    size_t x1 = dx < 0 ? m_size_x : m_ori_x;
    size_t y0 = dy < 0 ? m_ori_y : 0;
    size_t y1 = dy < 0 ? m_size_y : m_ori_y;
    size_t z0 = dz < 0 ? m_ori_z : 0;
    size_t z1 = dz < 0 ? m_size_z : m_ori_z;
    
    fill(x0, x1, y0, y1, z0, z1, fill_value);
}

template <class ElementT>
void RollingGrid<ElementT>::fill(size_t x0, size_t x1, size_t y0, size_t y1, size_t z0, size_t z1, ElementT value)
{
    for (size_t x = 0; x < m_size_x; x++)
    {
        for (size_t y = 0; y < m_size_y; y++)
        {
            for (size_t z = 0; z < m_size_z; z++)
            {
                if ((x0 <= x && x < x1) || (y0 <= y && y < y1) || (z0 <= z && z < z1))
                {
                    size_t idx = coord_to_idx(x, y, z);
                    m_buf[idx] = value;
                }
            }
        }
    }

}

template <class ElementT>
ElementT &RollingGrid<ElementT>::operator[](size_t idx)
{
    return m_buf[idx];
}

template <class ElementT>
const ElementT &RollingGrid<ElementT>::operator[](size_t idx) const
{
    return m_buf[idx];
}

template <class ElementT>
inline ElementT &RollingGrid<ElementT>::operator()(size_t x, size_t y, size_t z)
{
    return m_buf[coord_to_idx(x, y, z)];
}

template <class ElementT>
inline const ElementT &RollingGrid<ElementT>::operator()(size_t x, size_t y, size_t z) const
{
    return m_buf[coord_to_idx(x, y, z)];
}

template <class ElementT>
inline size_t RollingGrid<ElementT>::coord_to_idx(size_t x, size_t y, size_t z)
{
    return (x * m_size_y * m_size_z) + (y * m_size_z) + z;
}