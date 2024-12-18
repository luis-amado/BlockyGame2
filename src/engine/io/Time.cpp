#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Time.h"

namespace {
const double STANDARD_DELTA = 0.0167;
} // namespace

double Time::s_deltaTime_ = 0.0;
double Time::s_prevTime_ = 0.0;
const double& Time::deltaTime = Time::s_deltaTime_;

void Time::Start() {
  s_prevTime_ = glfwGetTime() - STANDARD_DELTA;
  s_deltaTime_ = 0.0;
}

void Time::Update() {
  double currTime = glfwGetTime();
  s_deltaTime_ = currTime - s_prevTime_;
  s_prevTime_ = currTime;
}
