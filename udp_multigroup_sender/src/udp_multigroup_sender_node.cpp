

#include "udp_multigroup_sender/udp_multigroup_sender.hpp"

namespace ars548_scan
{

void UdpMultigroupSender::dieWithError(char* msgs)
{
  printf("dead:  %s", msgs);
}

bool UdpMultigroupSender::initialSendArs548()
{
  memset(&local_addr_, 0, sizeof(local_addr_));
  local_addr_.sin_family = AF_INET;
  local_addr_.sin_addr.s_addr = inet_addr(local_ip);
  local_addr_.sin_port = local_port;
  bind(socket_, (struct sockaddr*)&local_addr_, sizeof(local_addr_));
}

void UdpMultigroupSender::bindToNetCard(char* my_card, char* my_ip_port)
{
  int fd;
  // my_ip_port[0] = 0;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;

  strncpy(ifr.ifr_name, my_card, IFNAMSIZ - 1);
  ioctl(socket_, SIOCGIFADDR, &ifr);

  /* display result */
  sprintf(my_ip_port, "%s", inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
  printf("MY IP address:%s: on port: %d\n", my_ip_port, local_port);
  close(fd);
}

bool UdpMultigroupSender::sendMessage2Ars548(std::string& msgs)
{
  if ((msgs.size() == 0) || !initial_flag_)
  {
    ROS_INFO("msgs size == 0 or initial flag is false");
    return false;
  }

  std::string cov_str = msgs;

  int cov_str_size = cov_str.size();
  for (;;)
  {
    if (sendto(socket_, cov_str.c_str(), cov_str_size, 0, (struct sockaddr*)&multi_cast_addr_,
               sizeof(multi_cast_addr_)) != cov_str_size)
    {
      dieWithError("Send to send a differenct number of bytes than expected.");
      sleep(3);
      return false;
    }
    break;
  }
  ROS_INFO("Send ok.");
  return true;
}
}  // namespace ars548_scan
int main(int argc, char** argv)
{
  ros::init(argc, argv, "ars548_scan");

  ars548_scan::UdpMultigroupSender sender;

  // sender.initialSendArs548();

  ros::NodeHandle nh("~");

  ros::Rate r(20);

  while (nh.ok())
  {

    std::string msg =  "from ars548_scan multi_group\n";
    sender.sendMessage2Ars548(msg);

    r.sleep();
    ros::spinOnce();
  }

  ros::spin();

  return 0;
}