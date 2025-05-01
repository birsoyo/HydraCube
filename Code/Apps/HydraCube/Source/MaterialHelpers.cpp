#include <MaterialHelpers.hpp>

#include <pxr/base/gf/vec3f.h>
#include <pxr/imaging/hd/materialConnectionSchema.h>
#include <pxr/imaging/hd/materialNodeParameterSchema.h>
#include <pxr/imaging/hd/materialNodeSchema.h>
#include <pxr/imaging/hd/materialSchema.h>
#include <pxr/imaging/hd/retainedDataSource.h>
#include <pxr/imaging/hd/tokens.h>
// #include <pxr/usdImaging/usdImaging/tokens.h>

#include <print>

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

namespace
{
   // Copied from maya-hydra project in lib\mayaHydra\hydraExtensions\sceneIndex\mayaHydraSceneIndexUtils.h file.
   pxr::HdContainerDataSourceHandle convertToMaterialNetworkSchema(const pxr::HdMaterialNetworkMap& hdNetworkMap)
   {
      using namespace pxr;
      // HD_TRACE_FUNCTION();

      TfTokenVector terminalsNames;
      std::vector<HdDataSourceBaseHandle> terminalsValues;
      std::vector<TfToken> nodeNames;
      std::vector<HdDataSourceBaseHandle> nodeValues;

      struct ParamData
      {
         VtValue value;
         TfToken colorSpace;
      };

      for (auto const& iter : hdNetworkMap.map)
      {
         const TfToken& terminalName        = iter.first;
         const HdMaterialNetwork& hdNetwork = iter.second;

         if (hdNetwork.nodes.empty())
         {
            continue;
         }

         terminalsNames.push_back(terminalName);

         // Transfer over individual nodes.
         // Note that the same nodes may be shared by multiple terminals.
         // We simply overwrite them here.
         for (const HdMaterialNode& node : hdNetwork.nodes)
         {
            std::vector<TfToken> paramsNames;
            std::vector<HdDataSourceBaseHandle> paramsValues;

            // Gather parameter value and colorspace metadata in paramsInfo, a
            // mapping of the parameter name to its value and colorspace data.
            std::map<std::string, ParamData> paramsInfo;
            for (const auto& p : node.parameters)
            {
               // Strip "colorSpace" prefix
               const std::pair<std::string, bool> res =
                  SdfPath::StripPrefixNamespace(p.first, HdMaterialNodeParameterSchemaTokens->colorSpace);

               // Colorspace metadata
               if (res.second)
               {
                  paramsInfo[res.first].colorSpace = p.second.Get<TfToken>();
               }
               // Value
               else
               {
                  paramsInfo[p.first].value = p.second.Get<VtValue>();
               }
            }

            // Create and store the HdMaterialNodeParameter DataSource
            for (const auto& item : paramsInfo)
            {
               paramsNames.push_back(TfToken(item.first));
               paramsValues.push_back(HdMaterialNodeParameterSchema::Builder()
                                         .SetValue(HdRetainedTypedSampledDataSource<VtValue>::New(item.second.value))
                                         .SetColorSpace(
                                            item.second.colorSpace.IsEmpty()
                                               ? nullptr
                                               : HdRetainedTypedSampledDataSource<TfToken>::New(item.second.colorSpace))
                                         .Build());
            }

            // Accumulate array connections to the same input
            TfDenseHashMap<TfToken, TfSmallVector<HdDataSourceBaseHandle, 8>, TfToken::HashFunctor> connectionsMap;

            TfSmallVector<TfToken, 8> cNames;
            TfSmallVector<HdDataSourceBaseHandle, 8> cValues;

            for (const HdMaterialRelationship& rel : hdNetwork.relationships)
            {
               if (rel.outputId == node.path)
               {
                  TfToken outputPath = rel.inputId.GetToken();
                  TfToken outputName = TfToken(rel.inputName.GetString());

                  HdDataSourceBaseHandle c =
                     HdMaterialConnectionSchema::Builder()
                        .SetUpstreamNodePath(HdRetainedTypedSampledDataSource<TfToken>::New(outputPath))
                        .SetUpstreamNodeOutputName(HdRetainedTypedSampledDataSource<TfToken>::New(outputName))
                        .Build();

                  connectionsMap[TfToken(rel.outputName.GetString())].push_back(c);
               }
            }

            cNames.reserve(connectionsMap.size());
            cValues.reserve(connectionsMap.size());

            // NOTE: not const because HdRetainedSmallVectorDataSource needs
            //       a non-const HdDataSourceBaseHandle*
            for (auto& entryPair : connectionsMap)
            {
               cNames.push_back(entryPair.first);
               cValues.push_back(
                  HdRetainedSmallVectorDataSource::New(entryPair.second.size(), entryPair.second.data()));
            }

            nodeNames.push_back(node.path.GetToken());
            nodeValues.push_back(
               HdMaterialNodeSchema::Builder()
                  .SetParameters(
                     HdRetainedContainerDataSource::New(paramsNames.size(), paramsNames.data(), paramsValues.data()))
                  .SetInputConnections(HdRetainedContainerDataSource::New(cNames.size(), cNames.data(), cValues.data()))
                  .SetNodeIdentifier(HdRetainedTypedSampledDataSource<TfToken>::New(node.identifier))
                  .Build());
         }

         terminalsValues.push_back(
            HdMaterialConnectionSchema::Builder()
               .SetUpstreamNodePath(
                  HdRetainedTypedSampledDataSource<TfToken>::New(hdNetwork.nodes.back().path.GetToken()))
               .SetUpstreamNodeOutputName(HdRetainedTypedSampledDataSource<TfToken>::New(terminalsNames.back()))
               .Build());
      }

      HdContainerDataSourceHandle nodesDefaultContext =
         HdRetainedContainerDataSource::New(nodeNames.size(), nodeNames.data(), nodeValues.data());

      HdContainerDataSourceHandle terminalsDefaultContext =
         HdRetainedContainerDataSource::New(terminalsNames.size(), terminalsNames.data(), terminalsValues.data());

      return HdMaterialNetworkSchema::Builder()
         .SetNodes(nodesDefaultContext)
         .SetTerminals(terminalsDefaultContext)
         .Build();
   }

}   // namespace

auto convertMaterialNetworkToDataSource(const pxr::HdMaterialNetworkMap& network_map)
   -> pxr::HdContainerDataSourceHandle
{
   using namespace pxr;

   HdDataSourceBaseHandle network = convertToMaterialNetworkSchema(network_map);

   TfToken defaultContext = HdMaterialSchemaTokens->universalRenderContext;
   return HdMaterialSchema::BuildRetained(1, &defaultContext, &network);
}
