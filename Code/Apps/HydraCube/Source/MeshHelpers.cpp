#include <MeshHelpers.hpp>

#include <pxr/base/vt/array.h>
#include <pxr/imaging/hd/materialBindingsSchema.h>
#include <pxr/imaging/hd/meshSchema.h>
#include <pxr/imaging/hd/primvarsSchema.h>
#include <pxr/imaging/hd/purposeSchema.h>
#include <pxr/imaging/hd/retainedDataSource.h>
#include <pxr/imaging/hd/tokens.h>
#include <pxr/imaging/hd/visibilitySchema.h>
#include <pxr/imaging/hd/xformSchema.h>

auto createCube(const pxr::GfVec3f& center, float size, const pxr::SdfPath& material_path)
   -> pxr::HdContainerDataSourceHandle
{
   using namespace pxr;

   const auto half_size   = size / 2.0f;
   const VtVec3fArray pts = {
      {center[0] - half_size, center[1] - half_size, center[2] - half_size},
      {center[0] + half_size, center[1] - half_size, center[2] - half_size},
      {center[0] + half_size, center[1] + half_size, center[2] - half_size},
      {center[0] - half_size, center[1] + half_size, center[2] - half_size},

      {center[0] - half_size, center[1] - half_size, center[2] + half_size},
      {center[0] + half_size, center[1] - half_size, center[2] + half_size},
      {center[0] + half_size, center[1] + half_size, center[2] + half_size},
      {center[0] - half_size, center[1] + half_size, center[2] + half_size},
   };
   const VtIntArray face_counts = {4, 4, 4, 4, 4, 4};

   // clang-format off
      const VtIntArray indices = {
         0, 3, 2, 1,  // bottom
         4, 5, 6, 7,  // top
         5, 1, 2, 6,  // right
         3, 2, 6, 7,  // back
         0, 3, 7, 1,  // left
         0, 1, 5, 4,  // front
      };

      const VtVec3fArray nrm = {
          {0, 0, -1},
          {0, 0, 1},
          {1, 0, 0},
          {0, 1, 0},
          {-1, 0, 0},
          {0, -1, 0},
      };

      const VtVec3fArray colors = {
          {0, 0, 1},
          {0, 0, 1},
          {1, 0, 0},
          {0, 1, 0},
          {1, 0, 0},
          {0, 1, 0},
          {0, 1, 0},
          {0, 1, 0},
      };
   // clang-format on

   GfMatrix4d transform;
   transform.SetIdentity();

   const TfToken material_binding_purposes[] = {
      HdMaterialBindingsSchemaTokens->allPurpose,
   };
   HdDataSourceBaseHandle material_binding_sources[] = {
      HdMaterialBindingSchema::Builder().SetPath(HdRetainedTypedSampledDataSource<SdfPath>::New(material_path)).Build(),
   };

   // clang-format off
   auto data_source = HdRetainedContainerDataSource::New(
      HdPrimTypeTokens->mesh,
      HdMeshSchema::Builder()
        .SetDoubleSided(HdRetainedTypedSampledDataSource<bool>::New(true))
        .SetTopology(HdMeshTopologySchema::Builder()
                               .SetFaceVertexCounts(HdRetainedTypedSampledDataSource<VtIntArray>::New(face_counts))
                               .SetFaceVertexIndices(HdRetainedTypedSampledDataSource<VtIntArray>::New(indices))
                               .SetOrientation(HdRetainedTypedSampledDataSource<TfToken>::New(HdTokens->rightHanded))
                               .Build())
              .Build(),
          HdPrimvarsSchemaTokens->primvars,
          HdRetainedContainerDataSource::New(
              HdTokens->points,
              HdPrimvarSchema::Builder()
                  .SetPrimvarValue(HdRetainedTypedSampledDataSource<VtVec3fArray>::New(pts))
                  .SetRole(HdPrimvarSchema::BuildRoleDataSource(HdPrimvarSchemaTokens->point))
                  .SetInterpolation(HdPrimvarSchema::BuildInterpolationDataSource(HdPrimvarSchemaTokens->vertex))
                  .Build()
          //,HdTokens->displayColor,
          //HdPrimvarSchema::Builder()
          //    .SetPrimvarValue(HdRetainedTypedSampledDataSource<VtVec3fArray>::New(colors))
          //    .SetInterpolation(HdPrimvarSchema::BuildInterpolationDataSource(
          //        HdPrimvarSchemaTokens->vertex))
          //    .SetRole(HdPrimvarSchema::BuildRoleDataSource(HdPrimvarSchemaTokens->color))
          //    .Build()
              //,HdTokens->normals,
              //HdPrimvarSchema::Builder()
              //    .SetPrimvarValue(HdRetainedTypedSampledDataSource<VtVec3fArray>::New(nrm))
              //    .SetRole(HdPrimvarSchema::BuildRoleDataSource(HdPrimvarSchemaTokens->normal))
              //    .SetInterpolation(HdPrimvarSchema::BuildInterpolationDataSource(HdPrimvarSchemaTokens->faceVarying))
              //    .Build()),  // end of primvars
             ),
          HdPurposeSchemaTokens->purpose,
          HdPurposeSchema::Builder()
            .SetPurpose(HdRetainedTypedSampledDataSource<TfToken>::New(HdRenderTagTokens->geometry))
            .Build(),
          HdVisibilitySchemaTokens->visibility,
          HdVisibilitySchema::Builder().SetVisibility(HdRetainedTypedSampledDataSource<bool>::New(true)).Build(),
          HdXformSchemaTokens->xform,
          HdXformSchema::Builder()
            .SetMatrix(HdRetainedTypedSampledDataSource<GfMatrix4d>::New(transform))
            .SetResetXformStack(HdRetainedTypedSampledDataSource<bool>::New(true))
            .Build(),
          HdMaterialBindingsSchemaTokens->materialBindings,
          HdMaterialBindingsSchema::BuildRetained(
            TfArraySize(material_binding_purposes), material_binding_purposes, material_binding_sources)
      );
   // clang-format on

   return data_source;
}
