#include "ros/ros.h"

#include "sensor_msgs/Joy.h"

#include <iostream>
#include <boost/asio.hpp>


boost::asio::io_service io_service;
boost::asio::ip::udp::socket sck(io_service);
std::string remote_server = "10.42.0.1";
int port = 5001;

void remote_send(char *data, int len)
{
    boost::asio::ip::udp::endpoint destination(boost::asio::ip::address::from_string(remote_server), port);
//    ROS_INFO("Sending: ", data);
    sck.send_to(boost::asio::buffer(data, len), destination);
}

void remote_callback(const sensor_msgs::Joy joy)
{
//    ROS_INFO("Got into cb");
    char data[10];
    sprintf(data, "st;%1.04f", joy.axes[0]);
    remote_send(data, 9);
    sprintf(data, "th;%1.04f", joy.axes[1]);
    remote_send(data, 9);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "remote_udp_emitter");
    ros::NodeHandle n;
    ros::NodeHandle pH("~remote");

//    ROS_INFO("openning socket");
    sck.open(boost::asio::ip::udp::v4());

    ros::Subscriber remote = n.subscribe("/remote", 1, remote_callback);   

    // Handle ROS communication events
    ros::spin();

    return 0;
}
