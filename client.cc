// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "Util.h"
#include "HCI.h"
#include "SDP.h"
#include "RFCOMM.h"

#include <iostream>
#include <string>
#include <vector>

int main(
  int argc,
  char **argv
) {

  int
    dev_id = -1;

  // If an argument is passed, use it as a local device address.
  if (argc > 1) {
    dev_id = HCI::get_device_id(argv[1]);
  }

  // Get a local Bluetooth device.
  HCI::LocalDevice
    local_device = (dev_id < 0) ? HCI::LocalDevice() : HCI::LocalDevice(dev_id);
  dev_id = local_device.get_dev_id();

  ::bdaddr_t
    local_bdaddr = local_device.get_address();

  std::cout << "Local Device Name: " << local_device.get_name() << std::endl;
  std::cout << "Local Device Address: " << Util::get_bdaddr_str(local_bdaddr) << std::endl;

  // Open a Bluetooth socket to communicate with remote devices.
  HCI::RemoteSocket remote_socket(dev_id);
  if (remote_socket.get_socket() < 0) {
    std::cerr << "Failed to Open a Bluetooth Socket." << std::endl;
    return EXIT_FAILURE;
  }

  // Search remote devices.
  HCI::InquiryInfo inquiryInfo(dev_id);
  auto devices = inquiryInfo.get_devices();
  if (devices.empty()) {
    std::cerr << "No Available Remote Devices." << std::endl;
    return EXIT_FAILURE;
  }

  uint8_t
    svc_uuid_int[16] = {
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0xAB, 0xCD
    };
  SDP::UUID uuid(svc_uuid_int);
  std::cout << "Searcing a Service with UUID.: " << uuid.get_uuid_str() << std::endl;

  for (auto device : devices) {
    char bdaddr_str[18];
    ::ba2str(&device.bdaddr, bdaddr_str);
    std::cout << "Searching on a Remote Device.:" << bdaddr_str << std::endl;

    // Open a SDP session and search a service with the UUID.
    SDP::Session session(&local_bdaddr, &device.bdaddr);
    int channel = session.search_service_channel(uuid.get_uuid());

    if (channel == 0) {
      std::cout << "No Available Service is Found." << std::endl;
    } else {
      std::cout << "Available Service is Hosted on Channel.:" << channel << std::endl;

      // Open a RFCOMM connection and send a message.
      RFCOMM::Socket rfcomm_server;
      int
        status = rfcomm_server.connect(device.bdaddr, channel);
      if (status == 0) {
        rfcomm_server.write("Hello, RFCOMM.");
      } else {
        std::cerr << "Failed to Write a Message to the Socket." << std::endl;
      }
    }
  }

  return EXIT_SUCCESS;
}

