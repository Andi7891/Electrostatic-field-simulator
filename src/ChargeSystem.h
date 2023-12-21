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

  void update(double e0, double er) {
    m_e0 = e0;
    m_er = (float) er;
  }

  void compute_e() {
    m_e = m_e0 * m_er;
  }

  void compute_e(double e0, double er) {
    m_e0 = e0;
    m_er = (float) er;
    m_e = m_e0 * m_er;
  }

  Constants() = delete;
  Constants(double e0, float er) : m_e0{e0}, m_er{er} { m_e = m_e0 * m_er; }
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
  Vector2 e_res_vec{};
  Color color{};
  float radius{};
  Vector2 position{};
  bool mainCursor{};
  int index{};
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
  Features() = delete;
  Features(bool auto_zoom, bool showHelpPanel, bool showMoreHelpPanel, bool showResultPanel, bool showVectorGrid)
      : auto_zoom{auto_zoom},
        showHelpPanel{showHelpPanel},
        showMoreHelpPanel{showMoreHelpPanel},
        showResultPanel{showResultPanel},
        showVectorGrid{showVectorGrid} {}
};
#endif

class ChargeSystem {
 public:

  void addCharge(Vector2 position, double charge, Scale scale, float radius, Color color);
  void addCursor(Vector2 position, float radius, Color color, bool mainCursor);

  void removeCharge(int index);
  void removeCursor(int index);

  void setConstants(double e0, Scale scale, double er);
  Constants &getConstants() { return m_constants; }
  void compute_e(Features *features);

  ChargeSystem();
  ~ChargeSystem() = default;

  //In the case that no cursor is set as the main cursor, then return the first one.
  //In the case of multiple cursors set as the main cursor, the function will return the first one
  Cursor_Point &getMainCursor();
  void setMainCursor(int index);

  std::vector<Charge_particle> &getChargesList() { return m_charge_list; }
  std::vector<Cursor_Point> &getCursorList() { return m_cursor_list; }
 private:
  std::vector<Charge_particle> m_charge_list;
  std::vector<Cursor_Point> m_cursor_list;
  Constants m_constants;
};

#endif //ESFS_SRC_CHARGESYSTEM_H_
