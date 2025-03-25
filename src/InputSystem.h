#ifndef ESFS_SRC_INPUTSYSTEM_H_
#define ESFS_SRC_INPUTSYSTEM_H_

#include "raylib.h"
#include "raymath.h"

struct KeysStatus {
    bool F1 = false;
    bool F2 = false;
    bool F3 = false;
    bool F4 = false;
    bool F5 = false;
    bool R = false;
    bool H = false;
    bool E = false;
    bool A = false;
    bool D = false;
    bool debug = false;
    bool space = false;
    bool verbose = false;
};

class InputSystem {
public:
    void update_input(float cameraZoom);

    [[nodiscard]] Vector2 getRefMousePos() const;

    ~InputSystem() = default;

    InputSystem() = delete;

    explicit InputSystem(float scaleFactor);

public:
    KeysStatus keysStatus;

private:
    Vector2 m_ref_mouse_pos;
    float m_scale_factor;
};
#endif //ESFS_SRC_INPUTSYSTEM_H_
