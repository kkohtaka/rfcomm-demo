// Copyright (c) 2013 Kazumasa Kohtaka. All rights reserved.
// This file is available under the MIT license.

#include "SDP.h"

#include <iostream>

namespace SDP {

UUID::UUID(
  const uint8_t uuid_int[16]
) : uuid_() {

  ::sdp_uuid128_create(&uuid_, uuid_int);
}

UUID::~UUID(void) {}

::uuid_t UUID::get_uuid(void) {

  return uuid_;
}

const std::string UUID::get_uuid_str(void) {

  char uuid_string[64];
  ::sdp_uuid2strn(&uuid_, uuid_string, sizeof(uuid_string));
  return std::string(uuid_string);
}

Session::Session(
  const ::bdaddr_t *src_bdaddr,
  const ::bdaddr_t *dest_bdaddr
) : session_(::sdp_connect(src_bdaddr, dest_bdaddr, SDP_RETRY_IF_BUSY)) {}

Session::~Session(void) {

  ::sdp_close(session_);
}

int Session::register_service(
  const uint8_t svc_uuid_int[16],
  uint8_t rfcomm_channel,
  const std::string &service_name,
  const std::string &service_provider,
  const std::string &service_description
) {

  ::sdp_record_t
    *record = ::sdp_record_alloc();

  // Set the General Service ID.
  char uuid_str[256];
  ::uuid_t
    svc_uuid;
  ::sdp_uuid128_create(&svc_uuid, &svc_uuid_int[0]);
  ::sdp_set_service_id(record, svc_uuid);
  ::sdp_uuid2strn(&svc_uuid, uuid_str, sizeof(uuid_str));

  // Make the Service Record Publicly Browsable.
  ::uuid_t
    root_uuid;
  ::sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
  ::sdp_list_t
    *root_list = ::sdp_list_append(0, &root_uuid);
  ::sdp_set_browse_groups(record, root_list);

  // Set RFCOMM Information.
  ::uuid_t
    rfcomm_uuid;
  ::sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
  ::sdp_data_t
    *channel = ::sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
  ::sdp_list_t
    *rfcomm_list = ::sdp_list_append(0, &rfcomm_uuid);
  ::sdp_list_append(rfcomm_list, channel);

  // Attach Protocol Information to Service Record.
  ::sdp_list_t
    *proto_list = ::sdp_list_append(0, rfcomm_list);
  ::sdp_list_t
    *access_proto_list = ::sdp_list_append(0, proto_list);
  ::sdp_set_access_protos(record, access_proto_list);

  // Set the Name, Provider, and Description.
  ::sdp_set_info_attr(
    record,
    service_name.c_str(),
    service_provider.c_str(),
    service_description.c_str()
  );

  int
    error = ::sdp_record_register(session_, record, 0);

  if (error < 0) {
    std::perror("sdp_record_register");
  }

  // Clean up resources.
  ::sdp_list_free(root_list, 0);
  ::sdp_list_free(access_proto_list, 0);
  ::sdp_list_free(rfcomm_list, 0);
  ::sdp_data_free(channel);

  return error;
}

int Session::search_service_channel(
  const ::uuid_t svc_uuid
) {

  int channel = 0;

  ::sdp_list_t
    *search_list = ::sdp_list_append(nullptr, const_cast< ::uuid_t *>(&svc_uuid));

  // Specify that we want a list of all the matching applications' attributes
  uint32_t
    range = 0x0000ffff;
  ::sdp_list_t
    *attrid_list = ::sdp_list_append(nullptr, &range);

  // Get a list of service records that have UUID 0xabcd
  ::sdp_list_t
    *response_list = nullptr;
  int
    status = ::sdp_service_search_attr_req(
      session_,
      search_list,
      SDP_ATTR_REQ_RANGE,
      attrid_list,
      &response_list
    );

  if (status == 0) {
    ::sdp_list_t *r = response_list;

    for (; r; r = r->next) {
      ::sdp_record_t *rec = reinterpret_cast< ::sdp_record_t *>(r->data);
      ::sdp_list_t *proto_list = nullptr;

      if (channel == 0 && ::sdp_get_access_protos(rec, &proto_list) == 0) {
        channel = sdp_get_proto_port(proto_list, RFCOMM_UUID);
        ::sdp_list_free(proto_list, 0);
      }
      ::sdp_record_free(rec);
    }
  } else {
    std::perror("sdp_service_search_attr_req");
  }

  return channel;
}

};

