#include <MaterialHelpers.hpp>

#include <pxr/base/gf/vec3f.h>
#include <pxr/imaging/hd/materialSchema.h>
#include <pxr/imaging/hd/retainedDataSource.h>
#include <pxr/imaging/hd/tokens.h>
#include <pxr/imaging/hd/utils.h>

auto createSimpleMaterial(const pxr::SdfPath& path, pxr::GfVec3f diffuse_color) -> pxr::HdMaterialNetworkMap
{
   using namespace pxr;

   // Create a simple material network
   HdMaterialNode node;
   node.identifier = TfToken("UsdPreviewSurface");
   node.path       = path;

   node.parameters[TfToken("diffuseColor")] = VtValue(diffuse_color);
   node.parameters[TfToken("roughness")]    = VtValue(0.5f);
   node.parameters[TfToken("opacity")]      = VtValue(1.0f);

   HdMaterialNetwork network;
   network.nodes.push_back(std::move(node));

   HdMaterialNetworkMap network_map;
   network_map.map[HdMaterialTerminalTokens->surface] = std::move(network);
   network_map.terminals.push_back(path);

   return network_map;
}

auto convertMaterialNetworkToDataSource(const pxr::HdMaterialNetworkMap& network_map)
   -> pxr::HdContainerDataSourceHandle
{
   using namespace pxr;
   auto material_data_source = HdUtils::ConvertHdMaterialNetworkToHdMaterialSchema(network_map);
   auto data_source = HdRetainedContainerDataSource::New(HdPrimTypeTokens->material, material_data_source);
   return data_source;
}
