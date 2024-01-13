#pragma once

#include <cstdlib>
#include <cstddef>
#include <memory>
#include <tuple>
#include <cmath>
#include <cstdio> // TODO: remove after debug

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

    inline size_t size_x() { return m_size_x; };
    inline size_t size_y() { return m_size_y; };
    inline size_t size_z() { return m_size_z; };

    void print_buffer(); // TODO: remove after debug

private:
    inline size_t coord_to_idx(size_t x, size_t y, size_t z);
    inline size_t coord_to_bidx(size_t x, size_t y, size_t z);
    inline std::tuple<size_t, size_t, size_t> idx_to_coord(size_t idx);
    inline size_t idx_to_bidx(size_t idx);

    const size_t m_size_x, m_size_y, m_size_z, m_size;
    size_t m_ori_x, m_ori_y, m_ori_z;
    ElementT* m_buf;
};

template <class T, class Q>
inline T bounded(T value, Q const& size)
{
    value = value % static_cast<T>(size);
    return value < 0 ? value + size : value;
}

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
    // TODO: fix
    // For positive roll, fill old origin to new origin
    // For negative roll, fill old size limit to new size limit
    
    long new_ori_x  = bounded(static_cast<int>(m_ori_x) + dx, m_size_x);
    long lim_x = bounded(m_ori_x + m_size_x - 1, m_size_x);
    long new_lim_x = bounded(new_ori_x + m_size_x - 1, m_size_x);

    for (size_t y = 0; y < m_size_y; y++)
    {
        for (size_t z = 0; z < m_size_z; z++)
        {
            if (dx >= 0)
            {
                for (long x = m_ori_x; x != new_ori_x; x = (x + 1) % static_cast<long>(m_size_x))
                {
                    m_buf[coord_to_bidx(x, y, z)] = fill_value;
                }
            }
            else
            {
                for (long x = lim_x; x != new_lim_x; x--)
                {
                    x = x < 0 ? m_size_x + x : x;
                    m_buf[coord_to_bidx(x, y, z)] = fill_value;
                }
            }
        }
    }

    long new_ori_y  = bounded(static_cast<int>(m_ori_y) + dy, m_size_y);
    long lim_y = bounded(m_ori_y + m_size_y - 1, m_size_y);
    long new_lim_y = bounded(new_ori_y + m_size_y - 1, m_size_y);

    for (size_t x = 0; x < m_size_x; x++)
    {
        for (size_t z = 0; z < m_size_z; z++)
        {
            if (dy >= 0)
            {
                for (long y = m_ori_y; y != new_ori_y; y = (y + 1) % static_cast<long>(m_size_y))
                {
                    m_buf[coord_to_bidx(x, y, z)] = fill_value;
                }
            }
            else
            {
                for (long y = lim_y; y != new_lim_y; y--)
                {
                    y = y < 0 ? m_size_y + y : y;
                    m_buf[coord_to_bidx(x, y, z)] = fill_value;
                }
            }
        }
    }

    long new_ori_z  = bounded(static_cast<int>(m_ori_z) + dz, m_size_z);
    long lim_z = bounded(m_ori_z + m_size_z - 1, m_size_z);
    long new_lim_z = bounded(new_ori_z + m_size_z - 1, m_size_z);

    for (size_t x = 0; x < m_size_x; x++)
    {
        for (size_t y = 0; y < m_size_y; y++)
        {
            if (dz >= 0)
            {
                for (long z = m_ori_z; z != new_ori_z; z = (z + 1) % static_cast<long>(m_size_z))
                {
                    m_buf[coord_to_bidx(x, y, z)] = fill_value;
                }
            }
            else
            {
                for (long z = lim_z; z != new_lim_z; z--)
                {
                    z = z < 0 ? m_size_z + z : z;
                    m_buf[coord_to_bidx(x, y, z)] = fill_value;
                }
            }
        }
    }

    m_ori_x = new_ori_x;
    m_ori_y = new_ori_y;
    m_ori_z = new_ori_z;
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
    return m_buf[idx_to_bidx(idx)];
}

template <class ElementT>
const ElementT &RollingGrid<ElementT>::operator[](size_t idx) const
{
    return m_buf[idx_to_bidx(idx)];
}

template <class ElementT>
inline ElementT &RollingGrid<ElementT>::operator()(size_t x, size_t y, size_t z)
{
    return m_buf[coord_to_bidx(x, y, z)];
}

template <class ElementT>
inline const ElementT &RollingGrid<ElementT>::operator()(size_t x, size_t y, size_t z) const
{
    return m_buf[coord_to_bidx(x, y, z)];
}

// TODO: remove after debug
template <class ElementT>
inline void RollingGrid<ElementT>::print_buffer()
{
    for (size_t y = 0; y < m_size_y; y++)
    {
        for (size_t x = 0; x < m_size_x; x++)
        {
            auto val = m_buf[coord_to_idx(x, y, 0)];
            printf("% 4d", val);
        }
        printf("\n");
    }
}

/**
 * Converts a set of coordinates to flat array index.
*/
template <class ElementT>
inline size_t RollingGrid<ElementT>::coord_to_idx(size_t x, size_t y, size_t z)
{
    return (x * m_size_y * m_size_z) + (y * m_size_z) + z;
}

/**
 * Converts a set of coordinates to internal buffer index; 
*/
template <class ElementT>
inline size_t RollingGrid<ElementT>::coord_to_bidx(size_t x, size_t y, size_t z)
{
    x = (x + m_ori_x) % m_size_x;
    y = (y + m_ori_y) % m_size_y;
    z = (z + m_ori_z) % m_size_z;

    return coord_to_idx(x, y, z);
}

/**
 * Converts flat array index to set of coordinates.
*/
template <class ElementT>
inline std::tuple<size_t, size_t, size_t> RollingGrid<ElementT>::idx_to_coord(size_t idx)
{
    auto div = std::div(long(idx), long(m_size_y * m_size_z));
    size_t x = div.quot;
    div = std::div(div.rem, long(m_size_z));
    size_t y = div.quot, z = div.rem;

    return std::tuple<size_t, size_t, size_t>(x, y, z);
}


/**
 * Converts flat array index to internal buffer index.
*/
template <class ElementT>
inline size_t RollingGrid<ElementT>::idx_to_bidx(size_t idx)
{
    size_t x, y, z;
    std::tie(x, y, z) = idx_to_coord(idx);
    return coord_to_bidx(x, y, z);
}
