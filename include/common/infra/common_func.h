#ifndef COMMON_FUNC_H
#define COMMON_FUNC_H
#include <common/config.h>
#include <sstream>
namespace
{
    template<typename T>
    inline str_t X_MAKESTR(const T &val) {
        std::stringstream sin;
        sin << val;
        return sin.str();

    }
}

#endif // COMMON_H

