#pragma once

#define __STR(x) #x
#define STR(x) __STR(x)

#ifndef LABELQ_VERSION
# define LABELQ_VERSION_MAJOR 0
# define LABELQ_VERSION_MINOR 1
# define LABELQ_VERSION_PATCH 0
#endif

#define LABELQ_VERSION STR(LABELQ_VERSION_MAJOR) "." STR(LABELQ_VERSION_MINOR) "." STR(LABELQ_VERSION_PATCH)

template <class T>
class reversed {
    T _obj;

public:
    explicit inline reversed(const T &obj) : _obj(obj) {}

    inline auto begin() { return _obj.rbegin(); }
    inline auto end() { return _obj.rend(); }
};

#define never() assert(false)
#define not_implemented() assert(false)
