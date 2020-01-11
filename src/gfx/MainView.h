#pragma once

#include "ViewManager.h"

struct ObjectGfx;

namespace ui
{
  class GameView : public View
  {
  private:
    ViewManager* manager;

    const ObjectGfx& objectGfx(const baba::ObjectSpec* spec);

    void render();

  public:
    GameView(ViewManager* manager);
    ~GameView();

    void handleKeyboardEvent(const SDL_Event& event);
    void handleMouseEvent(const SDL_Event& event);
  };
}