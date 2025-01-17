#include "EventMouse.h"
#include "EventStep.h"
#include "EventView.h"
#include "Vector.h"
#include "WorldManager.h"

// Local
#include "./Bullet.h"
#include "./EventNuke.h"
#include "./GameOver.h"
#include "./Hero.h"
#include "ResourceManager.h"

void Hero::kbd(const df::EventKeyboard *p_keyboard_event) {
  switch (p_keyboard_event->getKey()) {
  case df::Keyboard::Q:
    if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
      WM.markForDelete(this);
    break;
  case df::Keyboard::UPARROW:
    if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
      move(-1);
    break;
  case df::Keyboard::DOWNARROW:
    if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
      move(+1);
    break;
  case df::Keyboard::SPACE:
    if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
      nuke();
    break;
  default:
    break;
  }
}

void Hero::move(int dy) {
  // See if time to move.
  if (move_countdown > 0)
    return;
  move_countdown = move_slowdown;

  df::Vector old_position = getPosition();
  df::Vector new_position(old_position.getX(), old_position.getY() + dy);

  bool is_within_boundaries =
      new_position.getY() > 3 &&
      new_position.getY() < WM.getBoundary().getHeight() - 1;

  if (is_within_boundaries) {
    WM.moveObject(this, new_position);
  }
}

void Hero::step() {
  move_countdown--;
  if (move_countdown < 0) {
    move_countdown = 0;
  }

  fire_countdown--;
  if (fire_countdown < 0) {
    fire_countdown = 0;
  }
}

void Hero::fire(df::Vector target) {
  if (fire_countdown > 0)
    return;
  fire_countdown = fire_slowdown;

  df::Vector v = target - getPosition();
  v.normalize();
  Bullet *b = new Bullet(getPosition() + df::Vector(1, 1));
  b->setVelocity(v);

  setSolidness(df::SOFT);
  df::Sound *p_sound = RM.getSound("fire");
  if (p_sound)
    p_sound->play();
}

Hero::Hero() {
  setSprite("ship");
  setType("Hero");

  df::Vector p(7, WM.getBoundary().getHeight() / 2);
  setPosition(p);

  move_slowdown = 2;
  move_countdown = move_slowdown;

  fire_countdown = 15;
  fire_slowdown = fire_countdown;

  subscribe(df::STEP_EVENT);
  subscribe(df::KEYBOARD_EVENT);
  subscribe(df::MSE_EVENT);

  p_reticle = new Reticle();
  p_reticle->draw();

  nuke_count = 1;
}

Hero::~Hero() {
  new GameOver;
  WM.markForDelete(p_reticle);

  // df::addParticles(df::SPARKS, getPosition(), 2, df::BLUE);
  // df::addParticles(df::SPARKS, getPosition(), 2, df::YELLOW);
  // df::addParticles(df::SPARKS, getPosition(), 3, df::RED);
  // df::addParticles(df::SPARKS, getPosition(), 3, df::RED);
}

void Hero::mouse(const df::EventMouse *p_mouse_event) {
  if ((p_mouse_event->getMouseAction() == df::CLICKED) &&
      (p_mouse_event->getMouseButton() == df::Mouse::LEFT))
    fire(p_mouse_event->getMousePosition());
}

int Hero::eventHandler(const df::Event *p_e) {
  if (p_e->getType() == df::KEYBOARD_EVENT) {
    const df::EventKeyboard *p_keyboard_event =
        dynamic_cast<const df::EventKeyboard *>(p_e);
    kbd(p_keyboard_event);
    return 1;
  }

  if (p_e->getType() == df::MSE_EVENT) {
    const df::EventMouse *p_mouse_event =
        dynamic_cast<const df::EventMouse *>(p_e);
    mouse(p_mouse_event);
    return 1;
  }

  if (p_e->getType() == df::STEP_EVENT) {
    step();
    return 1;
  }

  return 0;
}

void Hero::nuke() {
  if (nuke_count <= 0)
    return;
  nuke_count--;

  EventNuke nuke;
  WM.onEvent(&nuke);

  df::EventView ev("Nukes", -1, true);
  WM.onEvent(&ev);

  df::Sound *p_sound = RM.getSound("nuke");
  if (p_sound)
    p_sound->play();
}
