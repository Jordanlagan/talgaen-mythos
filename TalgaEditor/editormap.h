#ifndef EDITORMAP_H
#define EDITORMAP_H

#include "Map.h"
#include "Cmn.h"

namespace talga
{

  class AssetManager;
  class Rect;

  namespace editor
  {

    typedef std::vector<Tile> IndicesList;
    class EditorMap : public Map
    {
    public:
      EditorMap();
      EditorMap(const EditorMap& cpy);
      virtual ~EditorMap();

      const EditorMap& operator=(const EditorMap& cpy);
      const EditorMap& operator=(const Map& cpy);

      I32 getOffset(cpTex tex) const;
      I32 getTileOffset(Tile t) const;
      Tile getTileAtSheet(I32 x, I32 y, cpTex tex);
      void insertSheet(cpTex tex);
      MapLayer addLayer(std::string layerName, I32 index);
      MapLayer addLayer(const MapLayer& cpy, I32 index);
      MapLayer deleteLayer(std::string name);

      IndicesList insertTile(const std::vector<iPnt>& dropPositions, const std::vector<Tile>& tiles);
      std::vector<Tile> getTiles(std::vector<iPnt> tiles, cpTex tex);
      std::vector<MapLayer>* getLayers() { return &mLayers; }

      const MapLayer* getWorkingLayer() const {return mWorkingLayer;}
      I32 getWorkingLayerIndex() const {return mWorkingLayerIndex;}
      std::string setWorkingLayer(std::string layerName);
      void setInitScriptPath(const std::string& path);
      void setWidth(I32 w);
      void setHeight(I32 h);

      void addSceneGeom(IRenderable* shape);
      IRenderable* getSceneGeom(const vec3& point);
      void removeSceneGeom(const IRenderable* rdr);
    private:
      MapLayer* mWorkingLayer;
      I32 mWorkingLayerIndex;

    };

  }
}

#endif // EDITORMAP_H
