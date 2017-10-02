#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>

namespace mbgl {


TileLayerIndex::TileLayerIndex(CanonicalTileID coord, std::shared_ptr<std::vector<SymbolInstance>> symbolInstances)
    : coord(coord), symbolInstances(symbolInstances) {
        for (SymbolInstance& symbolInstance : *symbolInstances) {
            std::string key; // TODO use key from symbol instance
            if (indexedSymbolInstances.find(key) == indexedSymbolInstances.end()) {
                indexedSymbolInstances.emplace(key, std::vector<IndexedSymbolInstance>{});
            }

            indexedSymbolInstances.at(key).emplace_back(symbolInstance, getScaledCoordinates(symbolInstance, coord));
        }

        // TODO
        // symbolInstance.isDuplicate = false;
        //             // If we don't pick up an opacity from our parent or child tiles
        //                         // Reset so that symbols in cached tiles fade in the same
        //                                     // way as freshly loaded tiles
        //                                                 symbolInstance.textOpacityState = new OpacityState();
        //                                                             symbolInstance.iconOpacityState = new OpacityState();
    }

Point<double> TileLayerIndex::getScaledCoordinates(SymbolInstance& symbolInstance, CanonicalTileID& childTileCoord) {
    // Round anchor positions to roughly 4 pixel grid
    const double roundingFactor = 512.0 / util::EXTENT / 2.0;
    const double scale = roundingFactor / std::pow(2, childTileCoord.z - coord.z);
    return {
        std::floor((childTileCoord.x * util::EXTENT + symbolInstance.anchor.point.x) * scale),
        std::floor((childTileCoord.y * util::EXTENT + symbolInstance.anchor.point.y) * scale)
    };
}

optional<IndexedSymbolInstance> TileLayerIndex::getMatchingSymbol(SymbolInstance& childTileSymbol, CanonicalTileID& childTileCoord) {
    std::string key; // TODO

    auto it = indexedSymbolInstances.find(key);
    if (it == indexedSymbolInstances.end()) return {};

    Point<double> childTileSymbolCoord = getScaledCoordinates(childTileSymbol, childTileCoord);

    for (IndexedSymbolInstance& thisTileSymbol: it->second) {
		// Return any symbol with the same keys whose coordinates are within 1
        // grid unit. (with a 4px grid, this covers a 12px by 12px area)
        if (std::fabs(thisTileSymbol.coord.x - childTileSymbolCoord.x) <= 1 &&
            std::fabs(thisTileSymbol.coord.y - childTileSymbolCoord.y) <= 1) {
            //return {};
            return { thisTileSymbol };
        }
    }

    return {};
}

CrossTileSymbolLayerIndex::CrossTileSymbolLayerIndex() {}

void CrossTileSymbolLayerIndex::addTile(const CanonicalTileID& coord, std::shared_ptr<std::vector<SymbolInstance>>) {
}

void CrossTileSymbolLayerIndex::removeTile(const CanonicalTileID& coord) {
}

CrossTileSymbolIndex::CrossTileSymbolIndex() {}

void CrossTileSymbolIndex::addTileLayer(std::string& layerId, const CanonicalTileID& coord, std::shared_ptr<std::vector<SymbolInstance>> symbolInstances) {
    if (layerIndexes.find(layerId) == layerIndexes.end()) {
        layerIndexes.emplace(layerId, CrossTileSymbolLayerIndex{});
    }

    CrossTileSymbolLayerIndex& layerIndex = layerIndexes.at(layerId);
    layerIndex.addTile(coord, symbolInstances);
}

void CrossTileSymbolIndex::removeTileLayer(std::string& layerId, const CanonicalTileID& coord) {
    auto it = layerIndexes.find(layerId);
    if (it != layerIndexes.end()) {
        it->second.removeTile(coord);
    }
}
} // namespace mbgl
