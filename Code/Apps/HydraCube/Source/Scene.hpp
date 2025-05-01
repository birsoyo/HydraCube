#pragma once

#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hd/pluginRenderDelegateUniqueHandle.h>
#include <pxr/imaging/hd/retainedSceneIndex.h>
#include <pxr/imaging/hdx/taskController.h>

struct GLFWwindow;

class Scene final
{
public:
   Scene(const Scene&)            = delete;
   Scene& operator=(const Scene&) = delete;

private:
   friend class Engine;
   explicit Scene(const pxr::HdPluginRenderDelegateUniqueHandle& renderer, pxr::HdDriver& hgi_driver) noexcept;

   void render(pxr::HdEngine& engine, int width, int height);

private:
   std::unique_ptr<pxr::HdRenderIndex> render_index_;
   std::unique_ptr<pxr::HdxTaskController> task_controller_;

   pxr::HdRetainedSceneIndexRefPtr scene_;
   pxr::GfVec3d eye_, at_, up_;
};
