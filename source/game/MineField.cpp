#include "MineField.hpp"
#include <SDL2/SDL_stdinc.h>
#include <random>

using namespace nwge;

namespace mine {

MineField::MineField(render::Rect where)
  : mBasePos(where.pos), mMaxSize(where.extents), mTexture(*where.texture)
{}

void MineField::prepare(u8 width, u8 height, u8 bombCount) {
  mWidth = width;
  mHeight = height;
  mMineCount = bombCount;
  mGenerated = false;
  mLost = false;
  mFields = {usize(mWidth) * mHeight};

  if(mWidth > mHeight) {
    mFieldSize = mMaxSize / f32(mWidth);
  } else {
    mFieldSize = mMaxSize / f32(mHeight);
  }
  mPosOff = {
    0.5f * (mMaxSize.x - f32(mWidth) * mFieldSize.x),
    0.5f * (mMaxSize.y - f32(mHeight) * mFieldSize.y)
  };
}

Field &MineField::getField(u8 x, u8 y) {
  return mFields[y * mWidth + x];
}

const Field &MineField::getField(u8 x, u8 y) const {
  return mFields[y * mWidth + x];
}

Field &MineField::at(u8 x, u8 y) {
  Field &field = getField(x, y);
  if(field.isMine) {
    mLost = true;
  } else {
    propagate(x, y);
  }
  return field;
}

bool MineField::reveal(u8 x, u8 y) {
  if(!mGenerated) {
    generate(x, y);
  }
  Field &field = getField(x, y);
  if(field.isRevealed || field.isFlagged) {
    return false;
  }
  if(field.isMine) {
    return true;
  }
  propagate(x, y);
  return false;
}

void MineField::flag(u8 x, u8 y) {
  Field &field = getField(x, y);
  if(field.isRevealed) {
    return;
  }
  field.isFlagged = !field.isFlagged;
}

void MineField::generate(u8 entryX, u8 entryY) {
  mGenerated = true;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution
    xDis{0, mWidth - 1},
    yDis{0, mHeight - 1};

  for(int i = 0; i < mMineCount; ++i) {
    while(true) {
      u8 x = xDis(gen), y = yDis(gen);
      if(x == entryX && y == entryY) {
        continue;
      }
      auto &field = getField(x, y);
      if(field.isMine) {
        continue;
      }
      field.isMine = true;
      break;
    }
  }
}

void MineField::propagate(u8 x, u8 y) {
  auto &field = getField(x, y);
  if(field.isRevealed || field.isFlagged) {
    return;
  }
  field.isRevealed = true;

  // first loop - count neighboring mines
  for(s16 xOff = -1; xOff <= 1; ++xOff) {
  for(s16 yOff = -1; yOff <= 1; ++yOff) {
    if(xOff == 0 && yOff == 0) {
      continue;
    }
    s16 neighborX = s16(x) + xOff;
    s16 neighborY = s16(y) + yOff;
    if(neighborX < 0 || neighborX >= mWidth || neighborY < 0 || neighborY >= mHeight) {
      continue;
    }
    auto &neighbor = getField(neighborX, neighborY);
    if(neighbor.isMine) {
      ++field.neighboringMines;
    }
  }
  }

  // second loop - continue propagation
  if(field.neighboringMines != 0) {
    return;
  }
  for(s16 xOff = -1; xOff <= 1; ++xOff) {
  for(s16 yOff = -1; yOff <= 1; ++yOff) {
    if(xOff == 0 && yOff == 0) {
      continue;
    }
    s16 neighborX = s16(x) + xOff;
    s16 neighborY = s16(y) + yOff;
    if(neighborX < 0 || neighborX >= mWidth || neighborY < 0 || neighborY >= mHeight) {
      continue;
    }
    auto &neighbor = getField(neighborX, neighborY);
    if(!neighbor.isRevealed) {
      propagate(neighborX, neighborY);
    }
  }
  }
}

glm::ivec2 MineField::atMousePos(glm::vec2 mousePos) {
  f32 minX = mBasePos.x + mPosOff.x;
  f32 maxX = minX + f32(mWidth) * mFieldSize.x;
  f32 minY = mBasePos.y + mPosOff.y;
  f32 maxY = minY + f32(mHeight) * mFieldSize.y;
  if(mousePos.x < minX || mousePos.x >= maxX
  || mousePos.y < minY || mousePos.y >= maxY) {
    return {-1, -1};
  }
  return {
    s32((mousePos.x - minX) / mFieldSize.x),
    s32((mousePos.y - minY) / mFieldSize.y),
  };
}

static constexpr inline
render::gl::TexCoord textureIndex2TexCoord(s32 textureIndex) {
  f32 x = f32(textureIndex & 7) / 8.0f;
  f32 y = f32(textureIndex >> 3) / 2.0f;
  return {{x, y}, {1.0f/8.0f, 1.0f/2.0f}};
}

void MineField::render(bool loss) const {
  for(s32 x = 0; x < mWidth; ++x) {
  for(s32 y = 0; y < mHeight; ++y) {
    const auto &field = getField(x, y);
    glm::vec3 fieldPos{
      mBasePos.x + mPosOff.x + mFieldSize.x * x,
      mBasePos.y + mPosOff.y + mFieldSize.y * y,
      mBasePos.z
    };
    render::gl::TexCoord texCoord = textureIndex2TexCoord(field.textureIndex(loss));
    render::rect(fieldPos, mFieldSize, mTexture, texCoord);
  }
  }
}

} // namespace mine