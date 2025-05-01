#include <Engine.hpp>
#include <Scene.hpp>
#include <Surface.hpp>

#include <GLFW/glfw3.h>

#include <pxr/imaging/hd/pluginRenderDelegateUniqueHandle.h>
#include <pxr/imaging/hd/rendererPluginRegistry.h>
#include <pxr/imaging/hdx/selectionTracker.h>
#include <pxr/imaging/hdx/tokens.h>
#include <pxr/imaging/hgi/tokens.h>

#include <cassert>
#include <stdexcept>

Engine::Engine(GLFWwindow* main_window) noexcept : main_window_(main_window)
{
   assert(glfwGetCurrentContext() != nullptr);

   hgi_ = pxr::Hgi::CreateNamedHgi(pxr::HgiTokens->OpenGL);

   hgi_driver_.name   = pxr::HgiTokens->renderDriver;
   hgi_driver_.driver = pxr::VtValue(hgi_.get());

   pxr::HdRendererPluginRegistry& registry = pxr::HdRendererPluginRegistry::GetInstance();
   auto resolved_id                        = registry.GetDefaultPluginId(true);

   renderer_ = registry.CreateRenderDelegate(resolved_id);
   engine_   = std::make_unique<pxr::HdEngine>();

   // TODO: Although selection is disabled, I get errors (nullptr access) during render in pxr.
   // Perhaps "tasks" in `render()` should be filtered?
   auto selection_tracker = std::make_shared<pxr::HdxSelectionTracker>();
   pxr::VtValue selectionValue(selection_tracker);
   engine_->SetTaskContextData(pxr::HdxTokens->selectionState, selectionValue);
}

Engine::~Engine() noexcept = default;

auto Engine::createSurface(GLFWwindow* window) const -> Surface
{
   if (window == main_window_)
   {
      return Surface(window);
   }
   else
   {
      throw std::runtime_error("Only main window is supported for now.");
   }
}

auto Engine::createScene() -> Scene
{
   return Scene(renderer_, hgi_driver_);
}

void Engine::render(Scene& scene, Surface& surface)
{
   auto* window = surface.window_;

   glfwMakeContextCurrent(window);
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   glViewport(0, 0, width, height);
   glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   scene.render(*engine_, width, height);

   glfwSwapBuffers(window);
}
