#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <string>
#include <optional>
#include <glm/gtc/matrix_transform.hpp>

#include "DebugInformation.h"

#include "../world/World.h"
#include "../engine/io/Time.h"
#include "DebugSettings.h"
#include "DebugShapes.h"
#include "rendering/meshes/ColoredLinesMesh.h"


#include "util/Noise.h"
#include "rendering/textures/Texture.h"
#include "util/Logging.h"
#include "util/MathUtil.h"
#include "util/FileUtil.h"

#include "../block/Block.h"
#include "../init/Blocks.h"
#include "../engine/io/Input.h"
#include "../engine/rendering/ShaderLibrary.h"

bool DebugInformation::s_showDebugInformation = false;
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

// const char* YN(bool value) {
//   return value ? "Yes" : "No";
// }

void AddLineToMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, int& vertexCount, glm::vec3 start, glm::vec3 end, glm::vec3 color) {
  // each line is two vertices
  vertices.push_back(start.x);
  vertices.push_back(start.y);
  vertices.push_back(start.z);
  vertices.push_back(color.r);
  vertices.push_back(color.g);
  vertices.push_back(color.b);

  vertices.push_back(end.x);
  vertices.push_back(end.y);
  vertices.push_back(end.z);
  vertices.push_back(color.r);
  vertices.push_back(color.g);
  vertices.push_back(color.b);

  indices.push_back(vertexCount);
  indices.push_back(vertexCount + 1);

  vertexCount += 2;
};

ColoredLinesMesh CreateChunkBoundaryMesh() {
  ColoredLinesMesh mesh;

  int cw = Chunk::CHUNK_WIDTH;

  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  int vertexCount = 0;

  // Vertical corners (lower left in yellow, others in red)
  AddLineToMesh(vertices, indices, vertexCount, { 0, 0, 0 }, { 0, Chunk::CHUNK_HEIGHT, 0 }, { 1.0f, 1.0f, 0.0f });
  AddLineToMesh(vertices, indices, vertexCount, { cw, 0, 0 }, { cw, Chunk::CHUNK_HEIGHT, 0 }, { 1.0f, 0.0f, 0.0f });
  AddLineToMesh(vertices, indices, vertexCount, { 0, 0, cw }, { 0, Chunk::CHUNK_HEIGHT, cw }, { 1.0f, 0.0f, 0.0f });
  AddLineToMesh(vertices, indices, vertexCount, { cw, 0, cw }, { cw, Chunk::CHUNK_HEIGHT, cw }, { 1.0f, 0.0f, 0.0f });

  // Vertical in between lines (off white)
  for (int i = 1; i <= 15; i++) {
    AddLineToMesh(vertices, indices, vertexCount, { i, 0, 0 }, { i, Chunk::CHUNK_HEIGHT, 0 }, { 0.8f, 0.8f, 0.8f });
    AddLineToMesh(vertices, indices, vertexCount, { 0, 0, i }, { 0, Chunk::CHUNK_HEIGHT, i }, { 0.8f, 0.8f, 0.8f });
    AddLineToMesh(vertices, indices, vertexCount, { i, 0, cw }, { i, Chunk::CHUNK_HEIGHT, cw }, { 0.8f, 0.8f, 0.8f });
    AddLineToMesh(vertices, indices, vertexCount, { cw, 0, i }, { cw, Chunk::CHUNK_HEIGHT, i }, { 0.8f, 0.8f, 0.8f });
  }

  // Horizontal lines dividing subchunks in blue, all rest in white
  for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++) {

    glm::vec3 color = { 0.5f, 0.5f, 0.5f };
    if (y % Chunk::SUBCHUNK_HEIGHT == 0) color = { 0.0f, 0.0f, 1.0f };

    AddLineToMesh(vertices, indices, vertexCount, { 0, y, 0 }, { cw, y, 0 }, color);
    AddLineToMesh(vertices, indices, vertexCount, { 0, y, 0 }, { 0, y, cw }, color);
    AddLineToMesh(vertices, indices, vertexCount, { cw, y, 0 }, { cw, y, cw }, color);
    AddLineToMesh(vertices, indices, vertexCount, { 0, y, cw }, { cw, y, cw }, color);
  }

  mesh.SetData(vertices.data(), vertices.size(), indices.data(), indices.size());

  return mesh;
}

void DrawChunkBoundaries(const glm::dvec3& playerPosition) {
  // TODO: This is a little slow, can just precalculate this as a mesh and draw it at the position instead as it never changes
  glm::ivec2 chunkCoord = World::GetChunkCoord(std::floor(playerPosition.x), std::floor(playerPosition.z));

  static ColoredLinesMesh chunkBoundaryMesh = CreateChunkBoundaryMesh();
  Shader& shader = ShaderLibrary::GetInstance().Get("colored_lines");

  glm::mat4 model(1.0f);
  model = glm::translate(model, { chunkCoord.x * Chunk::CHUNK_WIDTH, 0, chunkCoord.y * Chunk::CHUNK_WIDTH });

  shader.Use();
  shader.LoadMatrix4f("model", model);

  chunkBoundaryMesh.Draw();

  return;

}

} // namespace

void DebugInformation::ShowIfActive(World& world, const PlayerEntity& player) {

  s_currTime = glfwGetTime();
  if (s_currTime - s_lastFPSCheckTime > 0.5) {
    s_fps = Time::averageFPS;
    s_lastFPSCheckTime = s_currTime;
  }

  if (s_showDebugInformation) {
    glm::dvec3 playerPos = player.GetPosition();

    if (DebugSettings::instance.showPlayerHitbox && !player.IsGhost()) {
      glDisable(GL_DEPTH_TEST);
      DebugShapes::DrawBoundingBox(player.GetBoundingBox(), playerPos);
      glEnable(GL_DEPTH_TEST);
    }

    if (DebugSettings::instance.showChunkBoundaries) {
      DrawChunkBoundaries(playerPos);
    }

    ImGui::PushFont(s_font);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    bool open_ptr = true;
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ 5000, 5000 });
    ImGui::Begin("Debug information", &open_ptr, windowFlags);
    ImGui::Text("FPS: %d", s_fps);
    ImGui::Text("XYZ: %.5f / %.5f / %.5f", playerPos.x, playerPos.y, playerPos.z);
    ImGui::Text("Rot: %.3f %.3f", player.GetRotation().x, player.GetRotation().y);
    ImGui::Text("");
    ImGui::Text("Vel: %.3f %.3f %.3f", player.GetVelocity().x, player.GetVelocity().y, player.GetVelocity().z);
    ImGui::Text("Speed: %.2lfx", player.GetSpeedMultiplier());
    ImGui::Text("");
    if (player.GetLookingAtBlock().has_value()) {
      const glm::ivec3& lookingAt = player.GetLookingAtBlock().value();
      ImGui::Text("Looking At: %d %d %d", lookingAt.x, lookingAt.y, lookingAt.z);
    }
    ImGui::Text("");

    glm::ivec3 playerBlockPos = { floor(playerPos.x), floor(playerPos.y), floor(playerPos.z) };
    glm::ivec2 chunkCoord = world.GetChunkCoord(playerBlockPos.x, playerBlockPos.z);
    glm::ivec3 localCoords = world.ToLocalCoords(XYZ(playerBlockPos));

    ImGui::Text("Chunk: %d, %d", chunkCoord.x, chunkCoord.y);
    ImGui::Text("Local XYZ: %d / %d / %d", localCoords.x, localCoords.y, localCoords.z);
    ImGui::Text("Light level (S/B): %d / %d", world.GetLightAt(LightType::SKY, XYZ(playerBlockPos)), world.GetLightAt(LightType::BLOCK, XYZ(playerBlockPos)));

    ImGui::Text("");

    ImGui::Text("Chunks: %d", world.GetChunkCount());
    ImGui::Text("Terrain queue: %d", world.GetChunksToGenerateTerrainSize());
    ImGui::Text("Lighting queue: %d", world.GetChunksToLightSize());
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

    char blockstate = world.GetBlockstateAt(floor(playerPos.x), playerPos.y, floor(playerPos.z));
    std::string blockName = Block::FromBlockstate(blockstate).GetRegistryName();
    ImGui::Text("Blockstate: %d", blockstate);
    ImGui::Text("Inside Block: %s", blockName.c_str());

    ImGui::End();
    ImGui::PopFont();

    // Debug Panel
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Debug Panel");

    ImGui::BeginTabBar("DebugSettings");

    if (ImGui::BeginTabItem("World")) {

      ImGui::SliderFloat("FOV", &DebugSettings::instance.defaultFOV, 0.0f, 360.0f, "%.0fº");
      ImGui::SliderInt("Render distance", &DebugSettings::instance.renderDistance, 1, 64);
      ImGui::Checkbox("Night vision", &DebugSettings::instance.nightVision);
      ImGui::Checkbox("Night time", &DebugSettings::instance.nightTime);

      if (ImGui::Button(DebugSettings::instance.updateWorld ? "Stop updating world" : "Continue updating world")) {
        DebugSettings::instance.updateWorld = !DebugSettings::instance.updateWorld;
      }

      ImGui::Text("");
      ImGui::InputInt("Terrain workers", &DebugSettings::instance.terrainWorkerCount);
      ImGui::InputInt("Lighting workers", &DebugSettings::instance.lightingWorkerCount);
      ImGui::InputInt("Mesh workers", &DebugSettings::instance.meshWorkerCount);

      if (ImGui::Button("Reset workers")) {
        world.ResetWorkers();
      }

      ImGui::Text("");
      ImGui::Text("Debug");
      ImGui::Checkbox("Show chunk boundaries", &DebugSettings::instance.showChunkBoundaries);
      ImGui::Checkbox("Show player hitbox", &DebugSettings::instance.showPlayerHitbox);

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Movement")) {
      ImGui::InputDouble("Gravity", &DebugSettings::instance.gravity);
      ImGui::InputDouble("Jump force", &DebugSettings::instance.jumpForce);
      ImGui::InputDouble("Walk speed", &DebugSettings::instance.walkSpeed);

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Data")) {

      if (ImGui::Button("Reload shaders")) {
        ShaderLibrary::GetInstance().ReloadShaders();
      }

      if (ImGui::Button("Reload textures")) {
        Blocks::GenerateBlockAtlas();
        world.RemeshAllChunks();
      }

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Terrain generation")) {
      ImGui::InputDouble("Noise scale", &DebugSettings::instance.noiseScale);
      ImGui::InputFloat2("Noise offset", DebugSettings::instance.noiseOffsets);
      ImGui::Text("");
      ImGui::InputInt("Octaves", &DebugSettings::instance.octaves);
      ImGui::InputDouble("Persistence", &DebugSettings::instance.persistence);
      ImGui::InputDouble("Lacunarity", &DebugSettings::instance.lacunarity);
      ImGui::Text("");
      ImGui::InputInt("Base terrain height", &DebugSettings::instance.baseTerrainHeight);
      ImGui::InputInt2("Terrain range", DebugSettings::instance.terrainRange);

      ImGui::Separator();
      ImGui::InputDouble("Cave noise scale", &DebugSettings::instance.caveNoiseScale);
      ImGui::InputDouble("Cave threshold", &DebugSettings::instance.caveThreshold);
      ImGui::InputFloat3("Cave noise offset", DebugSettings::instance.caveNoiseOffsets);
      ImGui::Separator();
      ImGui::InputDouble("Coal noise scale", &DebugSettings::instance.coalScale);
      ImGui::InputDouble("Coal threshold", &DebugSettings::instance.coalThreshold);
      ImGui::InputDouble("Iron noise scale", &DebugSettings::instance.ironScale);
      ImGui::InputDouble("iron threshold", &DebugSettings::instance.ironThreshold);

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
  Input::SubscribeKeyCallback(GLFW_KEY_F3, [](int key, int action, int mods) {
    if (action == GLFW_PRESS) {
      Toggle();
    }
  });

  s_font = window.GetImGuiIO().Fonts->AddFontFromFileTTF((std::string(RESOURCES_PATH) + "fonts/Minecraft.ttf").c_str(), 20.0f, NULL);
}

void DebugInformation::Toggle() {
  s_showDebugInformation = !s_showDebugInformation;
}