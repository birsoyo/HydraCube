#pragma once

struct GLFWwindow;

class Surface final
{
public:
private:
   friend class Engine;
   explicit Surface(GLFWwindow* window) noexcept;

private:
   GLFWwindow* window_;
};
