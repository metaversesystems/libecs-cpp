#pragma once

#include <string>
#include <queue>

namespace ecs
{
    class Container;

    class System
    {
      public:
        System();
        System(std::string Handle);
        virtual void Init();
        void ContainerSet(ecs::Container *Container);
        ecs::Container *ContainerGet();
        std::string HandleGet();
        virtual void Update() {};
        std::string Handle;
        ecs::Container *Container = nullptr;
        ecs::TypeEntityComponentList ComponentsGet();
        void ComponentRequest(std::string);
        void MessageSubmit(Json::Value);
        virtual Json::Value Export() = 0;
      protected:
        std::vector<std::string> RequestedComponents;
        std::queue<Json::Value> messages;
        std::chrono::steady_clock::time_point LastTime = std::chrono::steady_clock::now();
        uint32_t DeltaTimeGet();
    };
}
