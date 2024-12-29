#pragma once

class Time {
public:
  static const double& deltaTime;
  static const int& averageFPS;
  static const int& rawFPS;

  static void Start();
  static void Update();
private:
  static double s_deltaTime_;
  static int s_averageFPS_;
  static int s_rawFPS_;
  static double s_prevTime_;
};