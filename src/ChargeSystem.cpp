#include "ChargeSystem.h"
#include "imgui.h"

void applyScale(double &valueToScale, const Scale &scale) {
  switch (scale) {
    case PICO:valueToScale /= 1000000000000;
      break;
    case NANO:valueToScale /= 1000000000;
      break;
    case MICRO:valueToScale /= 1000000;
      break;
    case MILI:valueToScale /= 1000;
      break;
  }
}

int Charge_particle::global_index = 0;
int Cursor_Point::global_index = 0;

std::vector<CursorResult> Compute_Vectors_E(Cursor_Point &cursor,
                                            std::vector<Charge_particle> &m_charge_list,
                                            Constants &constants) {
  std::vector<CursorResult> cursor_results = {};

  if (m_charge_list.empty()) {
    cursor.e_res_vec = Vector2Zero();
    cursor.vectors.clear();
    return cursor_results;
  }

  cursor.vectors.clear();

  CursorResult result = {};
  Vector2 resVEC = {cursor.position.x, cursor.position.y};
  Vector2 val_resVec = {0, 0};
  for (const auto &charge : m_charge_list) {
    double scaled_charge = charge.charge_unscaled;
    applyScale(scaled_charge, charge.scale);

    result.r = sqrt(pow((charge.position.x - cursor.position.x), 2) + pow((charge.position.y - cursor.position.y), 2));
    result.mag_E = scaled_charge / (4 * PI * constants.m_e * pow(result.r, 2));

    result.e_x = cursor.position.x - charge.position.x;
    result.e_y = cursor.position.y - charge.position.y;

    result.E_x = (result.e_x / sqrt(pow(result.e_x, 2) + pow(result.e_y, 2))) * result.mag_E;
    result.E_y = (result.e_y / sqrt(pow(result.e_x, 2) + pow(result.e_y, 2))) * result.mag_E;

    result.abs_mag_E = sqrt(pow(result.E_x, 2) + pow(result.E_y, 2));

    result.pos_E_x = result.E_x + cursor.position.x;
    result.pos_E_y = result.E_y + cursor.position.y;

    cursor.vectors.push_back(Vector{cursor.position,
                                    Vector2{static_cast<float>(result.pos_E_x), static_cast<float>(result.pos_E_y)},
                                    charge.color});

    resVEC.x += static_cast<float>(result.E_x);
    resVEC.y += static_cast<float>(result.E_y);

    result.res_E_value = sqrt(pow(result.E_x, 2) + pow(result.E_y, 2));

    val_resVec = Vector2Add(val_resVec, Vector2{static_cast<float>(result.E_x), static_cast<float>(result.E_y)});
    cursor_results.push_back(result);
  }
  cursor.e_res_vec = val_resVec;

  return cursor_results;
}

void ChargeSystem::compute_e(Features *features) {
  for (auto &cursor : m_cursor_list) {
    if (cursor.active)
      cursor.cursor_result = Compute_Vectors_E(cursor, m_charge_list, m_constants);
  }
}

ChargeSystem::ChargeSystem() : m_constants{0.0, 0.0} {}

void ChargeSystem::addCharge(Vector2 position, double charge, Scale scale, float radius, Color color) {
  m_charge_list.emplace_back(position, charge, scale, radius, color);
}

void ChargeSystem::setConstants(double e0, Scale scale, double er) {
  applyScale(e0, scale);
  m_constants.compute_e(e0, er);
}

void ChargeSystem::addCursor(Vector2 position, float radius, Color color, bool mainCursor) {
  m_cursor_list.emplace_back(position, radius, color, true, mainCursor);
}

void ChargeSystem::addCursor(Vector2 position, float radius, Color color, bool no_contour, bool mainCursor) {
  m_cursor_list.emplace_back(position, radius, color, no_contour, mainCursor);
}

Cursor_Point &ChargeSystem::getMainCursor() {
    int index = 0;
    for (const auto &cursor : m_cursor_list) {
      if (cursor.mainCursor) {
        return m_cursor_list[index];
      }
      index++;
    }
    //If no cursor is set as the main cursor, then return the first one
    m_cursor_list[0].color = YELLOW;
    return m_cursor_list[0];
}

void ChargeSystem::removeCharge(int index) {
  for (auto &charge : m_charge_list) {
    if (charge.index == index) {
      m_charge_list.erase(m_charge_list.begin() + index);
      Charge_particle::getGlobalIndex() -= 1;
      break;
    }
  }
  for (int i = 0; i < m_charge_list.size(); i++) {
    m_charge_list[i].index = i;
  }
}

void ChargeSystem::removeCursor(int index) {
  for (auto &cursor : m_cursor_list) {
    if (cursor.index == index) {
      m_cursor_list.erase(m_cursor_list.begin() + index);
      Cursor_Point::getGlobalIndex() -= 1;
      break;
    }
  }
  for (int i = 0; i < m_cursor_list.size(); i++) {
    m_cursor_list[i].index = i;
  }
}

//Set main cursor and resets all other cursors to normal cursors
void ChargeSystem::setMainCursor(int index) {
  for (auto &cursor : m_cursor_list) {
    cursor.mainCursor = false;
  }
  m_cursor_list[index].mainCursor = true;
  m_cursor_list[index].color = YELLOW;
}

//The charge system is valid only if at least one cursor is present
bool ChargeSystem::isChargeSystemValid() {
  if (m_cursor_list.empty()) {
    return false;
  } else {
    return true;
  }
}

void ChargeSystem::resetCharges() {
  m_charge_list.clear();
  Charge_particle::getGlobalIndex() = 0;
}

void ChargeSystem::resetCursors() {
  m_cursor_list.clear();
  Cursor_Point::getGlobalIndex() = 0;
}
