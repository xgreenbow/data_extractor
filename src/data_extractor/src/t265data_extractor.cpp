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
int count=0;
void callback_stereo(const sensor_msgs::ImageConstPtr &msg1, const sensor_msgs::ImageConstPtr &msg2);
void callback_mono(const sensor_msgs::ImageConstPtr &msg);
int main(int argc,char **argv){
    ros::init(argc,argv,"run_extractor");
    ros::NodeHandle nh("~");
    std::string topic_fisheye1,topic_fisheye2;
    nh.param<std::string>("topic_fisheye1",topic_fisheye1,"/camera/fisheye1/image_raw");
    nh.param<std::string>("topic_fisheye2",topic_fisheye2,"/camera/fisheye2/image_raw");
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    std::vector<std::unique_ptr<message_filters::Synchronizer<sync_pol>>> sync_fisheye;
    std::vector<std::unique_ptr<message_filters::Subscriber<sensor_msgs::Image>>> sync_subs_fisheye;
    auto image_sub1=std::unique_ptr<message_filters::Subscriber<sensor_msgs::Image>>(
        new message_filters::Subscriber<sensor_msgs::Image>(nh,topic_fisheye1,5)
    );
    auto image_sub2=std::unique_ptr<message_filters::Subscriber<sensor_msgs::Image>>(
        new message_filters::Subscriber<sensor_msgs::Image>(nh,topic_fisheye2,5)
    );
    auto sync=std::unique_ptr<message_filters::Synchronizer<sync_pol>>(
        new message_filters::Synchronizer<sync_pol>(sync_pol(5),*image_sub1,*image_sub2)
    );
    image_sub1->registerCallback(callback_mono);
    /*sync->registerCallback(boost::bind(&callback_stereo, _1, _2));
    sync_fisheye.push_back(std::move(sync));
    sync_subs_fisheye.push_back(std::move(image_sub1));
    sync_subs_fisheye.push_back(std::move(image_sub2));*/
    ROS_INFO("subscribing to cam (stereo): %s", topic_fisheye1.c_str());
    //ROS_INFO("subscribing to cam (stereo): %s", topic_fisheye2.c_str());
    ros::spin();
}
void callback_stereo(const sensor_msgs::ImageConstPtr &msg1, const sensor_msgs::ImageConstPtr &msg2){
    cv_bridge::CvImageConstPtr cv_ptr1;
  try {
    cv_ptr1 = cv_bridge::toCvShare(msg1, sensor_msgs::image_encodings::MONO8);
  } catch (cv_bridge::Exception &e) {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }

  // Get the image
  cv_bridge::CvImageConstPtr cv_ptr2;
  try {
    cv_ptr2 = cv_bridge::toCvShare(msg2, sensor_msgs::image_encodings::MONO8);
  } catch (cv_bridge::Exception &e) {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
  rosbag::Bag bag;
  bag.open("/shared/data/bags/test.bag",rosbag::bagmode::Write);
  bag.write("/camera/fisheye1/image_raw",ros::Time::now(),msg1);
  bag.write("/camera/fisheye1/image_raw",ros::Time::now(),msg2);
  //cv::imwrite("/shared/data/fisheye1_im"+std::to_string(count)+".jpg",cv_ptr1->image);
  //cv::imwrite("/shared/data/fisheye2_im"+std::to_string(count)+".jpg",cv_ptr2->image);
  count++;
}
void callback_mono(const sensor_msgs::ImageConstPtr &msg){
  rosbag::Bag bag;
  bag.open("/shared/data/bags/test.bag",rosbag::bagmode::Append);
  bag.write("/camera/fisheye1/image_raw",ros::Time::now(),msg);
}
