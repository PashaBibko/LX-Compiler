#ifndef UTIL_MAP_MANIP_H
#define UTIL_MAP_MANIP_H

// Map manipulation functions (for unordered_map)

#ifndef DONT_MAP_MANIP_UNORDERED_MAP
#define DONT_MAP_MANIP_UNORDERED_MAP false
#endif // DONT_MAP_MANIP_UNORDERED_MAP

#if DONT_MAP_MANIP_UNORDERED_MAP == false

// Includes relevant map libraries
#include <unordered_map>

template <typename IDENTIFIER, typename VALUE>
inline bool inMap(const std::unordered_map<IDENTIFIER, VALUE>& map, const IDENTIFIER& key)
{
    return map.find(key) != map.end();
}

template <typename IDENTIFIER, typename VALUE, typename FUNCTION>
inline void callFuncOnMap(const std::unordered_map<IDENTIFIER, VALUE>& map, const FUNCTION& func)
{
    for (auto& pair : map)
    {
        func(pair.first, pair.second);
    }
}

#endif // DONT_MAP_MANIP_UNORDERED_MAP

// Map manipulation functions (for map)

#ifndef DONT_MAP_MANIP_MAP
#define DONT_MAP_MANIP_MAP false
#endif // DONT_MAP_MANIP_MAP

#if DONT_MAP_MANIP_MAP == false

// Includes relevant map libraries
#include <map>

template <typename IDENTIFIER, typename VALUE>
inline bool inMap(const std::map<IDENTIFIER, VALUE>& map, const IDENTIFIER& key)
{
    return map.find(key) != map.end();
}

template <typename IDENTIFIER, typename VALUE, typename FUNCTION>
inline void callFuncOnMap(const std::map<IDENTIFIER, VALUE>& map, const FUNCTION& func)
{
    for (auto& pair : map)
    {
        func(pair.first, pair.second);
    }
}

#endif // DONT_MAP_MANIP_MAP

#endif // UTIL_MAP_MANIP_H