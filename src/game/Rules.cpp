#include "Rules.h"

#include "game/Level.h"

#include <algorithm>
#include <cstdio>

using namespace baba;

std::string Rule::name() const
{
  std::string text = "";
  for (const auto& term : terms)
    text += term->spec->name.substr(5) + " ";
  return text;
}

void Rules::clear()
{
  _rules.clear();
  std::for_each(_state.begin(), _state.end(), [](decltype(_state)::value_type& pair) { pair.second.clear(); });

  _state[_data->EDGE].properties.set(ObjectProperty::STOP);
}

void Rules::generate(baba::Level* level)
{
  /* search trivial rules NOUN IS PROPERTY */

  std::vector<std::vector<const Object*>> sentences;

  for (coord_t y = 0; y < level->height(); ++y)
  {
    for (coord_t x = 0; x < level->width(); ++x)
    {
      Tile* tile = level->get(x, y);
      Tile* left = level->get(tile, D::LEFT);
      Tile* right = level->get(tile, D::RIGHT);
      Tile* up = level->get(tile, D::UP);
      Tile* down = level->get(tile, D::DOWN);

      baba::Object* is = tile->find(_data->IS);

      if (is)
      {
        std::vector<Object*> horizontal;
        std::vector<Object*> vertical;

        if (left)
        {
          auto* noun = left->find([](const Object& obj) { return obj.spec->isText && obj.spec->type == ObjectSpec::Type::Noun; });
          if (noun)
            horizontal.push_back(noun);
        }

        horizontal.push_back(is);

        if (right)
        {
          auto* property = right->find([](const Object& obj) { return obj.spec->isText && obj.spec->type == ObjectSpec::Type::Property; });
          if (property)
            horizontal.push_back(property);
        }

        if (up)
        {
          auto* noun = up->find([](const Object& obj) { return obj.spec->isText && obj.spec->type == ObjectSpec::Type::Noun; });
          if (noun)
            vertical.push_back(noun);
        }

        vertical.push_back(is);

        if (down)
        {
          auto* property = down->find([](const Object& obj) { return obj.spec->isText && obj.spec->type == ObjectSpec::Type::Property; });
          if (property)
            vertical.push_back(property);
        }

        if (horizontal.size() == 3)
          _rules.push_back({ horizontal });

        if (vertical.size() == 3)
          _rules.push_back({ vertical });
      }
    }
  }
}

void Rules::apply(baba::Level* level)
{
  for (const Rule& rule : _rules)
  {
    auto* noun = rule[0];
    auto* verb = rule[1];
    auto* property = rule[2];

    auto it = _data->objectsByGrid.find({ noun->spec->grid.x - 1, noun->spec->grid.y });
    assert(it != _data->objectsByGrid.end());
    const ObjectSpec* object = it->second;

    if (property->spec->name == "text_you")
      _state[object].properties.set(ObjectProperty::YOU);
    else if (property->spec->name == "text_stop")
      _state[object].properties.set(ObjectProperty::STOP);
    else if (property->spec->name == "text_push")
      _state[object].properties.set(ObjectProperty::PUSH);
    else if (property->spec->name == "text_win")
      _state[object].properties.set(ObjectProperty::WIN);
    else if (property->spec->name == "text_defeat")
      _state[object].properties.set(ObjectProperty::DEFEAT);
    else if (property->spec->name == "text_move")
      _state[object].properties.set(ObjectProperty::MOVE);
    else if (property->spec->name == "text_sink")
      _state[object].properties.set(ObjectProperty::SINK);
    else if (property->spec->name == "text_shift")
      _state[object].properties.set(ObjectProperty::SHIFT);
    else if (property->spec->name == "text_best")
      _state[object].properties.set(ObjectProperty::BEST);
    else if (property->spec->name == "text_hot")
      _state[object].properties.set(ObjectProperty::HOT);
    else if (property->spec->name == "text_melt")
      _state[object].properties.set(ObjectProperty::MELT);
    else if (property->spec->name == "text_float")
      _state[object].properties.set(ObjectProperty::FLOAT);
    else if (property->spec->name == "text_shut")
      _state[object].properties.set(ObjectProperty::SHUT);
    else if (property->spec->name == "text_open")
      _state[object].properties.set(ObjectProperty::OPEN);
    else
      assert(false);

    noun->active = true;
    verb->active = true;
    property->active = true;
  }

  level->forEachObject([this](Object& o) {
    if (o.spec->isText)
      _state[o.spec].properties.set(ObjectProperty::TEXT);
  });
}

void Rules::resolve(baba::Level* level)
{
  level->forEachTile([this](Tile& tile) {
    const bool hasDefeat = tile.any_of([this](const Object& object) { return hasProperty(object.spec, ObjectProperty::DEFEAT); });
  });
}