#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <deque>

#include "Time.h"

namespace {
const double STANDARD_DELTA = 1.0 / 120.0;
const double MAX_DELTA = 0.1;

// Variables for FPS calculation
std::deque<double> frameTimes; // Stores the delta times for averaging
const size_t MAX_FRAMES = 100; // Number of frames to average over
} // namespace

double Time::s_deltaTime_ = 0.0;
double Time::s_prevTime_ = 0.0;
const double& Time::deltaTime = Time::s_deltaTime_;

int Time::s_averageFPS_ = 0;
int Time::s_rawFPS_ = 0;
const int& Time::averageFPS = Time::s_averageFPS_;
const int& Time::rawFPS = Time::s_rawFPS_;

void Time::Start() {
  s_prevTime_ = glfwGetTime() - STANDARD_DELTA;
  s_deltaTime_ = STANDARD_DELTA;
}

void Time::Update() {
  double currTime = glfwGetTime();
  double rawDeltaTime = currTime - s_prevTime_;

  // Clamp the delta to reduce spikes (not sure if this is a good thing to do)
  s_deltaTime_ = (rawDeltaTime > MAX_DELTA) ? MAX_DELTA : rawDeltaTime;

  // Add delta time to the deque
  frameTimes.push_back(s_deltaTime_);
  if (frameTimes.size() > MAX_FRAMES) {
    frameTimes.pop_front(); // Remove the oldest frame time
  }

  // Calculate the average FPS
  double totalFrameTime = 0.0;
  for (double dt : frameTimes) {
    totalFrameTime += dt;
  }
  s_averageFPS_ = frameTimes.size() / totalFrameTime; // Average FPS

  s_rawFPS_ = 1.0 / s_deltaTime_;

  s_prevTime_ = currTime;
}