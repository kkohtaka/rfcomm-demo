// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#ifndef __SDP_H__
#define __SDP_H__

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <string>

namespace SDP {

class UUID {

 private:
  ::uuid_t uuid_;

 public:
  UUID(const uint8_t uuid_int[16]);
  ~UUID(void);
  ::uuid_t get_uuid(void);
  const std::string get_uuid_str(void);
};

class Session {

 private:
  ::sdp_session_t *session_;

 public:
  Session(const ::bdaddr_t *src_bdaddr, const ::bdaddr_t *dest_bdaddr);
  ~Session(void);
  int register_service(
    const uint8_t svc_uuid_int[16],
    uint8_t rfcomm_channel,
    const std::string &service_name,
    const std::string &service_provider,
    const std::string &service_description
  );
  int search_service_channel(const ::uuid_t svc_uuid);
};

};

#endif // __SDP_H__
