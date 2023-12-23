#include "lib.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "version.h"

int version() {
    return PROJECT_VERSION_PATCH;
}



std::vector<std::vector<int>> ip_filter::filtering(int first) {
	std::vector<std::vector<int>> ret_val;
	if (first < 0) {
		return ret_val;
	}
	for (const auto& ip : _ips) {
		if (ip.size() < 1) continue;
		auto f_it = std::find(ip.begin(), ip.end(), first);
		if (ip.end() != f_it && ip.begin() == f_it) {
			ret_val.push_back(ip);
		}
	}
	return ret_val;
}

std::vector<std::vector<int>> ip_filter::filtering(int first, int second) {
	std::vector<std::vector<int>> ret_val;
	if (first < 0 || second < 0) {
		return ret_val;
	}
	for (const auto& ip : _ips) {
		if (ip.size() < 2) continue;
		auto f_it = std::find(ip.begin(), ip.end(), first);
		if (ip.end() != f_it && ip.begin() == f_it && *(ip.begin() + 1) == second) {
			ret_val.push_back(ip);
		}
	}
	return ret_val;
}

std::vector<std::vector<int>> ip_filter::filtering_any_place(int first) {
	std::vector<std::vector<int>> ret_val;
	if (first < 0) {
		return ret_val;
	}
	for (const auto& ip : _ips) {
		if (ip.size() < 1) continue;
		if (ip.end() != std::find(ip.begin(), ip.end(), first)) {
			ret_val.push_back(ip);
		}
	}
	return ret_val;
}

bool ip_filter::filtering_exist(const std::vector<int>& serching_ip) {
	if (serching_ip.empty()) {
		return false;
	}
	return std::find(_ips.begin(), _ips.end(), serching_ip) != _ips.end();
}

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string& str, char d)
{
	std::vector<std::string> r;

	std::string::size_type start = 0;
	std::string::size_type stop = str.find_first_of(d);
	while (stop != std::string::npos)
	{
		r.push_back(str.substr(start, stop - start));

		start = stop + 1;
		stop = str.find_first_of(d, start);
	}

	r.push_back(str.substr(start));

	return r;
}

void cout_ips(const std::vector<std::vector<int>>& ip_pool) {
	for (const auto& ip : ip_pool)
	{
		bool first = true;
		for (const auto& ip_part : ip)
		{
			if (first) {
				first = false;
			}
			else {
				std::cout << ".";
			}
			std::cout << ip_part;
		}
		std::cout << std::endl;
		first = true;
	}
}