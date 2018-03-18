#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <math.h>
#include <iostream>
#include <random>
#include <chrono>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#define PI 3.14159265


struct bot_info {
  int bot_id,change_direction_counter;
  float location_x,location_y,velocityangle,velocity;
} ;

bot_info bot;
void initialvals() {
  bot.bot_id=1;
  bot.location_x=5;
  bot.location_y=-5;
  bot.velocityangle=0;
  bot.velocity=1;
  bot.change_direction_counter=0;
}

float random_sd() {
 return .5 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(3-.5)));
}

float normal_random(float mean){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::normal_distribution<double> distribution (mean,random_sd());
   float number=distribution(generator);
if (number >-1*PI && number<PI) {
  return distribution(generator) ;
}
else
normal_random(mean);
}

void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  std_msgs::String pub_str;
  std::stringstream ss;
  ss <<msg->data.c_str();
  pub_str.data=ss.str();
  using namespace boost::algorithm;
  std::string str = pub_str.data.c_str();
  std::vector<std::string> tokens;
  split(tokens, str, is_any_of(" "));
  if (tokens[0]=="stop") {
    if (tokens[1]=="1") {
      bot.velocity=0;
    }
  }
  if (tokens[0]=="change_direction") {
    if (tokens[1]=="1") {
      bot.change_direction_counter=0;
      float bot1_random=bot.velocity*normal_random(-1*PI/2);
      bot.velocityangle=bot1_random;
    }
  }

  if (tokens[0]=="start") {
    if (tokens[1]=="1") {
    bot.velocity=1;
    }
  }


}

int main(int argc, char **argv)
{
  initialvals();
  ros::init(argc, argv, "bot1");
  ros::NodeHandle bot_node;
  ros::Publisher bot_pub = bot_node.advertise<std_msgs::String>("bot_said", 1000);
  ros::Subscriber bot_sub = bot_node.subscribe("server_said", 1000, chatterCallback);
  ros::Rate loop_rate(10);

while (ros::ok()){

  if (bot.change_direction_counter==30) {
    bot.change_direction_counter=0;
    float bot_random=bot.velocity*normal_random(-1*PI/2);
    bot.velocityangle=bot_random;
    std::cout << "anglechange" << '\n';

  }
  bot.change_direction_counter++;
  bot.location_x=bot.location_x + (bot.velocity*sqrt(2))*cos(bot.velocityangle)*.1;
  bot.location_y=bot.location_y + (bot.velocity*sqrt(2))*sin(bot.velocityangle)*.1;
  std_msgs::String msg1;
  std::stringstream ss1;
  ss1 << "1" << " " <<bot.location_x <<" "<<bot.location_y;
  msg1.data = ss1.str();
  bot_pub.publish(msg1);
  ros::spinOnce();
  loop_rate.sleep();

}



return 0;
}
