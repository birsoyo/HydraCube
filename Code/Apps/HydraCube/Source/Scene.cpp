#include <Scene.hpp>

#include <MaterialHelpers.hpp>
#include <MeshHelpers.hpp>

#include <pxr/base/gf/frustum.h>
#include <pxr/imaging/hd/tokens.h>

Scene::Scene(const pxr::HdPluginRenderDelegateUniqueHandle& renderer, pxr::HdDriver& hgi_driver) noexcept
   : eye_(1, -5, 1)
   , at_(0, 0, 0)
   , up_(0, 0, 1)
{
   const auto renderer_instance_id =
      pxr::TfStringPrintf("ViewerEngine_%s_%p", renderer.GetPluginId().GetText(), renderer.Get());

   render_index_.reset(pxr::HdRenderIndex::New(renderer.Get(), {&hgi_driver}, renderer_instance_id));

   task_controller_ =
      std::make_unique<pxr::HdxTaskController>(render_index_.get(), pxr::SdfPath("/ViewerEngine"), true);

   pxr::HdxRenderTaskParams params;
   params.enableLighting = true;
   task_controller_->SetRenderParams(params);

   task_controller_->SetEnableSelection(false);
   task_controller_->SetOverrideWindowPolicy(pxr::CameraUtilFit);

   pxr::GlfSimpleLightingContextRefPtr lightingContextState = pxr::GlfSimpleLightingContext::New();

   {
      pxr::GlfSimpleLight light{pxr::GfVec4f(1, -1, 2, 1)};
      light.SetDiffuse(pxr::GfVec4f(2.1, 2.1, 2.1, 1));
      const pxr::GlfSimpleLightVector lights = {
         light,
      };
      lightingContextState->SetLights(lights);
   }
   lightingContextState->SetUseLighting(true);

   task_controller_->SetLightingState(lightingContextState);

   scene_ = pxr::HdRetainedSceneIndex::New();

   const auto material_path        = pxr::SdfPath::AbsoluteRootPath().AppendChild(pxr::TfToken("default_material"));
   const auto material_network_map = createSimpleMaterial(material_path, pxr::GfVec3f(0.8, 0.1, 0.2));
   const auto material             = convertMaterialNetworkToDataSource(material_network_map);

   const auto mesh_path = pxr::SdfPath("/cube");
   const auto mesh      = createCube(pxr::GfVec3f(0, 0, 0), 1.0f, material_path);

   scene_->AddPrims({
      {material_path, pxr::HdPrimTypeTokens->material, material},
      {mesh_path, pxr::HdPrimTypeTokens->mesh, mesh},
   });
   render_index_->InsertSceneIndex(scene_, pxr::SdfPath::AbsoluteRootPath());
}

void Scene::render(pxr::HdEngine& engine, const int width, const int height)
{
   task_controller_->SetRenderViewport(pxr::GfVec4f(0, 0, 1.0f * width, 1.0f * height));

   const auto view_mat = pxr::GfMatrix4d().SetLookAt(eye_, at_, up_);

   pxr::GfFrustum frustum;
   const double aspect = width / height;
   frustum.SetPerspective(45.0, aspect, 1, 10000);
   const auto projection_mat = frustum.ComputeProjectionMatrix();

   task_controller_->SetFreeCameraMatrices(view_mat, projection_mat);

   auto tasks = task_controller_->GetRenderingTasks();
   engine.Execute(render_index_.get(), &tasks);
}
