// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef __HCI_H__
#define __HCI_H__

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include <string>
#include <vector>

namespace HCI {

extern int get_device_id(const std::string &bdaddr_str);

class LocalDevice {

 private:
  int dev_id_;
  ::hci_dev_info dev_info_;

 public:
  LocalDevice(void);
  explicit LocalDevice(int dev_id);
  ~LocalDevice(void);
  int get_dev_id(void);
  const std::string get_name(void);
  const ::bdaddr_t get_address(void);
};

class RemoteSocket {

 private:
  int socket_;

 public:
  explicit RemoteSocket (int dev_id);
  ~RemoteSocket(void);
  int get_socket(void);
};

class InquiryInfo {

 private:
  const int len_;
  const int max_rsp_;
  const int flags_;
  inquiry_info *ii_;
  const int num_rsp_;
  std::vector< ::inquiry_info> devices_;

 public:
  explicit InquiryInfo(int dev_id);
  ~InquiryInfo(void);
  const std::vector< ::inquiry_info> get_devices(void);
};

};

#endif // __HCI_H__

