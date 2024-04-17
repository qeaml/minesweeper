#include "states.hpp"
#include "MineField.hpp"
#include <nwge/data/bundle.hpp>
#include <nwge/render/draw.hpp>
#include <nwge/render/window.hpp>
#include <nwge/bind.hpp>

using namespace nwge;

namespace mine {

static constexpr f32
  cConfigBaseZ = 0.5f,
  cMineFieldBaseZ = cConfigBaseZ + 0.1f;

static KeyBind
  gAccept{"mine.accept", Key::Return},
  gQuit{"mine.quit", Key::Escape};

class ConfigSubState: public nwge::SubState {
private:
  u8 mWidth = MineField::cDefaultWidth;
  u8 mHeight = MineField::cDefaultHeight;
  u8 mMineCount = MineField::cDefaultMineCount;
  u8 mSelection = 0;
  MineField &mMineField;
  render::Font &mFont;

  KeyBind
    mUp{"mine.up", Key::Up},
    mDown{"mine.down", Key::Down},
    mLess{"mine.less", Key::Left},
    mMore{"mine.more", Key::Right};

  static constexpr u8
    cMaxSelection = 3;

  static constexpr glm::vec4
    cBgColor{.0f, .0f, .0f, .5f};

  static constexpr f32
    cTextZ = cConfigBaseZ + 0.01f,
    cBgZ = cTextZ + 0.01f;

  static constexpr glm::vec3 cTextAnchor{0.14f, 0.14f, cTextZ};
  static constexpr f32
    cTextH = 0.04f,
    cArrowX = 0.1f,
    cArrowY = cTextAnchor.y + 2*cTextH;

public:
  ConfigSubState(render::Font &font, MineField &mineField)
    : mMineField(mineField), mFont(font)
  {}

  bool tick([[maybe_unused]] float delta) override {
    if(mUp.wasPressed()) {
      if(mSelection == 0) {
        mSelection = cMaxSelection;
      }
      --mSelection;
    }
    if(mDown.wasPressed()) {
      ++mSelection;
      if(mSelection == cMaxSelection) {
        mSelection = 0;
      }
    }
    if(mLess.wasPressed()) {
      switch(mSelection) {
      case 0:
        if(mWidth > MineField::cMinWidth) {
          --mWidth;
        }
        break;
      case 1:
        if(mHeight > MineField::cMinHeight) {
          --mHeight;
        }
        break;
      case 2:
        if(mMineCount > MineField::cMinMineCount) {
          --mMineCount;
        }
        break;
      }
    }
    if(mMore.wasPressed()) {
      switch(mSelection) {
      case 0:
        if(mWidth < MineField::cMaxWidth) {
          ++mWidth;
        }
        break;
      case 1:
        if(mHeight < MineField::cMaxHeight) {
          ++mHeight;
        }
        break;
      case 2:
        if(mMineCount < MineField::cMaxMineCount) {
          ++mMineCount;
        }
        break;
      }
    }
    if(gAccept.wasPressed()) {
      mMineField.prepare(mWidth, mHeight, mMineCount);
      popSubState();
      return true;
    }
    if(gQuit.wasPressed()) {
      return false;
    }

    return true;
  }

  void render() const override {
    render::color(cBgColor);
    render::rect({0, 0, cBgZ}, {1, 1});

    render::color();
    auto cursor = mFont.cursor(cTextAnchor, cTextH);
    cursor <<
      "qeaml's Super Cool Minesweeper\n\n"
      "Width: " << mWidth << '\n' <<
      "Height: " << mHeight << '\n' <<
      "Mine count: " << mMineCount << "\n"
      "\n"
      "Use Up Arrow and Down Arrow to select options\n"
      "Use Left Arrow and Right Arrow to change values\n"
      "Press Enter to start\n"
      "Press Escape to quit";
    cursor.draw();
    mFont.draw(">",
      {cArrowX, cArrowY + cTextH * f32(mSelection), cTextAnchor.z}, cTextH);
  }
};

class LossSubState: public nwge::SubState {
private:
  MineField &mMineField;
  render::Font &mFont;

  static constexpr glm::vec4
    cBgColor{.0f, .0f, .0f, .5f};

  static constexpr f32
    cTextZ = cConfigBaseZ + 0.01f,
    cBgZ = cTextZ + 0.01f;

  static constexpr glm::vec3 cTextAnchor{0.5f, 0.2f, cTextZ};
  static constexpr f32
    cTextH = 0.06f;

public:
  LossSubState(render::Font &font, MineField &mineField)
    : mMineField(mineField), mFont(font)
  {}

  bool tick([[maybe_unused]] f32 delta) override {
    if(gAccept.wasPressed()) {
      swapSubStatePtr(
        new ConfigSubState(mFont, mMineField),
        {
          .renderParent = true,
        });
      return true;
    }
    if(gQuit.wasPressed()) {
      return false;
    }
    return true;
  }

  void render() const override {
    render::color(cBgColor);
    render::rect({0, 0, cBgZ}, {1, 1});

    render::color();

    auto textSz = mFont.measure("You lost!", cTextH);
    mFont.draw("You lost!",
      {0.5f - 0.5f * textSz.x, cTextAnchor.y, cTextAnchor.z},
      cTextH);

    textSz = mFont.measure("Press Enter to retry.", cTextH);
    mFont.draw("Press Enter to retry.",
      {0.5f - 0.5f * textSz.x, cTextAnchor.y + 2*cTextH, cTextAnchor.z},
      cTextH);

    textSz = mFont.measure("Press Escape to quit.", cTextH);
    mFont.draw("Press Escape to quit.",
      {0.5f - 0.5f * textSz.x, cTextAnchor.y + 3*cTextH, cTextAnchor.z},
      cTextH);
  }
};

class VictorySubState: public nwge::SubState {
private:
  MineField &mMineField;
  render::Font &mFont;

  static constexpr glm::vec4
    cBgColor{.0f, .0f, .0f, .5f};

  static constexpr f32
    cTextZ = cConfigBaseZ + 0.01f,
    cBgZ = cTextZ + 0.01f;

  static constexpr glm::vec3 cTextAnchor{0.5f, 0.2f, cTextZ};
  static constexpr f32
    cTextH = 0.06f;

public:
  VictorySubState(render::Font &font, MineField &mineField)
    : mMineField(mineField), mFont(font)
  {}

  bool tick([[maybe_unused]] f32 delta) override {
    if(gAccept.wasPressed()) {
      swapSubStatePtr(
        new ConfigSubState(mFont, mMineField),
        {
          .renderParent = true,
        });
      return true;
    }
    if(gQuit.wasPressed()) {
      return false;
    }
    return true;
  }

  void render() const override {
    render::color(cBgColor);
    render::rect({0, 0, cBgZ}, {1, 1});

    render::color();

    auto textSz = mFont.measure("You win!", cTextH);
    mFont.draw("You win!",
      {0.5f - 0.5f * textSz.x, cTextAnchor.y, cTextAnchor.z},
      cTextH);

    textSz = mFont.measure("Press Enter to retry.", cTextH);
    mFont.draw("Press Enter to retry.",
      {0.5f - 0.5f * textSz.x, cTextAnchor.y + 2*cTextH, cTextAnchor.z},
      cTextH);

    textSz = mFont.measure("Press Escape to quit.", cTextH);
    mFont.draw("Press Escape to quit.",
      {0.5f - 0.5f * textSz.x, cTextAnchor.y + 3*cTextH, cTextAnchor.z},
      cTextH);
  }
};

/*
TODOs:
  * Fail SubState
  * Win SubState
  * SubStates loop back to Config SubState
  * 1:1 window aspect ratio
*/

class MineFieldState: public nwge::State {
private:
  data::Bundle mBundle;
  render::Font mFont;
  render::gl::Texture mIcons;
  MineField mMineField;

public:
  MineFieldState()
    : mMineField({
        {0, 0, cMineFieldBaseZ},
        {1, 1},
        &mIcons
      })
  {}

  bool preload() override {
    mBundle
      .load("mine.bndl")
      .nqTexture("icons.png", mIcons)
      .nqFont("inter.cfn", mFont);
    return true;
  }

  bool init() override {
    pushSubStatePtr(
      new ConfigSubState(mFont, mMineField),
      {
        .renderParent = true,
      });
    return true;
  }

  bool on(Event &evt) override {
    if(mMineField.lost() || mMineField.won()) {
      return true;
    }

    switch(evt.type) {
    case Event::MouseDown: {
      glm::ivec2 fieldPos = mMineField.atMousePos(evt.click.pos);
      if(fieldPos.x < 0 || fieldPos.y < 0) {
        break;
      }
      switch(evt.click.button) {
      case MouseButton::Left:
        if(mMineField.reveal(fieldPos.x, fieldPos.y)) {
          pushSubStatePtr(
            new LossSubState(mFont, mMineField),
            {
              .renderParent = true,
            });
        } else if(mMineField.won()) {
          pushSubStatePtr(
            new VictorySubState(mFont, mMineField),
            {
              .renderParent = true,
            });
        }
        break;
      case MouseButton::Middle:
      case MouseButton::Right:
        mMineField.flag(fieldPos.x, fieldPos.y);
        break;
      }
      break;
    }

    default:
      break;
    }
    return true;
  }

  void render() const override {
    render::clear();
    mMineField.render();
  }
};

State *getMineFieldState() {
  return new MineFieldState;
}

}
