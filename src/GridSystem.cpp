#include "GridSystem.h"
void GridSystem::draw_grid_background() const {
  for (int i = -m_grid_size; i < m_grid_size; i++) {
    DrawLineEx({(float) (m_grid_size * m_tile_size), (float) (m_tile_size * i)},
               {(float) (-m_grid_size * m_tile_size), (float) (m_tile_size * i)},
               1.2,
               m_grid_color);

    DrawLineEx({(float) (m_tile_size * i), (float) (m_grid_size * m_tile_size)},
               {(float) (m_tile_size * i), (float) (-m_grid_size * m_tile_size)},
               1.2,
               m_grid_color);
  }
}
void GridSystem::draw_axis() const {
  DrawLineEx({(float) (m_grid_size * m_tile_size), 0.0}, {(float) (-m_grid_size * m_tile_size), 0.0}, 2, m_axis_color);
  DrawLineEx({0.0, (float) (m_grid_size * m_tile_size)}, {0, (float) (-m_grid_size * m_tile_size)}, 2, m_axis_color);
  for (int i = -m_grid_size; i < m_grid_size; i++) {
    DrawText(TextFormat("%d", i * m_tile_size / 10), i * m_tile_size + 5, 5, 5, m_text_color);
    DrawText(TextFormat("%d", i * m_tile_size / 10), 5, -i * m_tile_size + 5, 5, m_text_color);
  }
}
GridSystem::GridSystem(int gridSize, int tileSize, Color gridColor, Color axisColor, Color textColor) : m_grid_size{
    gridSize}, m_tile_size{tileSize}, m_grid_color{gridColor}, m_axis_color{axisColor}, m_text_color{textColor} {}
