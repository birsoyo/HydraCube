#pragma once

#include <pxr/imaging/hd/driver.h>
#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hd/pluginRenderDelegateUniqueHandle.h>
#include <pxr/imaging/hgi/hgi.h>

struct GLFWwindow;

class Surface;
class Scene;

class Engine final
{
public:
   explicit Engine(GLFWwindow* main_window) noexcept;
   ~Engine() noexcept;

   // TODO: Moves need to be implemented, probably.
   // Due to `render_index_` and `task_controller_` has "id" strings that are pointers (`%%p`) in them.
   // I don't know the significance of those ids. Since I plan to have only one engine, it might not matter.
   Engine(Engine&& engine) noexcept   = default;
   Engine& operator=(Engine&& engine) = default;

   auto createSurface(GLFWwindow* window) const -> Surface;
   auto createScene() -> Scene;

   void render(Scene& scene, Surface& surface);

   Engine(const Engine& engine)            = delete;
   Engine& operator=(const Engine& engine) = delete;

private:
   GLFWwindow* main_window_;

   pxr::HgiUniquePtr hgi_;
   pxr::HdDriver hgi_driver_;
   pxr::HdPluginRenderDelegateUniqueHandle renderer_;
   std::unique_ptr<pxr::HdEngine> engine_;
};
