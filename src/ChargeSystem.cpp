#include "ChargeSystem.h"
#include "imgui.h"

#define RAD_TO_DEG(x) ((x / PI) * 180)

void applyScale(double& valueToScale, const Scale& scale) {
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

struct CursorResult {
  double rel_pos;
  double radian;
  double angle;
  double e_p;
  double e;
  Vector2 resVec;
};

std::vector<CursorResult> helperFunction(Cursor_Point& cursor, std::vector<Charge_particle>& m_charge_list, Constants& m_constants) {
  cursor.vectors.clear();
  std::vector<CursorResult> cursor_results = {};
  /*
  auto copy_cursor = cursor;
  for (const auto& charge : m_charge_list) {
    CursorResult result = {};
    //Scaling
    double charge_charge = charge.charge_unscaled;
    applyScale(charge_charge, charge.scale);

    //Position calculation
    result.rel_pos = (copy_cursor.position.x - charge.position.x) / (charge.position.y - copy_cursor.position.y);
    result.radian = atan(result.rel_pos);
    result.angle = RAD_TO_DEG(result.radian);

    //Electric field calculation
    result.e1_p = charge_charge / (4 * PI * m_constants.m_e);
    result.e1 = result.e1_p * (1 / sqrt(pow(pow(copy_cursor.position.x, 2) + pow((copy_cursor.position.y - charge.position.y), 2), 2)));

    //Converting to vectors
    Vector2 res_vec = {(float) (sin(result.radian) * result.e1),
                        (float)(cos(result.radian) * result.e1)};
    result.resVec = res_vec;
    cursor_resVec = Vector2Add(cursor_resVec, res_vec);

    //Storing results for gui
    cursor_results.emplace_back(result);
  }
  */

  CursorResult result_1 = {};

  //Scaling
  double charge_1 = m_charge_list[0].charge_unscaled;
  applyScale(charge_1, m_charge_list[0].scale);

  result_1.rel_pos = (cursor.position.x - m_charge_list[0].position.x) / (m_charge_list[0].position.y - cursor.position.y);
  result_1.radian = atan(result_1.rel_pos);
  result_1.angle = RAD_TO_DEG(result_1.radian);

  result_1.e_p = charge_1 / (4 * PI * m_constants.m_e);
  result_1.e = result_1.e_p * (1 / sqrt(pow(pow(cursor.position.x, 2) + pow((cursor.position.y - m_charge_list[0].position.y), 2), 2)));

  result_1.resVec.x = (float) (sin(result_1.radian) * result_1.e);
  result_1.resVec.y = (float) (-cos(result_1.radian) * result_1.e);

  //TODO create constructor for render_vector and replace push_back with emplace_back for performance reasons
  cursor.vectors.push_back(Vector{Vector2{cursor.position.x, cursor.position.y},
                              Vector2{result_1.resVec.x, result_1.resVec.y},
                              RED});
  cursor_results.emplace_back(result_1);

  CursorResult result_2 = {};

  //Scaling
  double charge_2 = m_charge_list[1].charge_unscaled;
  applyScale(charge_2, m_charge_list[1].scale);

  result_2.rel_pos = (cursor.position.x - m_charge_list[1].position.x) / abs((m_charge_list[1].position.y - cursor.position.y));
  result_2.radian = atan(result_2.rel_pos);
  result_2.angle = RAD_TO_DEG(result_2.radian);

  result_2.e_p = charge_2 / (4 * PI * m_constants.m_e);
  result_2.e = result_2.e_p * (1 / sqrt(pow(pow(cursor.position.x, 2) + pow((cursor.position.y - m_charge_list[1].position.y), 2), 2)));

  result_2.resVec.x = (float) (sin(result_2.radian) * result_2.e);
  result_2.resVec.y = (float) (cos(result_2.radian) * result_2.e);

  Vector2 res_vec = Vector2Add(result_1.resVec, result_2.resVec);
  cursor.e_res_vec = res_vec;

  cursor.vectors.push_back(Vector{Vector2{cursor.position.x, cursor.position.y},
                                  Vector2{result_2.resVec.x, result_2.resVec.y},
                                  BLUE});
  cursor_results.emplace_back(result_2);

  return cursor_results;
}

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

  double rel_pos_1 = (cursor.position.x - m_charge_list[0].position.x) / (m_charge_list[0].position.y - cursor.position.y);
  double radian_1 = atan(rel_pos_1);

  double e1_p = charge_1 / (4 * PI * m_constants.m_e);
  double e1 = e1_p * (1 / sqrt(pow(pow(cursor.position.x, 2) + pow((cursor.position.y - m_charge_list[0].position.y), 2), 2)));

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
