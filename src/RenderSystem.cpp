#include "RenderSystem.h"

RenderSystem::RenderSystem(float scaleFactor, float radius_scale)
    : m_scale_factor(scaleFactor), m_radius_scale(radius_scale) {

}

void RenderSystem::endFrame() {
  EndDrawing();
}

void RenderSystem::newFrame() {
  BeginDrawing();
  ClearBackground(WHITE);
}

//https://www.geogebra.org/calculator/dj5dtntg
void DrawTriangleAtEndOfLine(Vector2 point1, Vector2 point2, float triangleSize, Color color) {
  Vector2 direction = Vector2Subtract(point2, point1);
  Vector2 normalizedDirection = Vector2Normalize(direction);
  Vector2 perpendicularVector = {-normalizedDirection.y, normalizedDirection.x};

  //ABC
  //  /C\
    // /   \
    //A ____B
  Vector2 triangle[3];
  //A
  triangle[0] = point2;
  //B
  triangle[1] = Vector2Add(Vector2Subtract(point2, Vector2Scale(normalizedDirection, triangleSize / 2)),
                           Vector2Scale(perpendicularVector, -0.35f * triangleSize));
  //C
  triangle[2] = Vector2Add(Vector2Subtract(point2, Vector2Scale(normalizedDirection, triangleSize / 2)),
                           Vector2Scale(perpendicularVector, 0.35f * triangleSize));

  DrawTriangle(triangle[0], triangle[1], triangle[2], color);
}

void RenderSystem::render(ChargeSystem &charge_system) const {
  //Center point
  DrawCircleV(Vector2{0, 0}, 3.0f, BLACK);

  //Draw the charges
  for (const auto &charge : charge_system.getChargesList()) {
    Vector2 charge_position = {charge.position.x * m_scale_factor, -charge.position.y * m_scale_factor};
    float contourSize = 1.5f;
    DrawCircleV(charge_position, charge.radius * m_radius_scale + contourSize, BLACK);
    DrawCircleV(charge_position, charge.radius * m_radius_scale, charge.color);
  }

  //Draw the vectors
  for (const auto &vec : m_vector_render_list) {
    //The segment has to be reduced to
    //avoid puncturing of the triangle at the end
    Vector2 temp_vec_head = Vector2MoveTowards(vec.head, vec.tail, 5.f);
    DrawLineEx(vec.tail, temp_vec_head, 2.0f, vec.color);
    DrawTriangleAtEndOfLine(vec.tail, vec.head, 10.f, vec.color);
  }

  //Cursor drawing
  float contourSize = 1.5f;
  for (const auto &cursor : charge_system.getCursorList()) {
    Vector2 refined_cursor_position = {cursor.position.x * m_scale_factor, -cursor.position.y * m_scale_factor};

    DrawCircleV(refined_cursor_position, (cursor.radius * m_radius_scale) + contourSize, BLACK);
    DrawCircleV(refined_cursor_position, cursor.radius * m_radius_scale, cursor.color);
  }
}

void RenderSystem::update(KeysStatus &keys_status, ChargeSystem &charge_system, Features &features) {
  m_vector_render_list.clear();

  for (const auto &cursor : charge_system.getCursorList()) {
    Vector2 refined_cursor_position = {cursor.position.x * m_scale_factor, -cursor.position.y * m_scale_factor};
    for (const auto &vec : cursor.vectors) {
      Vector temp_render_vector = {refined_cursor_position, Vector2Add(Vector2Zero(),
                                                                Vector2{vec.head.x * m_scale_factor,
                                                                        vec.head.y * -m_scale_factor}), vec.color};
      m_vector_render_list.push_back(temp_render_vector);
    }
    Vector vec_res = {refined_cursor_position, Vector2Add(refined_cursor_position,
                                                          Vector2{cursor.e_res_vec.x * m_scale_factor,
                                                                  cursor.e_res_vec.y * -m_scale_factor}), PINK};
    m_vector_render_list.push_back(vec_res);
  }
  //Grid vector
  /*
  if (features.showVectorGrid) {
    static Cursor_Point tempCursor = {Vector2{-10 * m_scale_factor, -10 * m_scale_factor}, 0, Color{0, 0, 0}, false};
    for (int index_x = 0; index_x < 8; index_x++) {
      for (int index_y = 0; index_y < 8; index_y++) {
        //TODO vector grid reimplementation
        //charge_system.compute_e(tempCursor);
        Vector tempVec = {tempCursor.position, Vector2Add(tempCursor.position,
                                                          Vector2{tempCursor.e_res_vec.x * m_scale_factor,
                                                                  tempCursor.e_res_vec.y * -m_scale_factor}), RED};

        m_vector_render_list.push_back(tempVec);

        tempCursor.position.x = tempCursor.position.x + 10.f * (float) index_y;
      }
      tempCursor.position.x = -10 * m_scale_factor;
      tempCursor.position.y = tempCursor.position.y + 10.f * (float) index_x;
    }
    tempCursor.position = {-10 * m_scale_factor, -10 * m_scale_factor};
  }
  */
}
