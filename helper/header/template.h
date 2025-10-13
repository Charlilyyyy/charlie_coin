#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <iostream>
#include <utility> // for std::pair

// Template declaration — must be in header because templates need to be visible to all translation units
template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

#endif
