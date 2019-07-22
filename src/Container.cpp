#include <uuid/uuid.h>
#include <libecs-cpp/ecs.hpp>
#include <thread>
#include <chrono>

namespace ecs
{
    Container::Container()
    {
        uuid_t uuid;
        this->Handle.resize(40);

#ifdef _WIN32
        UuidCreate(&uuid);
        RPC_CSTR szUuid = NULL;
        if(UuidToString(&uuid, &szUuid) == RPC_S_OK)
        {
            this->Handle = (char*) szUuid;
            RpcStringFree(&szUuid);
        }
#else
        uuid_generate(uuid);
        uuid_unparse(uuid, &this->Handle[0]);
#endif
    }

    Container::Container(std::string Handle)
    {
        this->Handle = Handle;
    }

    void Container::Start()
    {
        this->ContainerThread = std::thread(&Container::ThreadFunc, this);
        this->ContainerThread.detach();
    }

    void Container::Start(uint32_t interval)
    {
        this->sleep_interval = interval;
        this->ContainerThread = std::thread(&Container::ThreadFunc, this);
        this->ContainerThread.detach();
    }

    Json::Value Container::save()
    {
        Json::Value data;

        data["handle"] = this->Handle;

        for(auto &e : this->Entities)
        {
            ecs::Entity *entity = this->Entities[e.first];

            data["entities"][e.first] = entity->save();
        }

        return data;
    }

    void Container::ManagerSet(ecs::Manager *Manager)
    {
        this->Manager = Manager;
    }

    ecs::Manager *Container::ManagerGet()
    {
        return this->Manager;
    }

    std::string Container::HandleGet()
    {
        return this->Handle;
    }

    ecs::System *Container::System(ecs::System *system)
    {
        system->ContainerSet(this);
        this->Systems[system->HandleGet()] = system;
        return system;
    }

    std::vector<std::string> Container::SystemsGet()
    {
        std::vector<std::string> Handles;

        for(auto &system : this->Systems)
            Handles.push_back(system.first);

        return Handles;
    }

    ecs::Component *Container::Component(ecs::Component *c)
    {
        this->Components[c->Type].push_back(c); 
        return c;
    }

    std::map<std::string, ecs::ComponentList> Container::ComponentsGet()
    {
        return this->Components;
    }

    std::map<std::string, ecs::ComponentList> Container::ComponentsGet(std::vector<std::string> Types)
    {
        std::map<std::string, ecs::ComponentList> c;

        for(auto &t : Types) c[t] = this->Components[t];
        return c;
    }

    ecs::Entity *Container::Entity(std::string Handle)
    {
        return this->EntityCreate(Handle);
    }

    ecs::Entity *Container::Entity()
    {
        return this->EntityCreate("");
    }

    ecs::Entity *Container::EntityCreate(std::string Handle)
    {
        ecs::Entity *e;
        if(Handle.size() == 0)
        {
            e = new ecs::Entity();
            this->Entities[e->HandleGet()] = e;
        }
        else
        {
            if(this->Entities.count(Handle) == 0)
            {
                e = new ecs::Entity(Handle);
                this->Entities[Handle] = e;
            }
            else e = this->Entities[Handle];
        }

        e->ContainerSet(this);
        return e;
    }

    void Container::SystemsInit()
    {
        for(auto &Handle : this->SystemsGet())
        {
            this->Systems[Handle]->Init();
        }
    }

    void Container::ThreadFunc()
    {
        this->SystemsInit();

        while(this->ThreadRunning)
        {
            usleep(this->sleep_interval);	
            this->Update();
        }
    }

    void Container::Update()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        uint32_t dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->LastTime).count();
        this->LastTime = now;

        for(auto &Handle : this->SystemsGet())
        {
            this->Systems[Handle]->Update(dt);
        }
    }

    void Container::MessageSubmit(Json::Value message)
    {
        auto dest_system = message["destination"]["system"].asString();
        if(!this->Systems[dest_system])
        {
            auto err = "ecs::Container(\"" + message["destination"]["container"].asString() + "\")::MessageSubmit(): System " + dest_system + " not found.";
            throw std::runtime_error(err);
        }

        this->Systems[dest_system]->MessageSubmit(message);
    }
}
