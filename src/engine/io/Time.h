#pragma once

class Time {
public:
  static const double& deltaTime;

  static void Start();
  static void Update();
private:
  static double s_deltaTime_;
  static double s_prevTime_;
};