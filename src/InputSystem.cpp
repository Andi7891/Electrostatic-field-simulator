#include "InputSystem.h"

void InputSystem::update_input(float cameraZoom) {
  if (IsKeyPressed(KEY_F1)) KeysStatus.F1 = !KeysStatus.F1;
  if (IsKeyPressed(KEY_F2)) KeysStatus.F2 = !KeysStatus.F2;
  if (IsKeyPressed(KEY_F3)) KeysStatus.F3 = !KeysStatus.F3;
  if (IsKeyPressed(KEY_F4)) KeysStatus.F4 = !KeysStatus.F4;
  if (IsKeyPressed(KEY_F5)) KeysStatus.F5 = !KeysStatus.F5;
  if (IsKeyPressed(KEY_SPACE)) KeysStatus.space = !KeysStatus.space;
  if (IsKeyPressed(KEY_F10)) KeysStatus.debug = !KeysStatus.debug;
  if (IsKeyPressed(KEY_R)) KeysStatus.R = !KeysStatus.R;
  if (IsKeyPressed(KEY_H)) KeysStatus.H = !KeysStatus.H;
  if (IsKeyPressed(KEY_E)) KeysStatus.E = !KeysStatus.E;
  if (IsKeyPressed(KEY_A)) KeysStatus.A = !KeysStatus.A;
  if (IsKeyPressed(KEY_D)) KeysStatus.D = !KeysStatus.D;

  int windowWidth = GetScreenWidth();
  int windowHeight = GetScreenHeight();

  Vector2 raw_position = GetMousePosition();
  Vector2 refined_position = {0, 0};

  //Offsetting the mouse position to center
  Vector2 origin_offset = {(raw_position.x - (float) windowWidth / 2), (-raw_position.y + (float) windowHeight / 2)};

  refined_position.x = origin_offset.x / (m_scale_factor * cameraZoom);
  refined_position.y = origin_offset.y / (m_scale_factor * cameraZoom);

  m_ref_mouse_pos = refined_position;
}

InputSystem::InputSystem(float scaleFactor) : m_scale_factor{scaleFactor}, m_ref_mouse_pos{0, 0} {}
Vector2 InputSystem::getRefMousePos() const { return m_ref_mouse_pos; }
