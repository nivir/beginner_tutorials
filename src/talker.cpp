/******************************************************************************
 * MIT License

 * Copyright (c) 2019 Aman Virmani

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 ******************************************************************************/

/**
 * @file      talker.cpp
 * @author    Aman Virmani
 * @copyright MIT License
 * @brief     ROS publisher node implementation 
 */

#include <tf/transform_broadcaster.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "beginner_tutorials/modifyTalkerString.h"

/**
 * Default String published by Talker
 */
extern std::string message = "Written By Aman Virmani";

/**
 * @brief Function to call service
 * @param req Input String sent as input to service
 * @param res Response sent by service 
 * @return bool
 */
bool say(beginner_tutorials::modifyTalkerString::Request  &req,
         beginner_tutorials::modifyTalkerString::Response &res) {
  res.modifiedStr = req.inputStr;
  message = res.modifiedStr;
  ROS_INFO_STREAM("Default message by talker changed to: ");
  return true;
}


/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
/**
 * @brief      main function
 * @param      argc  The argc
 * @param      argv  The argv
 * @return     nothing
 */
int main(int argc, char **argv) {
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  // tf transform broadcaster object
  static tf::TransformBroadcaster br;
  tf::Transform transform;


  // variable to store loop frequency and default is set to 10Hz
  int talkerFrequency = 10;

  if (argc > 1) {
    // converting frequency input to int
    talkerFrequency = atoi(argv[1]);
  }

  if (talkerFrequency > 0) {
    ROS_DEBUG_STREAM("Talker publishing at frequency: "<< talkerFrequency);
  } else if (talkerFrequency < 0) {
    ROS_FATAL_STREAM("Talker expects positive value of frequency");

    ROS_WARN_STREAM("Talker frequency set to default value of 10Hz");

    // setting default value for talkerFrequency
    talkerFrequency = 10;
  } else if (talkerFrequency == 0) {
    ROS_ERROR_STREAM("Talker expects non-zero frequency");

    ROS_WARN_STREAM("Talker frequency set to default value of 10Hz");

    // setting default value for talkerFrequency
    talkerFrequency = 10;
}

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  auto chatter_pub = n.advertise < std_msgs::String > ("chatter", 1000);

  auto server = n.advertiseService("modifyTalkerMessage", say);

  ros::Rate loop_rate(talkerFrequency);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
  while (ros::ok()) {
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
    std_msgs::String msg;

    std::stringstream ss;
    ss << count << " " << message;
    msg.data = ss.str();

    ROS_INFO("%s", msg.data.c_str());

    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    chatter_pub.publish(msg);

    // set translation
    transform.setOrigin(tf::Vector3(0.0, 2.0, 0.0));
    // set rotation
    tf::Quaternion q;
    q.setRPY(0, 0, 1);
    transform.setRotation(q);

    // broadcast the transform
    br.sendTransform(
        tf::StampedTransform(transform, ros::Time::now(), "world", "talk"));

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }

  return 0;
}
