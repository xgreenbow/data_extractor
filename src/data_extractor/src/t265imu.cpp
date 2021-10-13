#include <cv_bridge/cv_bridge.h>
#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/time_synchronizer.h>
#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory>
bool first=true;
void callback_inertial(const sensor_msgs::Imu::ConstPtr &msg);
int main(int argc, char**argv){
    ros::init(argc,argv,"run_imu_extractor");
    ros::NodeHandle nh("~");
    // Create imu subscriber
    std::string topic_imu;
    nh.param<std::string>("topic_imu", topic_imu, "/camera/imu");
    ros::Subscriber subimu = nh.subscribe(topic_imu, 9999999, callback_inertial);
    ROS_INFO("Subscribing to imu,%s",topic_imu.c_str());
    ROS_INFO("done...spinning to ros");
    ros::spin();
}
void callback_inertial(const sensor_msgs::Imu::ConstPtr &msg){
    rosbag::Bag bag;
    bag.open("/shared/data/imu_calib/t265/1/imu.bag",rosbag::bagmode::Append);
    bag.write("/camera/imu",ros::Time::now(),msg);
}