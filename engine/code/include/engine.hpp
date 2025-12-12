#pragma once
#include "core/thread_pool.hpp"

namespace hm
{
namespace ecs
{
class EntityComponentSystem;

}
namespace input
{
class Input;
}

class Device;

class Engine
{
 public:
  static Engine& Instance();

  /// <summary>
  /// Initializes core modules
  /// </summary>
  void Init();
  /// <summary>
  /// Starts the update loop
  /// </summary>
  SDL_AppResult Run();

  /// <summary>
  /// Cleans up resources and close window
  /// </summary>
  void Shutdown();

  Device& GetDevice() const { return *m_pDevice; }
  ecs::EntityComponentSystem& GetECS() { return *m_pEntityComponentSystem; };
  input::Input& GetInput() { return *m_pInput; };
  ThreadPool& GetThreadPool() const { return *m_pThreadPool; };

 private:
  Device* m_pDevice {nullptr};
  ecs::EntityComponentSystem* m_pEntityComponentSystem {nullptr};
  input::Input* m_pInput {nullptr};
  ThreadPool* m_pThreadPool {nullptr};
};
} // namespace hm
