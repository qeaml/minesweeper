#pragma once

/*
MineField.hpp
--------------------
The mine field, the most important part of Minesweeper.
*/

#include <nwge/common/array.hpp>
#include <nwge/common/def.h>
#include <nwge/render/aspectRatio.hpp>
#include <nwge/render/draw.hpp>

namespace mine {

struct Field {
  // only goes up to 9, so we can squeeze it in 4 bits
  u8 neighboringMines: 4 = 0;
  bool isMine: 1 = false;
  bool isRevealed: 1 = false;
  bool isFlagged: 1 = false;
  bool isQuestioned: 1 = false;

  constexpr inline s32 textureIndex(bool loss) const {
    if(loss && isMine) {
      return 15;
    }
    if(!isRevealed) {
      if(isFlagged) {
        return 13;
      }
      if(isQuestioned) {
        return 14;
      }
      return 12;
    }
    if(isMine) {
      return 15;
    }
    return neighboringMines;
  }
};

class MineField {
public:
  static constexpr u8
    cMinWidth = 5,
    cDefaultWidth = 10,
    cMaxWidth = 20,
    cMinHeight = 5,
    cDefaultHeight = 10,
    cMaxHeight = 20,
    cMinMineCount = 1,
    cDefaultMineCount = 5,
    cMaxMineCount = 20;

private:
  u8 mWidth = cDefaultWidth;
  u8 mHeight = cDefaultHeight;
  u8 mMineCount = cDefaultMineCount;
  bool mGenerated = false;
  bool mLost = false;
  nwge::Array<Field> mFields{usize(mWidth) * mHeight};

  nwge::render::AspectRatio mAR{1, 1};
  glm::vec3 mBasePos;
  glm::vec2 mMaxSize;
  glm::vec2 mPosOff;
  glm::vec2 mFieldSize;
  const nwge::render::gl::Texture &mTexture;

  /**
   * @brief Get a field without running any extra logic.
   *
   * @param x X coordinate of the field
   * @param y Y coordinate of the field
   * @return Field& The field
   */
  Field &getField(u8 x, u8 y);
  const Field &getField(u8 x, u8 y) const;

  /**
   * @brief Generate the minefield.
   *
   * @param x Entry point X coordinate
   * @param y Entry point Y coordinate
   */
  void generate(u8 x, u8 y);

  /**
   * @brief Propagate an empty field to its neighbors.
   *
   * @param x X coordinate of the field
   * @param y Y coordinate of the field
   */
  void propagate(u8 x, u8 y);

public:
  MineField(nwge::render::Rect where);

  /**
   * @brief Prepare the mine field for use.
   *
   * @param width Width of mine field
   * @param height Height of mine field
   * @param mineCount Amount of mines
   */
  void prepare(u8 width, u8 height, u8 mineCount);

  /**
   * @brief Get the field at a given coordinate.
   *
   * This will perform extra processing before giving a result. For example, if
   * the field has not yet been generated, it will generate it ensuring the
   * given coordinate is not a mine.
   *
   * In case the clicked coordinate does not have a bomb, all adjacent Hidden
   * fields are also revealed.
   *
   * @param x Field X coordinate
   * @param y Field Y coordinate
   * @return Field The field at the coordinate or Invalid.
   */
  Field &at(u8 x, u8 y);

  /**
   * @brief Reveal a field.
   *
   * This will reveal the field if it hasn't been revealed yet and returns
   * whether that field was a mine.
   *
   * @param x Field X coordinate
   * @param y Field Y coordinate
   * @return true There was a mine
   * @return false There was no mine
   */
  bool reveal(u8 x, u8 y);

  /**
   * @brief Place a flag on a field.
   *
   * @param x Field X coordinate
   * @param y Field Y coordinate
   */
  void flag(u8 x, u8 y);

  /**
   * @brief Calculate coordinate of field given mouse cursor position.
   *
   * @param mousePos Mouse position on-screen
   * @return glm::ivec2 Corresponding field or {-1, -1} if out of bounds
   */
  glm::ivec2 atMousePos(glm::vec2 mousePos);

  /**
   * @brief Check if all mines have been found.
   *
   * This will return true if all fields without mines have been revealed. Does
   * not check whether those fields have been marked.
   *
   * @return true All mines found
   * @return false Not all mines found
   */
  bool checkWin() const;

  /**
   * @brief Render the minefield.
   *
   * @param loss Whether the player has clicked on a mine
   */
  void render(bool loss) const;
};

} // namespace mine
