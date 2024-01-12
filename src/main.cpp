#include <cstdio>
#include <memory>
#include <rolling_grid/grid.hpp>

template <class T>
void print_grid(RollingGridPtr<T> grid_ptr)
{
    for (size_t y = 0; y < grid_ptr->m_size_y; y++)
    {
        for (size_t x = 0; x < grid_ptr->m_size_x; x++)
        {
            printf("% 4d", (*grid_ptr)(x, y, 0));
        }
        printf("\n");
    }
        
}

int main(int argc, char* argv[])
{
    auto grid_ptr = std::make_shared<RollingGrid<int>>(10, 5, 1);
    
    for (int i = 0; i < 10*5; i++)
    {
        (*grid_ptr)[i] = i;
    }
    
    print_grid(grid_ptr);

    grid_ptr->roll_clear(-2, 1, 0, 0);

    printf("\nAfter:\n");
    print_grid(grid_ptr);
}