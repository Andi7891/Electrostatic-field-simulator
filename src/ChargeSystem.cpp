#include "ChargeSystem.h"
#include "imgui.h"

#define RAD_TO_DEG(x) ((x / PI) * 180)

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

/*
struct CursorResult {
  double rel_pos;
  double radian;
  double angle;
  double e_p;
  double e;
  Vector2 resVec;
};
*/

int Charge_particle::global_index = 0;

struct CursorResult {
  double r;
  double mag_E;
  double e_x;
  double e_y;
  double E_x;
  double E_y;
  double abs_mag_E;
  double pos_E_x;
  double pos_E_y;
  double res_E_value;
};

std::vector<CursorResult> Compute_Vectors_E(Cursor_Point &cursor,
                                            std::vector<Charge_particle> &m_charge_list,
                                            Constants &constants) {
  if (m_charge_list.empty()) {
    std::vector<CursorResult> cursor_results = {};
    cursor.e_res_vec = Vector2Zero();
    return cursor_results;
  }

  cursor.vectors.clear();
  std::vector<CursorResult> cursor_results = {};

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
                                    Vector2{static_cast<float>(result.pos_E_x),
                                            static_cast<float>(result.pos_E_y)},
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
  std::vector<CursorResult> cursor_results = Compute_Vectors_E(getMainCursor(), m_charge_list, m_constants);
  //GUI
  if (features != nullptr && features->showResultPanel) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center = ImVec2{center.x / 2.5f, center.y};
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.f);
    ImGui::Begin("Values", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PopStyleVar();

    ImGui::Text("Cursor pos: %.3f %.3f", getMainCursor().position.x, getMainCursor().position.y);

    ImGui::Separator();
    int index = 1;
    for (const auto &charge_result : cursor_results) {
      ImGui::Text("Particle %d", index);
      ImGui::Text(TextFormat("r%d: %lf", index, charge_result.r));
      ImGui::Text(TextFormat("E%d: %lf", index, charge_result.mag_E));
      ImGui::Text(TextFormat("e%dx: (%lf %lf)", index, charge_result.e_x, charge_result.e_y));
      ImGui::Text(TextFormat("E%dx: (%lf %lf)", index, charge_result.E_x, charge_result.E_y));
      ImGui::Text(TextFormat("Abs_mag_E%d: %lf", index, charge_result.abs_mag_E));
      ImGui::Text(TextFormat("Pos_E_%d: (%lf %lf)", index, charge_result.pos_E_x, charge_result.pos_E_y));
      ImGui::Text(TextFormat("Val_Res_E_%d: %lf", index, charge_result.res_E_value));
      ImGui::Separator();
      index++;
    }

    ImGui::Separator();

    ImGui::Text(TextFormat("Val_Res_Vec: (%f %f)", getMainCursor().e_res_vec.x, getMainCursor().e_res_vec.y));
    ImGui::Text(TextFormat("Val_Res_Vec_Mag: %f", Vector2Length(getMainCursor().e_res_vec)));

    ImGui::End();
  }
}

/*
void ChargeSystem::compute_e(Features *features) {
  std::vector<CursorResult> cursor_results = helperFunction(getMainCursor(), m_charge_list, m_constants);
  //GUI
  if (features != nullptr && features->showResultPanel) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    center = ImVec2{center.x / 2.5f, center.y};
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.f);
    ImGui::Begin("Values", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PopStyleVar();

    ImGui::Text("Cursor pos: %.3f %.3f", getMainCursor().position.x, getMainCursor().position.y);

    ImGui::Separator();
    int index = 1;
    for(const auto& charge_result : cursor_results) {
      ImGui::Text("Particle %d", index);
      ImGui::Text(TextFormat("Rel_Pos: %lf", charge_result.rel_pos));
      ImGui::Text(TextFormat("Radians: %lf", charge_result.radian));
      ImGui::Text(TextFormat("Degree: %lf", charge_result.angle));
      ImGui::Text(TextFormat("Ep_%d: %lf", index, charge_result.e_p));
      ImGui::Text(TextFormat("E%d: %lf", index, charge_result.e));
      ImGui::Text(TextFormat("Res_Vec_%d: %lf %lf", index, charge_result.resVec.x, charge_result.resVec.y));
      ImGui::Separator();
      index++;
    }

    ImGui::Separator();

    ImGui::Text(TextFormat("Resultant_Vec: %f %f", getMainCursor().e_res_vec.x, getMainCursor().e_res_vec.y));
    ImGui::Text(TextFormat("Resultant_Vec_mag: %f", Vector2Length(getMainCursor().e_res_vec)));

    ImGui::End();
  }
}
*/
ChargeSystem::ChargeSystem() : m_constants{0.0, 0.0} {}

void ChargeSystem::addCharge(Vector2 position, double charge, Scale scale, float radius, Color color) {
  m_charge_list.emplace_back(position, charge, scale, radius, color);
}

void ChargeSystem::setConstants(double e0, Scale scale, double er) {
  applyScale(e0, scale);
  m_constants.compute_e(e0, er);
}

Vector2 ChargeSystem::compute_e_grid(Cursor_Point &cursor) {
  Vector2 resVec_1 = {0.0f, 0.0f};

  //Scaling
  double charge_1 = m_charge_list[0].charge_unscaled;
  applyScale(charge_1, m_charge_list[0].scale);

  double
      rel_pos_1 = (cursor.position.x - m_charge_list[0].position.x) / (m_charge_list[0].position.y - cursor.position.y);
  double radian_1 = atan(rel_pos_1);

  double e1_p = charge_1 / (4 * PI * m_constants.m_e);
  double e1 =
      e1_p * (1 / sqrt(pow(pow(cursor.position.x, 2) + pow((cursor.position.y - m_charge_list[0].position.y), 2), 2)));

  resVec_1.x = (float) (sin(radian_1) * e1);
  resVec_1.y = (float) (-cos(radian_1) * e1);

  Vector2 resVec_2 = {0.0f, 0.0f};

  //Scaling
  double charge_2 = m_charge_list[1].charge_unscaled;
  applyScale(charge_2, m_charge_list[1].scale);

  double rel_pos_2 =
      (cursor.position.x - m_charge_list[1].position.x) / abs((m_charge_list[1].position.y - cursor.position.y));
  double radian_2 = atan(rel_pos_2);

  double e2_p = charge_2 / (4 * PI * m_constants.m_e);
  double e2 =
      e2_p * (1 / sqrt(pow(pow(cursor.position.x, 2) + pow((cursor.position.y - m_charge_list[1].position.y), 2), 2)));

  resVec_2.x = (float) (sin(radian_2) * e2);
  resVec_2.y = (float) (cos(radian_2) * e2);

  Vector2 res_vec = Vector2Add(resVec_1, resVec_2);
  cursor.e_res_vec = res_vec;
  return res_vec;
}

void ChargeSystem::addCursor(Vector2 position, float radius, Color color, bool mainCursor) {
  m_cursor_list.emplace_back(position, radius, color, mainCursor);
}
