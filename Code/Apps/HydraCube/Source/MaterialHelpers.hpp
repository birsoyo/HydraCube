#pragma once

#include <pxr/imaging/hd/dataSource.h>
#include <pxr/imaging/hd/material.h>

auto createSimpleMaterial(const pxr::SdfPath& path, pxr::GfVec3f diffuse_color) -> pxr::HdMaterialNetworkMap;
auto convertMaterialNetworkToDataSource(const pxr::HdMaterialNetworkMap& network_map)
   -> pxr::HdContainerDataSourceHandle;
