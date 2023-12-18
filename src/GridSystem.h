#ifndef ESFS_SRC_GRIDSYSTEM_H_
#define ESFS_SRC_GRIDSYSTEM_H_

#endif //ESFS_SRC_GRIDSYSTEM_H_

#include "raylib.h"

class GridSystem {
 public:

  void render() {
    draw_grid_background();
    draw_axis();
  }

  GridSystem() = delete;
  explicit GridSystem(int gridSize, int tileSize, Color gridColor, Color axisColor, Color textColor);

 private:
  void draw_grid_background() const;
  void draw_axis() const;

 private:
  int m_grid_size;
  int m_tile_size;
  Color m_grid_color;
  Color m_axis_color;
  Color m_text_color;
};