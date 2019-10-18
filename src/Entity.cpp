#include <uuid/uuid.h>
#include <cstring>
#include <libecs-cpp/ecs.hpp>
#include <iostream>

namespace ecs
{
    Entity::Entity()
    {
        this->Handle.resize(20);

#ifdef _WIN32
        UUID uuid;
        UuidCreate(&uuid);
        RPC_CSTR szUuid = NULL;
        if(UuidToString(&uuid, &szUuid) == RPC_S_OK)
        {
            this->Handle = (char*) szUuid;
            RpcStringFree(&szUuid);
        }
#else
        uuid_t uuid;
        uuid_generate(uuid);
        uuid_unparse(uuid, &this->Handle[0]);
#endif
    }

    Entity::Entity(std::string uuid)
    {
        this->Handle = uuid;
    }

    Json::Value Entity::save()
    {
        Json::Value config;

        config["Handle"] = this->HandleGet();
        for(auto &t : this->Components)
        {
            for(auto &c : t.second)
            {
                while(auto component = c.second.Pop())
                {
                    config["Components"][t.first].append(component->save()); 
                }
            }
        }

        return config;
    }

    void Entity::ContainerSet(ecs::Container *container)
    {
        this->Container = container;
    }

    ecs::TypeEntityComponentList Entity::ComponentsGet()
    {
        return this->Components;
    }

    ecs::TypeEntityComponentList Entity::ComponentGet(std::string Type)
    {
        ecs::TypeEntityComponentList result;
        result[Type] = this->Components[Type];

        return result;
    }

    std::shared_ptr<ecs::Component> Entity::Component(std::shared_ptr<ecs::Component> c)
    {
        c->EntityHandle = this->Handle;
        this->Components[c->Type][c->EntityHandle].Push(c);
        this->Container->Component(c);
        return c;
    }

    void Entity::destroy()
    {
        for(auto &t : this->Components)
        {
            for(auto &c : t.second) t.second.erase(c.first);
        }
        this->Container->EntityDestroy(this->Handle);
    }

    std::string Entity::HandleGet()
    {
        return this->Handle;
    }
}
