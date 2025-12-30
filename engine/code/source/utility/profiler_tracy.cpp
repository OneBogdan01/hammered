#include "utility/profiler_tracy.hpp"

#include "engine.hpp"
#include "core/ecs.hpp"
#include "core/renderer.hpp"
#include "core/logger.hpp"

#include <windows.h>

#include <string>
#include <vector>

// only this file uses these
namespace
{

hm::ProfilerSettings settings;
HANDLE capture_process = nullptr;

constexpr SDL_DialogFileFilter k_exe_filters[] = {{"Executable", "exe"},
                                                  {"All files", "*"}};

constexpr SDL_DialogFileFilter k_output_filters[] = {{"Tracy trace", "tracy"},
                                                     {"All files", "*"}};

enum class DialogType
{
  PROFILER,
  CAPTURE,
  OUTPUT
};
DialogType dialog = DialogType::PROFILER;

void launch_profiler()
{
  STARTUPINFOA si = {sizeof(si)};
  PROCESS_INFORMATION pi = {};

  std::string cmd = settings.profiler_path;
  cmd += " -a " + settings.address;
  cmd += " -p " + std::to_string(settings.port);

  std::vector buf(cmd.begin(), cmd.end());
  buf.push_back('\0');

  if (CreateProcessA(nullptr, buf.data(), nullptr, nullptr, FALSE, 0, nullptr,
                     nullptr, &si, &pi))
  {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    hm::log::Info("Launched Tracy profiler: {}", cmd);
  }
  else
  {
    hm::log::Error("Failed to launch Tracy profiler. Error code: {}",
                   GetLastError());
  }
}

void launch_capture()
{
  STARTUPINFOA si = {sizeof(si)};
  PROCESS_INFORMATION pi = {};

  std::string cmd = settings.capture_path;
  cmd += " -o " + settings.output_path;
  cmd += " -a " + settings.address;
  cmd += " -p " + std::to_string(settings.port);
  if (settings.overwrite)
  {
    cmd += " -f";
  }
  if (settings.use_timer)
  {
    cmd += " -s " + std::to_string(settings.duration);
  }

  std::vector buf(cmd.begin(), cmd.end());
  buf.push_back('\0');

  if (CreateProcessA(nullptr, buf.data(), nullptr, nullptr, FALSE, 0, nullptr,
                     nullptr, &si, &pi))
  {
    CloseHandle(pi.hThread);
    capture_process = pi.hProcess;
    hm::log::Info("Launched Tracy capture: {}", cmd);
  }
  else
  {
    hm::log::Error("Failed to launch Tracy capture. Error code: {}",
                   GetLastError());
  }
}

void stop_capture()
{
  if (capture_process)
  {
    TerminateProcess(capture_process, 0);
    CloseHandle(capture_process);
    capture_process = nullptr;
    hm::log::Info("Stopped Tracy capture");
  }
}

bool is_capture_running()
{
  if (!capture_process)
    return false;
  DWORD exit_code;
  if (GetExitCodeProcess(capture_process, &exit_code))
  {
    if (exit_code != STILL_ACTIVE)
    {
      CloseHandle(capture_process);
      capture_process = nullptr;
      return false;
    }
    return true;
  }
  return false;
}
bool InputTextString(const char* label, std::string& str, size_t max_size = 256)
{
  std::vector<char> buf(max_size);
  strncpy_s(buf.data(), buf.size(), str.c_str(), _TRUNCATE);

  if (ImGui::InputText(label, buf.data(), buf.size()))
  {
    str = buf.data();
    return true;
  }
  return false;
}
void SDLCALL on_file_selected(void* userdata, const char* const* filelist,
                              int filter)
{
  (void)userdata;
  (void)filter;

  if (!filelist || !*filelist)
    return;

  switch (dialog)
  {
    case DialogType::PROFILER:
      settings.profiler_path = *filelist;
      hm::log::Info("Selected profiler: {}", settings.profiler_path);
      break;
    case DialogType::CAPTURE:
      settings.capture_path = *filelist;
      hm::log::Info("Selected capture: {}", settings.capture_path);
      break;
    case DialogType::OUTPUT:
      settings.output_path = *filelist;
      hm::log::Info("Selected output: {}", settings.output_path);
      break;
  }
}

} // namespace

void hm::Profiler::display()
{
  if (ImGui::Begin("Profiler Window"))
  {
    InputTextString("Address", settings.address);
    ImGui::InputInt("Port", &settings.port);

    ImGui::Separator();
    if (ImGui::Button("Start test"))
    {
      launch_capture();

      {
        hm::Engine::Instance()
            .GetECS()
            .GetSystem<hm::gpx::Renderer>()
            .load_test_model();
      }
    }
    if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_DefaultOpen))
    {
      InputTextString("Path##profiler", settings.profiler_path);
      ImGui::SameLine();
      if (ImGui::Button("Browse##profiler"))
      {
        dialog = DialogType::PROFILER;
        SDL_ShowOpenFileDialog(on_file_selected, nullptr, nullptr,
                               k_exe_filters, SDL_arraysize(k_exe_filters),
                               nullptr, false);
      }

      if (ImGui::Button("Launch Profiler"))
      {
        launch_profiler();
      }
    }

    if (ImGui::CollapsingHeader("Capture", ImGuiTreeNodeFlags_DefaultOpen))
    {
      InputTextString("Path##capture", settings.capture_path);
      ImGui::SameLine();
      if (ImGui::Button("Browse##capture"))
      {
        dialog = DialogType::CAPTURE;
        SDL_ShowOpenFileDialog(on_file_selected, nullptr, nullptr,
                               k_exe_filters, SDL_arraysize(k_exe_filters),
                               nullptr, false);
      }

      InputTextString("Output File", settings.output_path);
      ImGui::SameLine();
      if (ImGui::Button("Browse##output"))
      {
        dialog = DialogType::OUTPUT;
        SDL_ShowSaveFileDialog(on_file_selected, nullptr, nullptr,
                               k_output_filters,
                               SDL_arraysize(k_output_filters), nullptr);
      }

      ImGui::Checkbox("Overwrite existing", &settings.overwrite);

      ImGui::Checkbox("Timed Capture", &settings.use_timer);
      if (settings.use_timer)
      {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("seconds", &settings.duration);
        settings.duration = std::max(1, settings.duration);
      }

      bool running = is_capture_running();
      if (running)
      {
        if (ImGui::Button("Stop Capture"))
        {
          stop_capture();
        }
      }
      else
      {
        if (ImGui::Button("Start Capture"))
        {
          launch_capture();
        }
      }
    }
  }
  ImGui::End();
}
