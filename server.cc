// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "Util.h"
#include "HCI.h"
#include "SDP.h"
#include "RFCOMM.h"

#include <iostream>
#include <string>

static const bdaddr_t
  bdaddr_any({{ 0, 0, 0, 0, 0, 0 }}),
  bdaddr_local({{ 0, 0, 0, 0xff, 0xff, 0xff }});

int main(
  int argc,
  char **argv
) {

  HCI::LocalDevice
    local_device = HCI::LocalDevice();

  std::cout << "Local Device Name: " << local_device.get_name() << std::endl;
  std::cout << "Local Device Address: " << Util::get_bdaddr_str(local_device.get_address()) << std::endl;

  RFCOMM::Socket server;
  if (server.bind(local_device.get_address()) != 0) {
    std::cerr << "Failed to Bind the Address and a Channel." << std::endl;
    return EXIT_FAILURE;
  }
  if (server.listen() != 0) {
    std::cerr << "Failed to Start Listening the Socket." << std::endl;
    return EXIT_FAILURE;
  }

  SDP::Session
    session(&bdaddr_any, &bdaddr_local);
  const uint8_t
    svc_uuid_int[] = {
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0xAB, 0xCD
    };
  session.register_service(
    svc_uuid_int,
    server.get_bind_channel(),
    "RFCOMM Server",
    "Kazumasa Kohtaka <kkohtaka@gmail.com>",
    "A sample implementation of RFCOMM server"
  );

  server.accept([] (
      int client_socket,
      struct ::sockaddr_rc client_address) {

    // Print the address of the client.
    char
      bdaddr_str[18];
    ::ba2str(&client_address.rc_bdaddr, bdaddr_str);
    std::cout << "Remote Device Address: " << bdaddr_str << std::endl;

    // Read data from the client.
    char
      message[1024] = { 0 };
    int
      nread = ::read(client_socket, message, sizeof(message));
    if (nread >= 0) {
      std::cout << "Message from Client: " << message << std::endl;
    } else {
      std::perror("read");
    }

    // Close the connection.
    ::close(client_socket);
  });

  return EXIT_SUCCESS;
}

