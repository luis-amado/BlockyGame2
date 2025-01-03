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

bool DebugInformation::s_showDebugInformation = false;
bool DebugInformation::s_showDebugPanel = false;
ImFont* DebugInformation::s_font = nullptr;
int DebugInformation::s_fps = 0;
double DebugInformation::s_lastFPSCheckTime = -1.0;
double DebugInformation::s_currTime = glfwGetTime();

void DebugInformation::ShowIfActive(const World& world, const Camera& camera) {

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
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Terrain generation")) {
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