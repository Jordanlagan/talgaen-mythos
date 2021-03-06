#include "editormap.h"
#include "Rect.h"
#include "Math/Operations.h"
#include "Texture.h"
#include "renderableshapes.h"
#include "Math/Operations.h"
#include "sys.h"
#include "gdata.h"
#include "AssetManager.h"

namespace talga
{
  namespace editor
  {
    EditorMap::EditorMap()
      : Map()
      , mWorkingLayer{nullptr}
      , mWorkingLayerIndex{-1}
    {
    }

    EditorMap::~EditorMap()
    {
    }

    EditorMap::EditorMap(const EditorMap &cpy)
      : Map (cpy)
      , mWorkingLayer(cpy.mWorkingLayer)
      , mWorkingLayerIndex(cpy.mWorkingLayerIndex)
    {

    }

    const EditorMap &EditorMap::operator=(const EditorMap &cpy)
    {
      Map::operator =(cpy);
      mWorkingLayer = cpy.mWorkingLayer;
      mWorkingLayerIndex = cpy.mWorkingLayerIndex;

      return *this;
    }

    const EditorMap &EditorMap::operator=(const Map &cpy)
    {
      Map::operator =(cpy);
      return *this;
    }

    void EditorMap::setInitScriptPath(const std::string &path)
    {
      Script* script = (Script*)mInitScript;
      mInitScript = GData::getInstance()->getManager()->AddScript( path);

    }

    void EditorMap::setWidth(I32 w)
    {
      for (auto it = mLayers.begin(); it != mLayers.end(); ++it)
      {
        std::vector<I32> newIndices( w * mHeight);

        if (w >= mWidth)
        {
          I32 zeroOverflow = w - mWidth;

          for (I32 y = 0; y < mHeight; ++y)
          {
            for (I32 x = 0; x < w; ++x)
            {
              if (x < mWidth)
                newIndices[y * w + x] = (*it)[y * mWidth + x];
              else
                newIndices[y * w + x] = 0;
            }
          }
        }
        else
        {
          for (I32 y = 0; y < mHeight; ++y)
          {
            for (I32 x = 0; x < w; ++x)
            {
              newIndices[y * w + x] = (*it)[y * mWidth + x];
            }
          }
        }
        it->mIndices = newIndices;
      }

      mWidth = w;
    }

    void EditorMap::setHeight(I32 h)
    {
      for (auto it = mLayers.begin(); it != mLayers.end(); ++it)
      {
        std::vector<I32> newIndices( mWidth * h);

        if (h > mHeight)
        {
          I32 zeroOverflow = h - mHeight;

          for (I32 y = 0; y < h; ++y)
          {
            for (I32 x = 0; x < mWidth; ++x)
            {
              if (y < mHeight)
                newIndices[y * mWidth + x] = (*it)[y * mWidth + x];
              else
                newIndices[y * mWidth + x] = 0;
            }
          }
        }
        else
        {
          for (I32 y = 0; y < h; ++y)
          {
            for (I32 x = 0; x < mWidth; ++x)
            {
              newIndices[y * mWidth + x] = (*it)[y * mWidth + x];
            }
          }
        }
        it->mIndices = newIndices;
      }

      mHeight = h;
    }


    void EditorMap::addSceneGeom(IRenderable *shape)
    {
      TALGA_ASSERT(dynamic_cast<RdrRect*>(shape) || dynamic_cast<RdrTri*>(shape), "tried to push a non triangle/point into scene geom");
      mStaticSceneGeom.push_back(shape);
    }

    IRenderable* EditorMap::getSceneGeom(const vec3& pnt)
    {
      for (auto it = mStaticSceneGeom.begin(); it != mStaticSceneGeom.end(); ++it)
      {
        if (dynamic_cast<RdrRect*>(*it))
        {
          RdrRect* rect = static_cast<RdrRect*>(*it);

          for (int i = 0; i < 4; ++i)
          {
            if (pointInRenderableRect(rect->getChildren()[i], pnt))
              return const_cast<IRenderable*>(rect->getChildren()[i]);
          }

          if (pointInRect(rect->box(), pnt))
          {
            return rect;
          }
        }
        else if (dynamic_cast<RdrTri*>(*it))
        {
          RdrTri* tri = static_cast<RdrTri*>(*it);

          for (int i = 0; i < 3; ++i)
          {
            if (pointInRenderableRectParentTri(tri->getChildren()[i], pnt))
              return const_cast<IRenderable*>(tri->getChildren()[i]);
          }

          if (pointIsInTri(tri->getBase(), pnt))
          {
            return tri;
          }
        }
      }

      return nullptr;
    }

    void EditorMap::removeSceneGeom(const IRenderable *rdr)
    {
      for (auto& itm : mStaticSceneGeom)
      {
        if (rdr == itm)
        {
          if (mStaticSceneGeom.size() == 1)
          {
            delete itm;
            mStaticSceneGeom.pop_back();
            return;
          }
          else
          {
            delete itm;
            itm = mStaticSceneGeom.back();
            mStaticSceneGeom.pop_back();
            return;
          }
        }
      }
    }

    I32 EditorMap::getOffset(cpTex tex) const
    {
      for (I32 i = 0; i < mTileSet.size(); ++i)
      {
        if (tex == mTileSet[i].first)
        {
          return i;
        }
      }

      TALGA_WARN(0, std::string("attempted to find the offest of non exsistent texture in map ") + mName );
      return -1;
    }

    I32 EditorMap::getTileOffset(Tile t) const
    {
      if (t.first == nullptr)
        return 0;
      I32 texOffset = getOffset(t.first);

      I32 x = t.second[3](0) * t.first->w() / mTileWidth;
      I32 y = t.second[3](1) * t.first->h() / mTileHeight;


      return texOffset + (y * ((F32)t.first->w() / mTileWidth) + x) + 1;
    }

    void EditorMap::insertSheet(cpTex tex)
    {
      bool found = false;
      for (auto iter = mTileSet.begin(); iter != mTileSet.end(); ++iter)
      {
        if (iter->first == tex) return;
      }

      for (I32 y = 0; y < tex->h(); y += mTileHeight)
      {
        for (I32 x = 0; x < tex->w(); x += mTileWidth)
        {
          vec2 uvBL = GET_UV( x, tex->w(), y, tex->h());
          vec2 uvBR = GET_UV( x + mTileWidth, tex->w(), y, tex->h());
          vec2 uvTR = GET_UV( x + mTileWidth, tex->w(),y + mTileHeight, tex->h());
          vec2 uvTL = GET_UV( x, tex->w(), y + mTileHeight, tex->h());

          mTileSet.push_back( Tile{tex, UVFrame{{uvTL, uvTR, uvBR, uvBL}}} );
        }
      }

      TALGA_MSG(tex->getName() + " was sucessfully added to the map " + mName);
      mIsSaved = false;
      return;
    }

    MapLayer EditorMap::addLayer(const MapLayer &cpy, I32 index)
    {
      mLayers.insert(mLayers.begin() + index, cpy);
      setWorkingLayer(mLayers[index].getName());

      std::cout << "size: " << mLayers.size() << std::endl;
      std::cout << "capacity: " << mLayers.capacity() << std::endl;
      mIsSaved = false;
      return cpy;
    }

    MapLayer EditorMap::addLayer(std::string layerName, I32 index)
    {
      MapLayer temp = MapLayer((mWidth * mHeight), layerName);
      if (mLayers.size() >= MAX_LAYERS)
        return temp;


      mLayers.insert(mLayers.begin() + index, temp);
      setWorkingLayer(mLayers[index].getName());

      std::cout << "size: " << mLayers.size() << std::endl;
      std::cout << "capacity: " << mLayers.capacity() << std::endl;
      mIsSaved = false;
      return temp;
    }

    MapLayer EditorMap::deleteLayer(std::string name)
    {
      MapLayer temp;
      for (auto it = mLayers.begin(); it != mLayers.end(); ++it)
      {
        if (name == it->getName())
        {
          temp = *it;
          mLayers.erase(it);
          return temp;
        }
      }
      mIsSaved = false;
      return temp;
    }

    IndicesList EditorMap::insertTile(const std::vector<iPnt>& dropPositions, const std::vector<Tile>& tiles)
    {

      //insertSheet(tiles[0].first);

      I32 offset = getOffset(tiles[0].first);
      IndicesList previousIndices;

      if (!mWorkingLayer)
        return previousIndices;

      for (I32 i = 0; i < tiles.size(); ++i)
      {
        if (!Exists(dropPositions[i].x(), dropPositions[i].y())) continue;
        previousIndices.push_back( getTile(dropPositions[i].x(), dropPositions[i].y(), mWorkingLayerIndex) );

        mLayers[mWorkingLayerIndex][ dropPositions[i].y() * mWidth + dropPositions[i].x() ] = getTileOffset(tiles[i]);
      }
      mIsSaved = false;

      return previousIndices;
    }

    Tile EditorMap::getTileAtSheet(I32 x, I32 y, cpTex tex)
    {
      I32 offset = getOffset(tex);
      TALGA_ASSERT(offset != -1, "attempted to get offset of nonexistent texture");

      I32 textureTileWidth = tex->w() / mTileWidth;
      I32 textureTileHeight = tex->h() / mTileHeight;

      if (!((x >= 0 && x < textureTileWidth) && (y >= 0 && y < textureTileHeight)))
      {
        return BLANK_TILE;
      }

      return mTileSet[offset + (y * textureTileWidth + x)];
    }

    std::vector<Tile> EditorMap::getTiles(std::vector<iPnt> tiles, cpTex tex)
    {
      std::vector<Tile> set;

      for (const auto& pnt : tiles)
      {
        set.push_back(getTileAtSheet(pnt.x(), pnt.y(), tex));
      }

      return set;
    }

    std::string EditorMap::setWorkingLayer(std::string layerName)
    {
      I32 i = 0;
      std::string previousName = layerName;
      if (mWorkingLayer)
          previousName = mWorkingLayer->getName();

      if (layerName == "null")
      {
        mWorkingLayer = nullptr;
        return previousName;
      }
      for (auto it = mLayers.begin(); it != mLayers.end(); it++)
      {
        std::string comp = (*it).getName();
        if (layerName == comp)
        {
          mWorkingLayer = &(*it);
          mWorkingLayerIndex = i;
          return previousName;
        }
        ++i;
      }

      TALGA_WARN(0, "searched for layer: " + layerName + " found nothing")
      mIsSaved = false;
      return previousName;
    }



  }
}
