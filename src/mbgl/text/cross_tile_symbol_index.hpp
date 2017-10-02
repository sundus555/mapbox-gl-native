#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>

#include <map>
#include <vector>
#include <string>

namespace mbgl {

class SymbolInstance;

class IndexedSymbolInstance {
    public:
        IndexedSymbolInstance(SymbolInstance& symbolInstance, Point<double> coord)
            : instance(symbolInstance), coord(coord) {};
        SymbolInstance& instance;
        Point<double> coord;
};

class TileLayerIndex {
    public:
        TileLayerIndex(CanonicalTileID coord, std::shared_ptr<std::vector<SymbolInstance>>);
        Point<double> getScaledCoordinates(SymbolInstance&, CanonicalTileID&);
        optional<IndexedSymbolInstance> getMatchingSymbol(SymbolInstance& childTileSymbol, CanonicalTileID& childTileCoord);
    private:
        CanonicalTileID coord;
        std::map<std::string,std::vector<IndexedSymbolInstance>> indexedSymbolInstances;
        std::shared_ptr<std::vector<SymbolInstance>> symbolInstances;
};

class CrossTileSymbolLayerIndex {
    public:
        CrossTileSymbolLayerIndex();

        void addTile(const CanonicalTileID&, std::shared_ptr<std::vector<SymbolInstance>>);
        void removeTile(const CanonicalTileID&);
    private:
        std::map<std::string,std::vector<TileLayerIndex>> layerIndexes;
};

class CrossTileSymbolIndex {
    public:
        CrossTileSymbolIndex();

        void addTileLayer(std::string& layerId, const CanonicalTileID&, std::shared_ptr<std::vector<SymbolInstance>>);
        void removeTileLayer(std::string& layerId, const CanonicalTileID&);
    private:
        std::map<std::string,CrossTileSymbolLayerIndex> layerIndexes;
};

} // namespace mbgl
