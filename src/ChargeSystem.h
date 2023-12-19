#ifndef ESFS_SRC_CHARGESYSTEM_H_
#define ESFS_SRC_CHARGESYSTEM_H_

//TODO More than 2 charges at the same time

#include "raylib.h"
#include "raymath.h"

#include <vector>
#include <cassert>

#include <cstdio>

#ifndef RENDER_VECTOR
#define RENDER_VECTOR
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

  static int& getGlobalIndex() { return global_index; }
};

struct Cursor_Point {
  Vector2 e_res_vec;
  Color color;
  float radius;
  Vector2 position;
  bool mainCursor;
  std::vector<Vector> vectors;

  Cursor_Point() = default;
  Cursor_Point(Vector2 position, float radius, Color color, bool mainCursor) : position{position},
                                                                               radius{radius},
                                                                               color{color},
                                                                               mainCursor{mainCursor},
                                                                               e_res_vec{0, 0} {}
};

#ifndef FEATURES_STRUCT
#define FEATURES_STRUCT
struct Features {
  bool auto_zoom = false;
  bool showHelpPanel = true;
  bool showResultPanel = true;
  bool showVectorGrid = false;
};
#endif

class ChargeSystem {
 public:

  void addCharge(Vector2 position, double charge, Scale scale, float radius, Color color);
  void addCursor(Vector2 position, float radius, Color color, bool mainCursor);

  void removeCharge(int index) {
    for (auto &charge : m_charge_list) {
      if (charge.index == index) {
        m_charge_list.erase(m_charge_list.begin() + index);
        Charge_particle::getGlobalIndex() -= 1;
      }
    }
  }

  void setConstants(double e0, Scale scale, double er);
  Constants &getConstants() { return m_constants; }
  void compute_e(Features *features);
  Vector2 compute_e_grid(Cursor_Point &cursor);

  ChargeSystem();
  ~ChargeSystem() = default;

  //TODO dynamic main cursor
  //In case of multiple cursors set as mainCursor the function will return the first one
  Cursor_Point &getMainCursor() {
    if (!m_cursor_list.empty()) {
      int index = 0;
      for (const auto &cursor : m_cursor_list) {
        if (cursor.mainCursor)
          return m_cursor_list[index];
        index++;
      }
    }
    //TODO different approach for the error
    exit(-100);
  }

  std::vector<Charge_particle> &getChargesList() { return m_charge_list; }
  std::vector<Cursor_Point> &getCursorList() { return m_cursor_list; }

 private:
  std::vector<Charge_particle> m_charge_list;
  std::vector<Cursor_Point> m_cursor_list;
  Constants m_constants;
};

#endif //ESFS_SRC_CHARGESYSTEM_H_
