#pragma once
#include "json.hpp"

#define ODYSSEY_SERIALIZE(Type, ...)  \
    friend void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }
using namespace nlohmann;

namespace nlohmann
{
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>>
    {
        static void to_json(json& j, const std::shared_ptr<T>& opt)
        {
            if (opt)
            {
                j = *opt;
            }
            else
            {
                j = nullptr;
            }
        }

        static void from_json(const json& j, std::shared_ptr<T>& opt)
        {
            if (j.is_null())
            {
                opt = nullptr;
            }
            else
            {
                opt.reset(new T(j.get<T>()));
            }
        }
    };

    template <typename T>
    struct adl_serializer<std::unique_ptr<T>>
    {
        static void to_json(json& j, const std::unique_ptr<T>& opt)
        {
            if (opt)
            {
                j = *opt;
            }
            else
            {
                j = nullptr;
            }
        }

        // this is the overload needed for non-copyable types,
        static std::unique_ptr<T> from_json(const json& j)
        {
            if (j.is_null())
            {
                return nullptr;
            }
            else
            {
                return std::unique_ptr<T>(new T(j.get<T>()));
            }
        }
    };
}