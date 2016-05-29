#include <ros/ros.h>
#include <tf/transform_datatypes.h>
#include <sensor_msgs/NavSatFix.h>
#include <nav_msgs/Path.h>

#include "kmlparser.h"
#include "wgs84.h"

class kml_path
{
private:
	ros::NodeHandle nh;
	ros::Subscriber sub_datum;
	ros::Publisher pub_path;

	std::vector<blh_t> blh_path;

	void cb_datum(const sensor_msgs::NavSatFix::Ptr &msg)
	{
		blh_t origin;
		origin.lat = msg->latitude;
		origin.lon = msg->longitude;
		origin.height = msg->altitude;

		nav_msgs::Path path;
		path.header.stamp = ros::Time::now();
		path.header.frame_id = "map";
		for(auto &blh: blh_path)
		{
			enu_t enu;
			blh2enu(&origin, &blh, &enu, 1);
			ROS_DEBUG("  %0.6f %0.6f", enu.x, enu.y);

			geometry_msgs::PoseStamped p;
			p.pose.position.x = enu.x;
			p.pose.position.y = enu.y;
			p.pose.orientation.w = 1.0;

			path.poses.push_back(p);
		}
		pub_path.publish(path);
	}
public:
	kml_path():
		nh("~")
	{
		sub_datum = nh.subscribe("/gps/enu_datum", 1, &kml_path::cb_datum, this);
		pub_path = nh.advertise<nav_msgs::Path>("/path", 1, true);

		std::string file;
		std::string folder;
		nh.param("file", file, std::string("a.kml"));
		nh.param("folder", folder, std::string(""));

		auto fp = kmlReadFile(file.c_str());
		xmlNode **folders;
		int num_folders = kmlGetFolders(fp, &folders);
		for(int i = 0; i < num_folders; i ++)
		{
			char name[512];
			kmlGetFolderName(folders[i], name);
			ROS_INFO("KML Folder found: %s", name);
			if(folder.compare(name) == 0)
			{
				blh_t **blh_list;
				int num = kmlGetLines(folders[i], &blh_list);
				ROS_INFO(" %d lines", num);
				for(int j = 0; ; j ++)
				{
					if(blh_list[0][j].height == 0.0 &&
							blh_list[0][j].lat == 0.0 &&
							blh_list[0][j].lon == 0.0)
						break;
					blh_path.push_back(blh_list[0][j]);
					ROS_INFO("  %0.6f %0.6f", blh_list[0][j].lat, blh_list[0][j].lon);
				}
				kmlFreeLines(&blh_list, num);
			}
		}
		kmlFreeFolders(&folders);
		kmlFree(fp);
	}
};

int main(int argc, char *argv[])
{
	ros::init(argc, argv, "kml_path");
	
	kml_path kp;
	ros::spin();

	return 0;
}


