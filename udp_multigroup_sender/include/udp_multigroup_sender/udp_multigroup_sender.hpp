#ifndef __UDP_MULTIGROUP_SENDER_HPP__
#define __UDP_MULTIGROUP_SENDER_HPP__
#include <string.h>
#include "ros/ros.h"
#include <string>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <iostream>
#include <sys/socket.h> // for socket connect send recv
#include <sys/types.h>
#include <unistd.h> // for sleep


namespace ars548_scan {
constexpr char *SOURCE_IP = "10.13.1.113";
constexpr char *DEST_IP = "224.0.2.2";

constexpr char *group_ip = "224.0.2.2";
constexpr char *local_ip = "10.13.1.110";
constexpr char *nic_card = "enp0s31f6";

constexpr int dest_port = 42101;
constexpr int local_port = 42401;

class UdpMultigroupSender {
public:
 
  UdpMultigroupSender()
      :  multi_cast_ip_(group_ip), multi_cast_port_(dest_port),
        multi_cast_ttl_(1), initial_flag_(false), socket_(-1) {
    // create socket for sending datagrams
    socket_ = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_ < 0) {
      perror("socket: ");
      return;
    }

    memset(&multi_cast_addr_, 0, sizeof(multi_cast_addr_));
    multi_cast_addr_.sin_family = AF_INET;
    multi_cast_addr_.sin_addr.s_addr = inet_addr(multi_cast_ip_);
    multi_cast_addr_.sin_port = htons(multi_cast_port_);

    if (setsockopt(socket_, IPPROTO_IP, IP_DEFAULT_MULTICAST_TTL,
                   &multi_cast_ttl_, sizeof(multi_cast_ttl_)) < 0) {

      perror("setting IP_MULTICAST_TTL:");
      close(socket_);
      return;
    }

    bzero(&ifr_, sizeof(ifr_));
    strncpy(ifr_.ifr_ifrn.ifrn_name, nic_card, IFNAMSIZ - 1);
    if (setsockopt(socket_, IPPROTO_IP, SO_BINDTODEVICE, (char *)&ifr_,
                   sizeof(ifr_)) < 0) {
      perror("bind to net card: ");
      exit(1);
    }
    local_ip_ = (char *)malloc(22 * sizeof(char));
    bindToNetCard(nic_card, local_ip_);

    bzero(&local_addr_, sizeof(local_addr_));
    local_addr_.sin_family = AF_INET;
    // local_addr_.sin_addr.s_addr = inet_addr(local_ip_);
    local_addr_.sin_port = htons(local_port);
    int bind_src_port = -1;
    if ((bind_src_port = bind(socket_, (struct sockaddr *)&local_addr_,
                              sizeof(local_addr_))) < 0) {
      printf("Bind error: %d\n", bind_src_port);
      exit(1);
    }
    // // initialSendArs548();
    local_interface_.s_addr = inet_addr(local_ip_);
    if (setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_IF,
                   (char *)&local_interface_, sizeof(local_interface_))) {
      perror("setting local interface");
      exit(1);
    }
    initial_flag_ = true;
  }
  ~UdpMultigroupSender() {
    if (local_ip_)
      free(local_ip_);

    close(socket_);
  }

  bool sendMessage2Ars548(std::string &msgs);

private:
  bool initialSendArs548();
  void bindToNetCard(char *my_card, char *my_ip_port);
  void dieWithError(char *errorMessage);
  bool initial_flag_;

  int socket_;
  struct sockaddr_in multi_cast_addr_, local_addr_;
  char multi_cast_ttl_;
  unsigned short multi_cast_port_;
  char *multi_cast_ip_;
  char *local_ip_;
  struct in_addr local_interface_;
  struct ip_mreq group_;
  struct ifreq ifr_;
};

} // namespace ars548_scan

#endif /* F1DA49F2_5F4A_4073_8073_F5688655BA50 */
