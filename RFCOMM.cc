#include "RFCOMM.h"

namespace RFCOMM {

Socket::Socket(void) :
  socket_(::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)),
  channel_(0) {

  if (socket_ < 0) {
    std::perror("socket");
  }
}

Socket::~Socket(void) {

  if (::close(socket_) < 0) {
    std::perror("close");
  }
}

uint8_t Socket::get_bind_channel(void) {

  return channel_;
}

int Socket::bind(::bdaddr_t bdaddr, uint8_t channel) {

  struct ::sockaddr_rc
    address = { 0 };

  address.rc_family = AF_BLUETOOTH;
  address.rc_bdaddr = bdaddr;
  address.rc_channel = channel;

  int status = ::bind(
    socket_,
    reinterpret_cast<struct ::sockaddr *>(&address),
    sizeof(address)
  );

  if (status == 0) {
    channel_ = channel;
  } else {
    std::perror("bind");
  }

  return status;
}

int Socket::bind(::bdaddr_t bdaddr) {

  int status = -1;
  for (uint8_t channel = 1; channel < 30; ++channel) {
    status = bind(bdaddr, channel);
    if (status == 0) {
      break;
    }
  }
  return status;
}

int Socket::listen(void) {

  int
    status = ::listen(socket_, 1);

  if (status < 0) {
    std::perror("listen");
  }

  return status;
}

void Socket::accept(
  std::function<void(
    int client_socket,
    struct ::sockaddr_rc client_address
  )> callback
) {

  struct ::sockaddr_rc
    address = { 0 };
  ::socklen_t
    addresslen = sizeof(address);
  int
    socket = ::accept(
      socket_,
      reinterpret_cast<struct ::sockaddr *>(&address),
      &addresslen
    );

  if (socket < 0) {
    std::perror("accept");
  }

  callback(socket, address);
}

int Socket::connect(::bdaddr_t bdaddr, int channel) {

  struct ::sockaddr_rc
    address = { 0 };
  address.rc_family = AF_BLUETOOTH;
  address.rc_bdaddr = bdaddr;
  address.rc_channel = channel;

  int
    status = ::connect(
      socket_,
      reinterpret_cast<struct sockaddr *>(&address),
      sizeof(address)
    );

  if (status < 0) {
    std::perror("connect");
  }

  return status;
}

ssize_t Socket::write(const std::string &message) {

  ssize_t
    nwrite = ::write(socket_, message.c_str(), message.length());

  if (nwrite < 0) {
    std::perror("write");
  }

  return nwrite;
}

}

