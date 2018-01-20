#include<iostream>
#include<cmath>
#include<fstream>
#include<string>
#include<vector>
#include<cstdlib>
#include<future>
#include<functional>

struct config
{
	unsigned point_count, distance;
	int min_x, max_x, min_y, max_y;
};

class point
{
public:
	point(int x = 0, int y = 0) : x_axis(x), y_axis(y) {}
	int get_x() const { return x_axis; }
	int get_y() const { return y_axis; }
	std::vector<point> find_neighbourhood(const std::vector<point> &, const config &) const;
private:
	int x_axis, y_axis;
};

// function return all neighbourhood except equal himself
std::vector<point> point::find_neighbourhood(const std::vector<point> &all_point, const config &cfg) const
{
	std::vector<point> neighbourhood;
	for (auto &el : all_point)
	{
		double distance = std::sqrt(std::pow((el.get_x() - x_axis), 2) + std::pow((el.get_y() - y_axis), 2)); // find distance between 2 points
		if (distance > 0 && distance <= cfg.distance)
			neighbourhood.push_back(el);
	}
	return neighbourhood;
}

int main()
{
	config cfg;
	std::ifstream fcfg("config.txt");
	if (!fcfg.is_open()) //check for exist config file
	{
		std::cout << "Config file not found" << std::endl;
		return 1;
	}

	while (fcfg.good()) // read config
	{
		std::string cfg_line, par_name;
		while (par_name.empty() || *(par_name.end() - 1) != ':')
		{
			fcfg >> cfg_line;
			par_name += cfg_line;
		}
		if (par_name == "distance:") fcfg >> cfg.distance;
		else if (par_name == "xaxis:")
		{
			fcfg >> cfg.min_x;
			fcfg.ignore(1);
			fcfg >> cfg.max_x;
		}
		else if (par_name == "yaxis:")
		{
			fcfg >> cfg.min_y;
			fcfg.ignore(1);
			fcfg >> cfg.max_y;
		}
		else
		{
			std::cout << "Unknown param" << std::endl;
			return 2;
		}
	}
	std::cin >> cfg.point_count;

	std::vector<point> all_point;
	for (unsigned i = 0; i < cfg.point_count; ++i) //fill random point
	{
		all_point.emplace_back(rand() % (cfg.max_x - cfg.min_x) + cfg.min_x, rand() % (cfg.max_y - cfg.min_y) + cfg.min_y);
	}

	std::vector<std::future<std::vector<point> > > all_future;  //all returned from thread
	for (auto &el : all_point)
		all_future.push_back(std::async(std::mem_fn(&point::find_neighbourhood), &el, all_point, cfg));

	for (unsigned i = 0; i < all_point.size(); ++i)
	{
		std::vector<point> neighbourhood = all_future[i].get();
		if (!neighbourhood.empty())
		{
			std::cout << "Point (" << all_point[i].get_x() << ", " << all_point[i].get_y() << ") neighbourhoods: ";
			for (auto &el : neighbourhood)
				std::cout << "point (" << el.get_x() << ", " << el.get_y() << "); ";
		}
		else
			std::cout << "Point (" << all_point[i].get_x() << ", " << all_point[i].get_y() << ") hasn't neighbourhoods";
		std::cout << std::endl;
	}
	system("pause");
	return 0;
}