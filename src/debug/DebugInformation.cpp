#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DebugInformation.h"

#include <imgui/imgui.h>
#include "../world/World.h"
#include "../engine/camera/Camera.h"
#include "../engine/io/Time.h"
#include "DebugSettings.h"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>

#include "util/Noise.h"
#include "rendering/Texture.h"
#include <optional>

bool DebugInformation::s_showDebugInformation = false;
bool DebugInformation::s_showDebugPanel = false;
ImFont* DebugInformation::s_font = nullptr;
int DebugInformation::s_fps = 0;
double DebugInformation::s_lastFPSCheckTime = -1.0;
double DebugInformation::s_currTime = glfwGetTime();

namespace {

std::vector<unsigned char> CreateNoiseImage(int width, int height, double noiseScale, double offsetX, double offsetY, int octaves, double persistence, double lacunarity) {
  std::vector<unsigned char> data(width * height * 4);

  for (int yi = 0; yi < height; yi++) {
    double y = yi;
    for (int xi = 0; xi < width; xi++) {
      double x = xi;
      double val = Noise::Noise2D(x, y, offsetX, offsetY, noiseScale, octaves, persistence, lacunarity);
      int mappedVal = floor(((val + 1) / 2.0) * 255);
      data[(xi + yi * width) * 4] = mappedVal;
      data[(xi + yi * width) * 4 + 1] = mappedVal;
      data[(xi + yi * width) * 4 + 2] = mappedVal;
      data[(xi + yi * width) * 4 + 3] = 255;
    }
  }

  return data;
}

int capturedNoiseWidth, capturedNoiseHeight;

int noiseWidth = 400;
int noiseHeight = 400;
double noiseScale = 10.0;
float offset[] = { 0.0f, 0.0f };
int octaves = 1;
double persistence = 1.0;
double lacunarity = 1.0;

bool autoUpdate = false;
std::optional<Texture> noiseTexture = std::nullopt;

} // namespace

void DebugInformation::ShowIfActive(World& world, const Camera& camera) {

  s_currTime = glfwGetTime();
  if (s_currTime - s_lastFPSCheckTime > 0.5) {
    s_fps = Time::averageFPS;
    s_lastFPSCheckTime = s_currTime;
  }

  if (s_showDebugInformation) {
    ImGui::PushFont(s_font);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    bool open_ptr = true;
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ 1000, 1000 });
    ImGui::Begin("Debug information", &open_ptr, windowFlags);
    ImGui::Text("FPS: %d", s_fps);
    ImGui::Text("XYZ: %.5f / %.5f / %.5f", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
    ImGui::Text("Rot: %.3f %.3f", camera.GetRotation().x, camera.GetRotation().y);
    ImGui::Text("Speed: %.3f", camera.GetSpeed());
    ImGui::Text("");

    glm::ivec2 chunkCoord = world.GetChunkCoord(floor(camera.GetPosition().x), floor(camera.GetPosition().z));
    glm::ivec3 localCoords = world.ToLocalCoords(floor(camera.GetPosition().x), camera.GetPosition().y, floor(camera.GetPosition().z));

    ImGui::Text("Chunk: %d, %d", chunkCoord.x, chunkCoord.y);
    ImGui::Text("Local XYZ: %d / %d / %d", localCoords.x, localCoords.y, localCoords.z);

    ImGui::Text("");

    ImGui::Text("Terrain queue: %d", world.GetChunksToGenerateTerrainSize());
    ImGui::Text("Mesh queue: %d", world.GetChunksToGenerateMeshSize());

    ImGui::Text("");

    mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) != KERN_SUCCESS) {
      LOG(FATAL) << "Failed to get process memory usage.";
    }

    size_t processMemory = info.resident_size;

    int mib[2] = { CTL_HW, HW_MEMSIZE };
    size_t totalMemory;
    size_t length = sizeof(totalMemory);

    if (sysctl(mib, 2, &totalMemory, &length, nullptr, 0) != 0) {
      LOG(FATAL) << "Failed to get total system memory.";
    }

    // Get used and free memory using host_statistics
    size_t usedMemory = 0;
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics64_data_t vmStats;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO, reinterpret_cast<host_info_t>(&vmStats), &count) == KERN_SUCCESS) {
      size_t pageSize = vm_kernel_page_size;

      size_t activeMemory = vmStats.active_count * pageSize;
      size_t inactiveMemory = vmStats.inactive_count * pageSize;
      size_t wiredMemory = vmStats.wire_count * pageSize;

      usedMemory = activeMemory + inactiveMemory + wiredMemory;
    }

    ImGui::Text("RAM used: %zu MB", processMemory / (1024 * 1024));
    ImGui::Text("Total RAM: %zu MB / %zu MB", usedMemory / (1024 * 1024), totalMemory / (1024 * 1024));

    ImGui::Text("");

    if (ImGui::Button(s_showDebugPanel ? "Hide debug panel" : "Show debug panel")) {
      s_showDebugPanel = !s_showDebugPanel;
    }

    ImGui::Text("");

    if (world.GetBlockstateAt(floor(camera.GetPosition().x), camera.GetPosition().y, floor(camera.GetPosition().z))) {
      ImGui::Text("Inside Block");
    }

    ImGui::End();
    ImGui::PopFont();
  }

  if (s_showDebugPanel) {
    ImGui::Begin("Debug Panel");

    ImGui::BeginTabBar("DebugSettings");

    if (ImGui::BeginTabItem("World")) {

      ImGui::SliderInt("Render distance", &DebugSettings::instance.renderDistance, 1, 64);

      if (ImGui::Button(DebugSettings::instance.updateWorld ? "Stop updating world" : "Continue updating world")) {
        DebugSettings::instance.updateWorld = !DebugSettings::instance.updateWorld;
      }

      ImGui::Text("");
      ImGui::InputInt("Terrain workers", &DebugSettings::instance.terrainWorkerCount);
      ImGui::InputInt("Mesh workers", &DebugSettings::instance.meshWorkerCount);

      if (ImGui::Button("Reset workers")) {
        world.ResetWorkers();
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Terrain generation")) {
      ImGui::InputDouble("Noise Scale", &DebugSettings::instance.noiseScale);
      ImGui::InputFloat2("Noise offset", DebugSettings::instance.noiseOffsets);
      ImGui::Text("");
      ImGui::InputInt("Octaves", &DebugSettings::instance.octaves);
      ImGui::InputDouble("Persistence", &DebugSettings::instance.persistence);
      ImGui::InputDouble("Lacunarity", &DebugSettings::instance.lacunarity);
      ImGui::Text("");
      ImGui::InputInt("Base terrain height", &DebugSettings::instance.baseTerrainHeight);
      ImGui::InputInt2("Terrain Range", DebugSettings::instance.terrainRange);

      ImGui::Text("");
      if (ImGui::Button("Regenerate world")) {
        world.Regenerate();
      }

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Noise Testing")) {

      ImGui::Columns(2);

      ImGui::InputInt("Width", &noiseWidth);
      ImGui::InputInt("Height", &noiseHeight);
      ImGui::Text("");
      ImGui::Text("Noise Settings");
      ImGui::InputDouble("Scale", &noiseScale);
      ImGui::InputFloat2("Offset", offset);
      ImGui::InputInt("Octaves", &octaves);
      ImGui::InputDouble("Persistence", &persistence);
      ImGui::InputDouble("Lacunarity", &lacunarity);
      ImGui::Text("");
      ImGui::Checkbox("Auto update", &autoUpdate);
      if (autoUpdate || ImGui::Button("Create Noise Image")) {
        capturedNoiseWidth = noiseWidth;
        capturedNoiseHeight = noiseHeight;
        std::vector<unsigned char> image_data = CreateNoiseImage(capturedNoiseWidth, capturedNoiseHeight, noiseScale, offset[0], offset[1], octaves, persistence, lacunarity);
        noiseTexture = Texture(capturedNoiseWidth, capturedNoiseHeight, image_data.data());
      }

      ImGui::NextColumn();
      if (noiseTexture.has_value()) {
        ImGui::Image((ImTextureID)(intptr_t)noiseTexture->GetID(), { (float)capturedNoiseWidth, (float)capturedNoiseHeight });
      }

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
  }

}

void DebugInformation::Setup(const Window& window) {
  s_font = window.GetImGuiIO().Fonts->AddFontFromFileTTF("res/fonts/Minecraft.ttf", 20.0f, NULL);
}

void DebugInformation::Toggle() {
  s_showDebugInformation = !s_showDebugInformation;
}