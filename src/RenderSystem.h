#ifndef ESFS_SRC_RENDERSYSTEM_H_
#define ESFS_SRC_RENDERSYSTEM_H_

#include "raylib.h"
#include "raymath.h"

#include <vector>

#ifndef INPUTSYSTEM
#define INPUTSYSTEM
#include "InputSystem.h"
#endif

#ifndef CHARGESYSTEM
#define CHARGESYSTEM
#include "ChargeSystem.h"
#endif


#ifndef RENDER_VECTOR
#define RENDER_VECTOR
struct Vector {
  Vector2 tail;
  Vector2 head;
  Color color;
};
#endif

#ifndef FEATURES_STRUCT
#define FEATURES_STRUCT
struct Features {
  bool auto_zoom = false;
  bool showHelpPanel = true;
  bool showResultPanel = true;
};
#endif

class RenderSystem {
 public:

  void update(KeysStatus &keys_status, ChargeSystem &charge_system, Features& features);
  void render(ChargeSystem& charge_system) const;

  static void newFrame();
  static void endFrame();

  ~RenderSystem() = default;
  RenderSystem() = delete;
  explicit RenderSystem(float scaleFactor, float radius_scale);

 private:
  float m_scale_factor;
  float m_radius_scale;
  std::vector<Vector> m_vector_render_list;
};

#endif //ESFS_SRC_RENDERSYSTEM_H_
