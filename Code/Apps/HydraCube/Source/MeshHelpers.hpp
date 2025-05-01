#pragma once

#include <pxr/base/gf/vec3f.h>
#include <pxr/imaging/hd/dataSource.h>
#include <pxr/usd/sdf/path.h>

auto createCube(const pxr::GfVec3f& center, float size, const pxr::SdfPath& material_path)
   -> pxr::HdContainerDataSourceHandle;
