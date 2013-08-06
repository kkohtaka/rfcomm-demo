// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "Util.h"

namespace Util {

std::string get_bdaddr_str(const ::bdaddr_t &bdaddr) {

  char
    bdaddr_str[18];
  ::ba2str(&bdaddr, bdaddr_str);

  return bdaddr_str;
}

};

