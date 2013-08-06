// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef __UTIL_H__
#define __UTIL_H__

#include <bluetooth/bluetooth.h>

#include <string>

namespace Util {

extern std::string get_bdaddr_str(const ::bdaddr_t &bdaddr);

};

#endif // __UTIL_H__

