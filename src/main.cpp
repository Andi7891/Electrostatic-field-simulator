#include "rlImGui.h"
#include "imgui.h"

#include "GridSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "ChargeSystem.h"
#include "imgui_internal.h"

void Init(float &currentScale) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(1, 1, "ESFS");
  int monitor_index = GetCurrentMonitor();
  int monitor_width = GetMonitorWidth(monitor_index);
  int monitor_height = GetMonitorHeight(monitor_index);
  SetWindowSize((int) ((float) monitor_width / 1.5f), (int) ((float) monitor_height / 1.5f));
  SetWindowPosition((monitor_width / 2) - GetScreenWidth() / 2, (monitor_height / 2) - GetScreenHeight() / 2);
  MaximizeWindow();
  SetTargetFPS(60);
  currentScale = rlImGuiSetupS(1.f);
}

void show_debug_panel(InputSystem &input_system, Features &features, float &cameraScalar) {
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  center = ImVec2{center.x + center.x / 2, center.y - center.y / 2};
  ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));

  ImGui::Begin("DEBUG", nullptr, windowFlags);
  ImGui::TextColored(ImVec4(0, 255, 0, 255), "WINDOW SPECIFICATIONS");
  ImGui::Text("Absolut window size %d | %d", GetScreenWidth(), GetScreenHeight());
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);
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
  ImGui::SliderFloat("##Auto zoom scalar slider", &cameraScalar, 1.0f, 10.f);

  //4th row
  ImGui::Separator();
  ImGui::Text("Mouse raw position: %d %d", GetMouseX(), GetMouseY());
  ImGui::Text("Mouse scaled position: %.2f %.2f", input_system.getRefMousePos().x, input_system.getRefMousePos().y);

  ImGui::End();
}

void show_cursor_result_panel(Cursor_Point &cursor) {
  auto windowFlagsInfo =
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar
          | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
  ImGui::Begin("INFO", nullptr, windowFlagsInfo);
  ImGui::Text("Cursor position X: %.2f Y: %.2f", cursor.position.x, cursor.position.y);
  ImGui::Text("Ef_res_vec cursor X: %.2f Y: %.2f", cursor.e_res_vec.x, cursor.e_res_vec.y);
  ImGui::End();
}

void show_control_panel(ChargeSystem &charge_system, Cursor_Point &cursor) {
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_AlwaysAutoResize;

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  center = ImVec2{center.x + center.x / 2, center.y + center.y / 2};
  ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));

  ImGui::Begin("Control Panel", nullptr, windowFlags);

  float cursor_pos[2] = {cursor.position.x, cursor.position.y};
  ImGui::Text("Main cursor position");
  ImGui::SliderFloat2("##Main cursor pos", cursor_pos, -100.f, 100.f);
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

  for (auto &charge : charge_system.getChargesList()) {
    auto particle_color = ImColor(charge.color.r, charge.color.g, charge.color.b);
    ImGui::TextColored(particle_color, "PARTICLE NO %d", index);

    //Position
    float temp_val[2] = {charge.position.x, charge.position.y};
    ImGui::Text("Particle position");
    ImGui::SliderFloat2(TextFormat("##Pos %d", index), temp_val, -100.f, 100.f);
    charge.position.x = temp_val[0];
    charge.position.y = temp_val[1];

    //Scale
    if (ImGui::BeginCombo(TextFormat("Charge S.U. %d", index),
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

    ImGui::Text("Particle charge");
    ImGui::SliderFloat(TextFormat("##Charge %d", index), &charge.charge_unscaled, -100.f, 100.f);

    //Radius
    ImGui::Text("Particle radius(m)");
    ImGui::SliderFloat(TextFormat("##Radius %d", index), &charge.radius, 0.f, 100.f);

    auto color = ImColor(charge.color.r, charge.color.g, charge.color.b);
    float temp_color[3] = {color.Value.x, color.Value.y, color.Value.z};

    ImGui::Text("Particle color");
    ImGui::ColorEdit3(TextFormat("##Color %d", index), temp_color);

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

void show_help_panel(Features &features) {
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_AlwaysAutoResize;
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.f);
  ImGui::Begin("Help", nullptr, windowFlags);
  ImGui::PopStyleVar();

  ImGui::BeginColumns("Cols", 2, ImGuiOldColumnFlags_GrowParentContentsSize);

  ImGui::Text("F1 for the main cursor short results panel");
  ImGui::Text("F2 for control panel");
  ImGui::Text("F3 for vector grid (Incomplete)");
  ImGui::Text("F10 for debug panel");
  ImGui::Text("R to toggle the influences on the main cursor panel");
  ImGui::Text("H to toggle the help panel");

  ImGui::NextColumn();

  ImGui::Text("CTRL/STRG + MOUSE_WHEEL for manual zoom");
  ImGui::Text("Hold MOUSE LEFT BUTTON for dragging charges/cursors");

  ImGui::Separator();
  ImGui::Text("Press Q for adding charges");
  ImGui::Text("SHIFT + Q for removing charges");

  ImGui::Separator();
  ImGui::Text("S to set the main cursor");
  ImGui::Text("SPACE to toggle main cursor moving");
  ImGui::Text("Press C for adding cursor");
  ImGui::Text("SHIFT + C for removing cursor");

  ImGui::Separator();
  ImGui::Text("ESC for closing the program");

  ImGui::EndColumns();

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
  features.showHelpPanel = !ImGui::Button("Close");
  ImGui::SameLine();
  features.showMoreHelpPanel = ImGui::Button("More information");
  if (features.showMoreHelpPanel)
    features.showHelpPanel = !features.showHelpPanel;

  ImGui::PopStyleVar();

  ImGui::End();
}

void show_mouse_position_panel(ImFont *font_Drag, InputSystem &input_system, float camera_scalar, Camera2D &camera) {
  ImVec2 windowPos = ImVec2{GetMousePosition().x, GetMousePosition().y - (camera_scalar * camera.zoom * 1.1f)};
  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2{0.5f, 1.f});
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground
      | ImGuiWindowFlags_NoDecoration;

  ImGui::PushFont(font_Drag);
  ImGui::Begin("Cursor_position", nullptr, windowFlags);
  ImVec4 textColor = {0, 0, 0, 255}; //Black
  ImGui::TextColored(textColor, "%.2f %.2f", input_system.getRefMousePos().x, input_system.getRefMousePos().y);
  ImGui::End();
  ImGui::PopFont();
}

void show_results_panel(Features &features, ChargeSystem &charge_system) {
  if (features.showResultPanel) {
    if (charge_system.isChargeSystemValid()) {
      if (!charge_system.getMainCursor().cursor_result.empty()) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        center = ImVec2{center.x / 2.5f, center.y};
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.f);
        ImGui::Begin("Influences on the main cursor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::PopStyleVar();

        ImGui::Text("Main cursor pos: %.3f %.3f",
                    charge_system.getMainCursor().position.x,
                    charge_system.getMainCursor().position.y);

        ImGui::Separator();
        ImGui::Text("Charge influences on the main cursor");
        int index = 1;
        for (const auto &charge_result : charge_system.getMainCursor().cursor_result) {
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

        ImGui::Text("Main cursor");
        ImGui::Text(TextFormat("Resultant vector: (%f %f)",
                               charge_system.getMainCursor().e_res_vec.x,
                               charge_system.getMainCursor().e_res_vec.y));
        ImGui::Text(TextFormat("Magnitude resultant vector: %f",
                               Vector2Length(charge_system.getMainCursor().e_res_vec)));

        ImGui::End();
      }
    }
  }
}

void show_more_help_panel(Features &features) {
  auto windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_AlwaysAutoResize;
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.f);
  ImGui::Begin("More information", nullptr, windowFlags);
  ImGui::PopStyleVar();

  ImGui::BulletText("Data panels can be moved by holding mouse left button");
  ImGui::BulletText("Data panels can be docked by holding shift/strg while holding mouse left button");
  ImGui::BulletText("CTRL/STRG + mouse left button on any slider in the control panel for precise input");
  ImGui::BulletText("The colour of charges can be changed by clicking on the colour preview");
  ImGui::BulletText(
      "The simulator needs a main cursor, otherwise the simulation is disabled and all related windows are closed, until a cursor is present");

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
  features.showMoreHelpPanel = !ImGui::Button("Close");
  ImGui::PopStyleVar();

  ImGui::End();
}

////////////////////////////////////////////////////ENTRY_POINT///////////////////////////////////////////////
int main() {
  float currentScale = 0.f;
  Init(currentScale);
  float newScale;

  ImFont *font_Drag = nullptr;

  float GLOBAL_SCALE = 10.f;
  InputSystem input_system(GLOBAL_SCALE);
  RenderSystem render_system(GLOBAL_SCALE, GLOBAL_SCALE * 8.f);
  GridSystem grid_system(50, (int) GLOBAL_SCALE * 5, BLACK, BLACK, BLACK);
  ChargeSystem charge_system;

  charge_system.setConstants(8.854817178, PICO, 1);

  Features features = {false, true, false, true, false};
  float camera_scalar = 5.f;

  Camera2D camera = {0};
  camera.offset = {(float) GetScreenWidth() / 2.0f, (float) GetScreenHeight() / 2.0f};
  camera.target = {0.0f, 0.0f};
  camera.rotation = 0.0f;
  camera.zoom = ImClamp(((float) GetScreenHeight() / (float) GetScreenWidth()) * camera_scalar, 1.0f, 10.f);

  //ImGui terminology
  //https://github.com/ocornut/imgui/wiki/Glossary

  //Main loop
  while (!WindowShouldClose()) {
    RenderSystem::newFrame();

    //Font auto scaling
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

    //ImGui rendering
    rlImGuiBegin();

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Main", nullptr, windowFlags);
    ImGui::PopStyleVar(3);
    ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    //Camera
    //Auto zoom feature
    camera.offset = {(float) GetScreenWidth() / 2.0f, (float) GetScreenHeight() / 2.0f};

    if (features.auto_zoom) {
      camera.zoom = ImClamp(((float) GetScreenHeight() / (float) GetScreenWidth()) * camera_scalar, 1.0f, 10.f);
    } else {
      if (IsKeyDown(KEY_LEFT_CONTROL))
        camera.zoom += ((float) GetMouseWheelMove() * 0.125f);
    }

    //Camera zoom clamping
    if (camera.zoom > 10.0f) camera.zoom = 10.0f;
    else if (camera.zoom < 0.5f) camera.zoom = 0.5f;

    //Update the input system
    input_system.update_input(camera.zoom);

    //Toggling the main cursor dragging
    if (!ImGui::GetIO().WantCaptureMouse) {
      if (input_system.KeysStatus.space)
        if (charge_system.isChargeSystemValid())
          charge_system.getMainCursor().position = input_system.getRefMousePos();
    }

    //Toggling value window
    if (input_system.KeysStatus.R) {
      features.showResultPanel = !features.showResultPanel;
      input_system.KeysStatus.R = false;
    }

    //Toggling help window
    if (input_system.KeysStatus.H) {
      features.showHelpPanel = !features.showHelpPanel;
      input_system.KeysStatus.H = false;
    }

    //Toggling cursor info window
    if (input_system.KeysStatus.F1)
      if (charge_system.isChargeSystemValid())
        show_cursor_result_panel(charge_system.getMainCursor());

    //Toggling control panel window
    if (input_system.KeysStatus.F2)
      if (charge_system.isChargeSystemValid())
        show_control_panel(charge_system, charge_system.getMainCursor());

    //Toggling the vector grid window
    if (input_system.KeysStatus.F3) {
      features.showVectorGrid = !features.showVectorGrid;
      input_system.KeysStatus.F3 = false;
    }

    //Toggling the debugging window
    if (input_system.KeysStatus.debug)
      show_debug_panel(input_system, features, camera_scalar);

    //Toggling the help window
    if (features.showHelpPanel) {
      show_help_panel(features);
      if (input_system.KeysStatus.F2)
        input_system.KeysStatus.F2 = !input_system.KeysStatus.F2;
    }

    //Toggling the more help window
    if (features.showMoreHelpPanel)
      show_more_help_panel(features);

    //Toggling the result window
    if (features.showResultPanel && !features.showHelpPanel && !features.showMoreHelpPanel)
      show_results_panel(features, charge_system);

    //Render ray-lib
    BeginMode2D(camera);
    //Render grid
    grid_system.render();

    //Disable the cursor if it is inside a charge
    for (auto &cursor : charge_system.getCursorList()) {
      for (auto &charge : charge_system.getChargesList())
        if (CheckCollisionCircles(cursor.position,
                                  (cursor.radius + 0.25f * cursor.radius) * GLOBAL_SCALE,
                                  charge.position,
                                  charge.radius * GLOBAL_SCALE)) {
          cursor.active = false;
        } else {
          cursor.active = true;
        }
    }

    //Starts the rendering if the help panel is closed
    if (!features.showHelpPanel) {
      charge_system.compute_e(&features);
      render_system.update(charge_system, features);
      render_system.render(charge_system);
    }
    EndMode2D();

    //Check for inputs when the help panel is closed only
    if (!features.showHelpPanel && !features.showMoreHelpPanel) {

      ///////////////////////////////////////////////////Charges/////////////////////////////////////////////

      //Common variables for add/remove charge feature
      static int charges_color_index = 0;
      static std::vector<Color>
          charge_color_list = {Color(RED), Color(BLUE), Color(ORANGE), Color(GRAY), Color(GREEN), Color(MAROON)};

      //If the color index is less than 0, clamp it to 0
      if (charges_color_index < 0)
        charges_color_index = 0;

      //Reset the color index when there are no more colors
      if (charges_color_index == charge_color_list.size())
        charges_color_index = 0;

      if (input_system.KeysStatus.debug) {
        ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Separator();
        ImGui::TextColored(ImVec4{0, 255, 255, 255}, "CHARGE SYSTEM");
        ImGui::End();
      }

      //Implementation for add charges feature
      if (!ImGui::GetIO().WantCaptureMouse)
        if (IsKeyPressed(KEY_Q) && !IsKeyDown(KEY_LEFT_SHIFT)) {
          charge_system.addCharge(input_system.getRefMousePos(), 10, NANO, 0.1, charge_color_list[charges_color_index]);
          charges_color_index++;
        }

      if (input_system.KeysStatus.debug) {
        ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Color index: %d", charges_color_index);
        ImGui::Text("Charge system global index: %d", Charge_particle::getGlobalIndex());
        ImGui::End();
      }

      //Implementation for remove charges feature
      if (!ImGui::GetIO().WantCaptureMouse)
        if (IsKeyPressed(KEY_Q) && IsKeyDown(KEY_LEFT_SHIFT)) {
          for (const auto &charge : charge_system.getChargesList()) {
            if (CheckCollisionPointCircle(input_system.getRefMousePos(),
                                          charge.position,
                                          charge.radius * GLOBAL_SCALE)) {
              charge_system.removeCharge(charge.index);
              charges_color_index--;
              break;
            }
          }
        }
      if (input_system.KeysStatus.debug) {
        ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Charges indexes");
        for (const auto &charge : charge_system.getChargesList()) {
          if (!(charge.index % 12)) {
            ImGui::NewLine();
          }
          ImGui::Text("%d ", charge.index);
          ImGui::SameLine();
        }
        ImGui::End();
      }

      ///////////////////////////////////////Charge dragging/////////////////////////////////////////

      //Implementation for charges dragging feature
      //Last_pressed_element solves the problem of fast moving cursor.
      //By "short-circuiting" the collision check.
      //If the mouse click is still pressed, then ignore if it is over the charge
      static size_t charge_last_pressed_element = -1;
      static bool charge_dragged = false;
      static bool cursor_dragged = false;

        if (!cursor_dragged) {
          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (charge_last_pressed_element != -1) {
              charge_system.getChargesList()[charge_last_pressed_element].position = input_system.getRefMousePos();
              show_mouse_position_panel(font_Drag, input_system, camera_scalar, camera);
            } else {
              for (auto &charge : charge_system.getChargesList()) {
                if (CheckCollisionPointCircle(input_system.getRefMousePos(),
                                              charge.position,
                                              charge.radius * GLOBAL_SCALE)) {
                  charge_dragged = true;

                  if (!ImGui::GetIO().WantCaptureMouse)
                    charge.position = input_system.getRefMousePos();

                  show_mouse_position_panel(font_Drag, input_system, camera_scalar, camera);

                  //Find the pressed element index in the vector
                  for (size_t index = 0; index < charge_system.getChargesList().size(); index++)
                    if (&charge == &charge_system.getChargesList()[index])
                      charge_last_pressed_element = index;

                  break;
                }
              }
            }

          } else {
            charge_last_pressed_element = -1;
            charge_dragged = false;
          }
        }


      //////////////////////////////////////////Cursor///////////////////////////////////////////

      static int cursor_color_index = 0;

      static std::vector<Color>
          cursor_color_list = {Color(RED), Color(BLUE), Color(ORANGE), Color(GRAY), Color(GREEN), Color(MAROON)};

      if (!features.showVectorGrid) {
        //If the color index is less than 0, clamp it to 0
        if (cursor_color_index < 0)
          cursor_color_index = 0;

        //Reset the color index when there are no more colors
        if (cursor_color_index == cursor_color_list.size())
          cursor_color_index = 0;
      }
      if (input_system.KeysStatus.debug) {
        ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::TextColored(ImVec4{255, 255, 0, 255}, "CURSOR SYSTEM");
        ImGui::End();
      }

      if (!features.showVectorGrid) {
        //Implementation for add cursor feature
        if (!ImGui::GetIO().WantCaptureMouse)
          if (IsKeyPressed(KEY_C) && !IsKeyDown(KEY_LEFT_SHIFT)) {
            charge_system.addCursor(input_system.getRefMousePos(), 0.05f, cursor_color_list[cursor_color_index], false);
            cursor_color_index++;
          }
      }

      if (input_system.KeysStatus.debug) {
        ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Color index: %d", cursor_color_index);
        ImGui::Text("Cursor system global index: %d", Cursor_Point::getGlobalIndex());
        ImGui::End();
      }

      if (!features.showVectorGrid) {
        //Implementation for remove cursor feature
        if (!ImGui::GetIO().WantCaptureMouse)
          if (IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_SHIFT)) {
            for (const auto &cursor : charge_system.getCursorList()) {
              if (CheckCollisionPointCircle(input_system.getRefMousePos(),
                                            cursor.position,
                                            cursor.radius * GLOBAL_SCALE)) {
                charge_system.removeCursor(cursor.index);
                cursor_color_index--;
                break;
              }
            }
          }
      }
      if (input_system.KeysStatus.debug) {
        ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Cursor indexes");
        for (const auto &cursor : charge_system.getCursorList()) {
          if (!(cursor.index % 12)) {
            ImGui::NewLine();
          }
          ImGui::Text("%d ", cursor.index);
          ImGui::SameLine();
        }
        ImGui::End();
      }

      //////////////////////////////////Cursor dragging/////////////////////////////////////////

      if (!features.showVectorGrid) {
        //Implementation for charges dragging feature
        //Last_pressed_element solves the problem of fast moving cursor.
        //By "short-circuiting" the collision check.
        //If the mouse click is still pressed, then ignore if it is over the charge
        static size_t cursor_last_pressed_element = -1;

          if (!charge_dragged) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
              if (cursor_last_pressed_element != -1) {
                charge_system.getCursorList()[cursor_last_pressed_element].position = input_system.getRefMousePos();
                show_mouse_position_panel(font_Drag, input_system, camera_scalar, camera);
              } else {
                for (auto &cursor : charge_system.getCursorList()) {
                  if (CheckCollisionPointCircle(input_system.getRefMousePos(),
                                                cursor.position,
                                                cursor.radius * GLOBAL_SCALE)) {
                    cursor_dragged = true;

                    if (!ImGui::GetIO().WantCaptureMouse)
                      cursor.position = input_system.getRefMousePos();

                    show_mouse_position_panel(font_Drag, input_system, camera_scalar, camera);

                    //Find the pressed element index in the vector
                    for (size_t index = 0; index < charge_system.getCursorList().size(); index++)
                      if (&cursor == &charge_system.getCursorList()[index])
                        cursor_last_pressed_element = index;

                    break;
                  }
                }
              }
            } else {
              cursor_last_pressed_element = -1;
              cursor_dragged = false;
            }
          }
        }


      ////////////////////////////////////Set main cursor/////////////////////
      if (charge_system.isChargeSystemValid()) {
        Color color = {};
        int indexCursor = -1;
        if (!ImGui::GetIO().WantCaptureMouse)
          if (IsKeyPressed(KEY_S)) {
            for (auto &cursor : charge_system.getCursorList()) {
              if (CheckCollisionPointCircle(input_system.getRefMousePos(),
                                            cursor.position,
                                            cursor.radius * GLOBAL_SCALE)) {
                indexCursor = cursor.index;
                color = cursor.color;
              }
            }
            if (indexCursor != -1) {
              charge_system.getMainCursor().color = color;
              charge_system.setMainCursor(indexCursor);
            }
          }
      }
    }
    if (!charge_system.isChargeSystemValid() && input_system.KeysStatus.debug) {
      ImGui::Begin("DEBUG");
      //ImGui::NewLine();
      ImGui::TextColored(ImVec4(255, 0, 0, 255), "CHARGE SYSTEM IS INVALID");
      ImGui::TextColored(ImVec4(255, 0, 0, 255), "MISSING A MAIN CURSOR");
      ImGui::End();
    }

    rlImGuiEnd();
    RenderSystem::endFrame();
  }

  //Unload resources, close all handles, exit the program
  rlImGuiShutdown();
  CloseWindow();

  return 0;
}