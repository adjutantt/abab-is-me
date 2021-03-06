#pragma once

#include "Types.h"

#include "Tile.h"
#include "Rules.h"

namespace baba
{
  using LevelState = std::vector<Tile>;

  enum LevelType { Normal = 0, Meta = 1 };

  struct LevelInfo
  {
    std::string name;
    std::string subtitle;
    std::string filename;
  };

  struct Level
  {
  private:
    Rules _rules;
    GameData _data;
    coord_t _width, _height;
    LevelState _tiles;
    LevelInfo _info;
    std::string _palette;

    std::vector<std::string> _images;

  public:
    Level(const GameData& data);

    const std::string& palette() const { return _palette; }
    const std::string& name() const { return _info.name; }
    const std::string& subtitle() const { return _info.subtitle; }
    const std::string& filename() const { return _info.filename; }
    const decltype(_images)& images() const { return _images; }

    const GameData* data() const { return &_data; }
    const Rules& rules() const { return _rules; }

    void resize(coord_t width, coord_t height);

    decltype(_tiles)::const_iterator begin() const { return _tiles.begin(); }
    decltype(_tiles)::const_iterator end() const { return _tiles.end(); }

    decltype(_tiles)::iterator begin() { return _tiles.begin(); }
    decltype(_tiles)::iterator end() { return _tiles.end(); }

    Tile* get(coord_t i) { return &_tiles[i]; }

    Tile* get(coord_t x, coord_t y) { return x >= 0 && x < _width && y >= 0 && y < _height ? &_tiles[y*_width + x] : nullptr; }
    const Tile* get(coord_t x, coord_t y) const { return x >= 0 && x < _width && y >= 0 && y < _height ? &_tiles[y*_width + x] : nullptr; }

    Tile* get(const Tile* t, D d);
    const Tile* get(const Tile* t, D d) const;

    void forEachObject(std::function<void(Object&)> lambda);
    void forEachTile(std::function<void(Tile&)> lambda);

    void computeTiling();

    coord_t height() const { return _height; }
    coord_t width() const { return _width; }

    //TODO: use std::move from History class to make it much more efficient
    void restore(const LevelState& state) { _tiles = state; }
    LevelState state() const { return _tiles; }

    bool isVictory();
    bool isDefeat();

    void updateRules();

    friend class io::Loader;
  };
}

#include <deque>

namespace baba
{
  class History
  {
  private:
    static constexpr size_t MAX_SIZE = 16;
    std::deque<LevelState> history;
  public:
    void push(LevelState&& state)
    {
      history.push_back(state);
      if (history.size() > MAX_SIZE)
        history.pop_front();
    }

    LevelState pop()
    {
      LevelState state = history.back();
      history.pop_back();
      return state;
    }

    bool empty() const { return history.empty(); }
    size_t size() const { return history.size(); }
  };
}

