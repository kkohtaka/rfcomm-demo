// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "HCI.h"

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/hci_lib.h>

namespace HCI {

int get_device_id(const std::string &bdaddr_str) {

  return ::hci_devid(bdaddr_str.c_str());
}

static ::hci_dev_info get_device_info_(int dev_id) {

  ::hci_dev_info
    dev_info;

  if (::hci_devinfo(dev_id, &dev_info) < 0) {
    std::perror("hci_devinfo");
  }

  return dev_info;
}

LocalDevice::LocalDevice(void) :
  dev_id_(::hci_get_route(NULL)),
  dev_info_(get_device_info_(dev_id_)) {

  if (dev_id_ < 0) {
    std::perror("hci_get_route");
  }
}

LocalDevice::LocalDevice(int dev_id) :
  dev_id_(dev_id),
  dev_info_(get_device_info_(dev_id_)) {}

LocalDevice::~LocalDevice(void) {}

int LocalDevice::get_dev_id(void) {

  return dev_id_;
}

const std::string LocalDevice::get_name(void) {

  return dev_info_.name;
}

const ::bdaddr_t LocalDevice::get_address(void) {

  return dev_info_.bdaddr;
}

RemoteSocket::RemoteSocket(int dev_id) :
  socket_(::hci_open_dev(dev_id)) {

  if (socket_ < 0) {
    std::perror("hci_open_dev");
  }
}

RemoteSocket::~RemoteSocket(void) {

  if (::close(socket_) < 0) {
    std::perror("close");
  }
}

int RemoteSocket::get_socket(void) {

  return socket_;
}

InquiryInfo::InquiryInfo(int dev_id) :
  len_(8),
  max_rsp_(255),
  flags_(IREQ_CACHE_FLUSH),
  ii_(new ::inquiry_info[max_rsp_]),
  num_rsp_(::hci_inquiry(dev_id, len_, max_rsp_, NULL, &ii_, flags_)),
  devices_() {

  if (num_rsp_ < 0) {
    std::perror("hci_inquiry");
  }

  for (int i = 0; i < num_rsp_; ++i) {
    devices_.push_back(*(ii_ + i));
  }
  delete [] ii_;
}

InquiryInfo::~InquiryInfo(void) {}

const std::vector< ::inquiry_info> InquiryInfo::get_devices(void) {

  return devices_;
}

};

