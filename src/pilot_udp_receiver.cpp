#include "ros/ros.h"

#include "sensor_msgs/Joy.h"

#include <iostream>
#include <boost/asio.hpp>


int port = 5001;
double steering, throttle = 0.0;

ros::Publisher pilot_publisher;

void on_data(char *data)
{
    float st = 0.0, th = 0.0;
    char mode[20];
//    ROS_INFO("data %s", data);
    sscanf(data, "%f;%f;%s", &st, &th, mode);
//    ROS_INFO("after sscanf %f %f %s", st, th, mode);

    sensor_msgs::Joy pilot;

    pilot.axes.resize(2);
    pilot.axes[0] = st;
    pilot.axes[1] = th;
    pilot.buttons.resize(1);
    pilot.buttons[0] = 0;

    if (strcmp("user", mode) != 0)
    { 
        pilot.buttons[0] = 1;
    }
//    ROS_INFO("Publish %f %f %d", pilot.axes[0], pilot.axes[1], pilot.buttons[0]);
    pilot_publisher.publish(pilot);
}

int main(int argc, char** argv)
{
    char rcv_data[32];
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "pilot_udp_receiver");
    ros::NodeHandle n;
    ros::NodeHandle pH("~pilot");

    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket sck(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));

    pilot_publisher = n.advertise<sensor_msgs::Joy>("/pilot", 1);

    while(ros::ok())
    {
        boost::asio::ip::udp::endpoint remote_endpoint;
        int sz = sck.receive_from(boost::asio::buffer(rcv_data), remote_endpoint);
        rcv_data[sz] = '\0';
        on_data(rcv_data);
//        ROS_INFO("Received: %d =  %s", sz, rcv_data);
        // Handle ROS communication events
        ros::spinOnce();
    }

    return 0;
}
