#include "rlImGui.h"
#include "imgui.h"

#include "GridSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "ChargeSystem.h"
#include "imgui_internal.h"

#include <string>

void Init(float &currentScale) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(1, 1, "Electrostatics simulator");
  int monitor_index = GetCurrentMonitor();
  int monitor_width = GetMonitorWidth(monitor_index);
  int monitor_height = GetMonitorHeight(monitor_index);
  SetWindowSize((int) ((float) monitor_width / 1.5f), (int) ((float) monitor_height / 1.5f));
  SetWindowPosition((monitor_width / 2) - GetScreenWidth() / 2, (monitor_height / 2) - GetScreenHeight() / 2);
  SetTargetFPS(60);
  currentScale = rlImGuiSetupS(1.f);
}

#ifndef FEATURES_STRUCT
#define FEATURES_STRUCT
struct Features {
  bool auto_zoom = false;
  bool showHelpPanel = true;
  bool showResultPanel = true;
};
#endif

void show_debug_panel(InputSystem &input_system, Features &features, float &cameraScalar) {
  //ImGuiWindowFlags
  //  windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
  //ImGui::SetNextWindowBgAlpha(0.8);

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  center = ImVec2{center.x + center.x / 2, center.y - center.y / 2};
  ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));

  ImGui::Begin("DEBUG", nullptr, windowFlags);
  ImGui::TextColored(ImVec4(0, 255, 0, 255), "WINDOW SPECIFICATIONS");
  ImGui::Text("Absolut window size %d | %d", GetScreenWidth(), GetScreenHeight());
  ImGui::Text("FPS: %d", GetFPS());
  ImGui::Text("Current window DPI: %.3f %.3f", GetWindowScaleDPI().x, GetWindowScaleDPI().y);

  ImGui::Separator();

  ImGui::TextColored(ImVec4(255, 0, 0, 255), "INPUT SPECIFICATIONS");
  ImGui::Checkbox("F1", &input_system.KeysStatus.F1);
  ImGui::SameLine();
  ImGui::Checkbox("F2", &input_system.KeysStatus.F2);
  ImGui::SameLine();
  ImGui::Checkbox("F3", &input_system.KeysStatus.F3);
  ImGui::SameLine();
  ImGui::Checkbox("F4", &input_system.KeysStatus.F4);
  ImGui::SameLine();
  ImGui::Checkbox("F5", &input_system.KeysStatus.F5);

  ImGui::Checkbox("Auto zoom", &features.auto_zoom);

  ImGui::Checkbox("Show help panel", &features.showHelpPanel);

  ImGui::Checkbox("Show result panel", &features.showResultPanel);

  //3rd Row
  ImGui::Text("Auto zoom scalar");
  ImGui::SliderFloat(".", &cameraScalar, 1.0f, 10.f);

  //4th row
  ImGui::Separator();
  ImGui::Text("Mouse raw position: %d %d", GetMouseX(), GetMouseY());
  ImGui::Text("Mouse scaled position: %.2f %.2f", input_system.getRefMousePos().x, input_system.getRefMousePos().y);

  ImGui::End();
}

void log_panel(Cursor_Point &cursor) {
  auto windowFlagsInfo =
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar
          | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
  //ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
  ImGui::Begin("INFO", nullptr, windowFlagsInfo);
  ImGui::Text("Cursor position X: %.2f Y: %.2f", cursor.position.x, cursor.position.y);
  ImGui::Text("Ef_res_vec cursor X: %.2f Y: %.2f", cursor.e_res_vec.x, cursor.e_res_vec.y);
  ImGui::End();
}

void control_panel(ChargeSystem &charge_system, Cursor_Point &cursor) {
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  center = ImVec2{center.x + center.x / 2, center.y + center.y / 2};
  ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));

  ImGui::Begin("Control Panel", nullptr, windowFlags);
  ImGui::Text("CTRL/STRG + mouse button left on the slider for manual input");

  float cursor_pos[2] = {cursor.position.x, cursor.position.y};
  ImGui::SliderFloat2("Cursor position", cursor_pos, -100.f, 100.f);
  cursor.position.x = cursor_pos[0];
  cursor.position.y = cursor_pos[1];

  ImGui::Separator();

  ImGui::Text("Constants");
  auto &constants = charge_system.getConstants();
  charge_system.getConstants().compute_e();
  ImGui::InputFloat("Er", &constants.m_er, -1000.f, 1000.f);

  ImGui::Separator();

  int index = 1;
  const char *items[] = {"Pico", "Nano", "Micro", "Mili"};

  std::vector<const char *> current_item = {};
  std::vector<int> current_scale = {};
  for (const auto &charge : charge_system.getChargesList()) {
    const char *temp_scale = items[charge.scale];
    current_item.push_back(temp_scale);
    current_scale.push_back(charge.scale);
  }

  //static const char *current_item[2] = {items[charge_system.getChargesList()[0].scale],
  //                                    items[charge_system.getChargesList()[1].scale]};

  //static int current_scale[2] = {charge_system.getChargesList()[0].scale,
  //                             charge_system.getChargesList()[1].scale};

  for (auto &charge : charge_system.getChargesList()) {

    //Position
    float temp_val[2] = {charge.position.x, charge.position.y};
    ImGui::SliderFloat2(TextFormat("Particle no %d position", index), temp_val, -100.f, 100.f);
    charge.position.x = temp_val[0];
    charge.position.y = temp_val[1];

    //Scale
    if (ImGui::BeginCombo(TextFormat("Charge %d scale unit", index),
                          current_item[index - 1],
                          ImGuiComboFlags_WidthFitPreview)) {
      for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
        bool is_selected = (current_item[index - 1] == items[n]);
        if (ImGui::Selectable(items[n], is_selected)) {
          current_item[index - 1] = items[n];
          current_scale[index - 1] = n;
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    //Charge
    charge.scale = Scale(current_scale[index - 1]);

    ImGui::SliderFloat(TextFormat("Particle no %d charge (%s)", index, current_item[index - 1]),
                       &charge.charge_unscaled,
                       -100.f,
                       100.f);

    //Radius
    ImGui::SliderFloat(TextFormat("Particle no %d radius (m)", index), &charge.radius, 0.f, 100.f);

    auto color = ImColor(charge.color.r, charge.color.g, charge.color.b);
    float temp_color[3] = {color.Value.x, color.Value.y, color.Value.z};

    ImGui::Text("Press on the colour preview for the colour selector");
    ImGui::ColorEdit3(TextFormat("Particle no %d color", index), temp_color);

    charge.color = Color{
        (unsigned char) (temp_color[0] >= 1.0 ? 255 : (temp_color[0] <= 0.0 ? 0 : (int) floor(temp_color[0] * 256.0))),
        (unsigned char) (temp_color[1] >= 1.0 ? 255 : (temp_color[1] <= 0.0 ? 0 : (int) floor(temp_color[1] * 256.0))),
        (unsigned char) (temp_color[2] >= 1.0 ? 255 : (temp_color[2] <= 0.0 ? 0 : (int) floor(temp_color[2] * 256.0))),
        255};

    ImGui::Separator();
    index++;
  }
  ImGui::End();
}

void help_panel(bool &showHelpPanel) {
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_AlwaysAutoResize;
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.f);
  ImGui::Begin("Help", nullptr, windowFlags);
  ImGui::PopStyleVar();

  ImGui::Text("F1 for results");
  ImGui::Text("F2 for control panel");
  ImGui::Text("F3 for vector grid (Incomplete)");
  ImGui::Text("F10 for debug panel");

  ImGui::Separator();

  ImGui::Text("R to toggle the result panel");
  ImGui::Text("H to toggle the help panel");
  ImGui::Text("SPACE to toggle cursor moving");
  ImGui::Separator();
  ImGui::Text("MOUSE_WHEEL for manual zoom");
  ImGui::Text("MOUSE_LEFT_HOLD for dragging charges");
  ImGui::Text("MOUSE_RIGHT/C for adding charges");
  ImGui::Text("SHIFT + MOUSE_LEFT/C for removing the charge");
  ImGui::Separator();
  ImGui::Text("ESC for exit");

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
  showHelpPanel = !ImGui::Button("Close");
  ImGui::PopStyleVar();

  ImGui::End();
}

////////////////////////////////////////////////////ENTRY_POINT///////////////////////////////////////////////
int main(int argc, char *argv[]) {
  float currentScale = 0.f;
  Init(currentScale);
  float newScale = currentScale;

  ImFont *font_Drag = nullptr;
  std::vector<Color> culor_list_order = {Color(RED), Color(BLUE), Color(ORANGE), Color(GRAY)};

  float GLOBAL_SCALE = 10.f;
  InputSystem input_system(GLOBAL_SCALE);
  RenderSystem render_system(GLOBAL_SCALE, GLOBAL_SCALE * 8.f);
  GridSystem grid_system(50, (int) GLOBAL_SCALE * 5, BLACK, BLACK, BLACK);
  ChargeSystem charge_system;

  charge_system.setConstants(8.854817178, PICO, 1);

  //charge_system.addCharge(Vector2{0, 10}, -30, NANO, 0.1, BLUE);
  //charge_system.addCharge(Vector2{0, -10}, -30, NANO, 0.1, GRAY);
  //charge_system.addCharge(Vector2{10, 0}, 30, NANO, 0.1, RED);
  //charge_system.addCharge(Vector2{-10, 0}, 30, NANO, 0.1, ORANGE);

  charge_system.addCursor(Vector2{0, 0}, 0.05f, YELLOW, true);
  //charge_system.addCursor(Vector2{5,0}, 0.05f, RED, false);

  Features features = {};
  float camera_scalar = 5.f;

  Camera2D camera = {0};
  camera.offset = {(float) GetScreenWidth() / 2.0f, (float) GetScreenHeight() / 2.0f};
  camera.target = {0.0f, 0.0f};
  camera.rotation = 0.0f;
  camera.zoom = ImClamp(((float) GetScreenHeight() / (float) GetScreenWidth()) * camera_scalar, 1.0f, 10.f);

  //Main loop
  while (!WindowShouldClose()) {
    RenderSystem::newFrame();
    //input
    if (features.auto_zoom) {
      camera.zoom = ImClamp(((float) GetScreenHeight() / (float) GetScreenWidth()) * camera_scalar, 1.0f, 10.f);
    } else {
      camera.zoom += ((float) GetMouseWheelMove() * 0.125f);
    }

    if (camera.zoom > 10.0f) camera.zoom = 10.0f;
    else if (camera.zoom < 0.5f) camera.zoom = 0.5f;

    input_system.update_input(camera.zoom);
    camera.offset = {(float) GetScreenWidth() / 2.0f, (float) GetScreenHeight() / 2.0f};

    if (input_system.KeysStatus.space)
      charge_system.getMainCursor().position = input_system.getRefMousePos();

    if (input_system.KeysStatus.R) {
      features.showResultPanel = !features.showResultPanel;
      input_system.KeysStatus.R = false;
    }
    if (input_system.KeysStatus.H) {
      features.showHelpPanel = !features.showHelpPanel;
      input_system.KeysStatus.H = false;
    }

    //Render ray-lib
    BeginMode2D(camera);
    grid_system.render();

//TODO imgui before rendering system?

    rlImGuiSetContext();
    newScale = ((float) GetScreenHeight() / (float) GetScreenWidth()) * 4.0f;
    if (newScale != currentScale) {
      UI_SCALING(newScale);
      currentScale = newScale;
      ImGuiIO &io = ImGui::GetIO();
      io.Fonts->Clear();
      ImFont *font = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", newScale * 12);
      font_Drag = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", newScale * 14);
      if (font_Drag == nullptr)
        font_Drag = ImGui::GetDefaultFont();
      if (font == nullptr)
        io.Fonts->AddFontDefault();

      rlImGuiReloadFonts();
    }

    rlImGuiBegin();
    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    if (!features.showHelpPanel) {
      render_system.update(input_system.KeysStatus, charge_system, charge_system.getMainCursor(), features);
      render_system.render(charge_system);
    }
    EndMode2D();

    if (input_system.KeysStatus.F1)
      log_panel(charge_system.getMainCursor());

    if (input_system.KeysStatus.F2)
      control_panel(charge_system, charge_system.getMainCursor());

    if (input_system.KeysStatus.F3) {
      features.showVectorGrid = !features.showVectorGrid;
      input_system.KeysStatus.F3 = false;
    }

    if (input_system.KeysStatus.debug)
      show_debug_panel(input_system, features, camera_scalar);

    if (features.showHelpPanel)
      help_panel(features.showHelpPanel);

    static int color_index = 0;
    //Adding charges
    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || (IsKeyPressed(KEY_C) && !IsKeyDown(KEY_LEFT_SHIFT))) {
      //Change color for each element
      if (color_index == culor_list_order.size())
        color_index = 0;
      charge_system.addCharge(input_system.getRefMousePos(), 10, NANO, 0.1, culor_list_order[color_index]);
      color_index++;
    }

    //Remove charges
    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || (IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_SHIFT))) {
      for(const auto& charge : charge_system.getChargesList()) {
        if (CheckCollisionPointCircle(input_system.getRefMousePos(), charge.position, charge.radius * GLOBAL_SCALE)) {
          //Color order decreased by one
          color_index--;
          if (color_index < 0)
            color_index = 0;
          charge_system.removeCharge(charge.index);
        }
      }
    }

    //Drag charges
    //TODO One of the messiest code ever, but it works perfectly (almost)
    // (also first time ran and worked)
    static size_t last_pressed_element = -1;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      for (auto &charge : charge_system.getChargesList()) {
        if (CheckCollisionPointCircle(input_system.getRefMousePos(), charge.position, charge.radius * GLOBAL_SCALE)
            || (last_pressed_element != -1 && &charge == &charge_system.getChargesList()[last_pressed_element])) {
          charge.position = input_system.getRefMousePos();

          ImVec2 windowPos = ImVec2{GetMousePosition().x, GetMousePosition().y - (camera_scalar * camera.zoom * 1.1f)};
          ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2{0.5f, 1.f});
          auto windowFlags =
              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse
                  | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground
                  | ImGuiWindowFlags_NoDecoration;

          ImGui::PushFont(font_Drag);
          ImGui::Begin("Cursor_position", nullptr, windowFlags);
          ImGui::TextColored(ImVec4{0, 0, 0, 255},
                             "%.2f %.2f",
                             input_system.getRefMousePos().x,
                             input_system.getRefMousePos().y);
          ImGui::End();
          ImGui::PopFont();

          //TODO high performance hit, but takes time to refactor this function
          //Find the pressed element
          for (size_t index = 0; index < charge_system.getChargesList().size(); index++) {
            if (&charge == &charge_system.getChargesList()[index]) {
              last_pressed_element = index;
            }
          }
          if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            last_pressed_element = -1;
          //It is used as a height layer, so the entities don't get stuck one in the other
          break;
        }
      }
    } else {
      last_pressed_element = -1;
    }

    rlImGuiEnd();

    RenderSystem::endFrame();
  }

  //Unload resources, close all handles, exit the program
  rlImGuiShutdown();
  CloseWindow();

  return 0;
}