
#include <Engine.hpp>
#include <Scene.hpp>
#include <Surface.hpp>

#include <GLFW/glfw3.h>

#include <print>

namespace
{
   void onGlfwError(int error, const char* description)
   {
      std::println("Error: {}", description);
   }

   void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
   {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      {
         glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
   }
}   // namespace

auto main() -> int
{
   if (!glfwInit())
   {
      std::exit(-1);
   }

   glfwSetErrorCallback(onGlfwError);

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
   GLFWwindow* window = glfwCreateWindow(1024, 768, "HydraCube", nullptr, nullptr);
   if (!window)
   {
      glfwTerminate();
      std::exit(-2);
   }
   glfwSetKeyCallback(window, onKey);

   {
      glfwMakeContextCurrent(window);

      Engine engine(window);
      auto surface = engine.createSurface(window);
      auto scene   = engine.createScene();

      while (!glfwWindowShouldClose(window))
      {
         engine.render(scene, surface);

         glfwPollEvents();
      }
   }

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
