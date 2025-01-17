#include "./Reticle.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "Object.h"

class Hero : public df::Object {
private:
  void kbd(const df::EventKeyboard *p_keyboard_event);
  void mouse(const df::EventMouse *p_mouse_event);
  void move(int dy);
  void step();
  void nuke();
  void fire(df::Vector target);

  int move_slowdown;
  int move_countdown;
  int fire_slowdown;
  int fire_countdown;
  int nuke_count;
  Reticle *p_reticle;

public:
  Hero();
  ~Hero();
  int eventHandler(const df::Event *p_e) override;
};