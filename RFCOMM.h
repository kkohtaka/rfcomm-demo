// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef __RFCOMM_H__
#define __RFCOMM_H__

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <string>
#include <functional>

namespace RFCOMM {

class Socket {

 private:
  int socket_;
  uint8_t channel_;

 public:
  Socket(void);
  ~Socket(void);
  uint8_t get_bind_channel(void);
  int bind(::bdaddr_t bdaddr, uint8_t channel);
  int bind(::bdaddr_t bdaddr);
  int listen(void);
  void accept(
    std::function<void(
      int client_socket,
      struct ::sockaddr_rc client_address
    )> callback
  );
  int connect(::bdaddr_t bdaddr, int channel);
  ssize_t write(const std::string &message);
};

}

#endif // __RFCOMM_H__
