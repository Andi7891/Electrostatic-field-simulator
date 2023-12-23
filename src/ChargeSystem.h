#ifndef ESFS_SRC_CHARGESYSTEM_H_
#define ESFS_SRC_CHARGESYSTEM_H_

#include "raylib.h"
#include "raymath.h"

#include <vector>
#include <cassert>

#include <cstdio>

#ifndef RENDER_VECTOR
#define RENDER_VECTOR

#include <iostream>

struct Vector {
  Vector2 tail;
  Vector2 head;
  Color color;
};
#endif

struct Constants {
  double m_e0;
  float m_er;
  double m_e;

  Constants();
  Constants(double e0, float er);

  void update(double e0, double er);
  void compute_e();
  void compute_e(double e0, double er);
};

enum Scale {
  PICO, NANO, MICRO, MILI
};

struct Charge_particle {
  Vector2 position;
  float charge_unscaled;
  Scale scale;
  float radius;
  Color color;
  int index;

  static int global_index;

  Charge_particle(Vector2 position, float charge, Scale scale, float radius, Color color) : position{position},
                                                                                            charge_unscaled(charge),
                                                                                            radius{radius},
                                                                                            color{color},
                                                                                            scale{scale} {
    index = global_index;
    global_index++;
  }

  static int &getGlobalIndex() { return global_index; }
};

struct CursorResult {
  Color particle_color;
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

struct Cursor_Point {
  //Active is used when a cursor is inside a charge to disable the cursor
  bool active = true;
  Vector2 e_res_vec{};
  Color color{};
  float radius{};
  Vector2 position{};
  bool mainCursor{};
  int index{};
  bool contour {};
  std::vector<Vector> vectors;
  std::vector<CursorResult> cursor_result;

  static int global_index;

  Cursor_Point() = delete;
  Cursor_Point(Vector2 position, float radius, Color color, bool mainCursor) : position{position},
                                                                               radius{radius},
                                                                               color{color},
                                                                               mainCursor{mainCursor},
                                                                               e_res_vec{0, 0} {
    index = global_index;
    global_index++;
  }
  Cursor_Point(Vector2 position, float radius, Color color, bool contour, bool mainCursor) : position{position},
                                                                               radius{radius},
                                                                               color{color},
                                                                               mainCursor{mainCursor},
                                                                               e_res_vec{0, 0} {
    index = global_index;
    global_index++;
  }

  static int &getGlobalIndex() { return global_index; }
};

#ifndef FEATURES_STRUCT
#define FEATURES_STRUCT
struct Features {
  bool auto_zoom;
  bool showHelpPanel;
  bool showMoreHelpPanel;
  bool showResultPanel;
  bool showVectorGrid;
  bool showControlPanel;
  bool onlyResultantVector;
  Features() = delete;
  Features(bool auto_zoom, bool showHelpPanel, bool showMoreHelpPanel, bool showControlPanel, bool showResultPanel, bool showVectorGrid, bool onlyResultantVector)
      : auto_zoom{auto_zoom},
        showHelpPanel{showHelpPanel},
        showMoreHelpPanel{showMoreHelpPanel},
        showResultPanel{showResultPanel},
        showVectorGrid{showVectorGrid},
        showControlPanel{showControlPanel},
        onlyResultantVector{onlyResultantVector} {}
};
#endif

class ChargeSystem {
 public:

  ChargeSystem(double e0, Scale scale, double er);
  ~ChargeSystem() = default;

  //Charge api
  void addCharge(Vector2 position, double charge, Scale scale, float radius, Color color);
  std::vector<Charge_particle> &getChargesList() { return m_charge_list; }
  void removeCharge(int index);
  void resetCharges();

  //Cursor api
  void addCursor(Vector2 position, float radius, Color color, bool mainCursor);
  void addCursor(Vector2 position, float radius, Color color, bool noContour, bool mainCursor);
  std::vector<Cursor_Point> &getCursorList() { return m_cursor_list; }
  Cursor_Point &getMainCursor();
  void setMainCursor(int index);
  void removeCursor(int index);
  void resetCursors();

  //Environment
  void setConstants(double e0, Scale scale, double er);
  Constants &getConstants() { return m_constants; }

  //Calculate electrical field
  void compute_e(Features *features);

  bool isChargeSystemValid();

 private:
  std::vector<Charge_particle> m_charge_list;
  std::vector<Cursor_Point> m_cursor_list;
  Constants m_constants;
};

#endif //ESFS_SRC_CHARGESYSTEM_H_
