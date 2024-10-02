#ifndef FOCUSER_REMOTE_UTILS
#define FOCUSER_REMOTE_UTILS
#include <functional>


template<typename F, typename ...Args> void optionalCall(const F &f, Args&&... args) {
    if(f) f(std::forward<Args>(args)...);
}
#endif

