#include "lib.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>


int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[])
{
    try
    {
        std::vector<std::vector<int>> ip_pool;
#ifdef _WIN32
		// TODO read from file for windows platform
        std::string filepath;
        std::cout << "Put path to data file: ";
        std::cin >> filepath;
		std::ifstream myfile(filepath); //F:\CPP_Learning\CPP_Learning\Second\ip_filter.tsv
		if (myfile.is_open() && myfile.good())
		{
            for (std::string line; std::getline(myfile, line);)
			{
				std::vector<std::string> v = split(line, '\t');
                std::vector<std::string> ip = split(v.at(0), '.');
                std::vector<int> i_ip;
                for (const auto& it : ip)
                    i_ip.push_back(atoi(it.c_str()));
				ip_pool.push_back(i_ip);
	        }
			myfile.close();
        }
        else {
            std::cout << "Invalid filepath or invalid file. Check it!\n";
        }
#else
        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
			std::vector<std::string> ip = split(v.at(0), '.');
			std::vector<int> i_ip;
			for (const auto& it : ip)
				i_ip.push_back(atoi(it.c_str()));
			ip_pool.push_back(i_ip);
        }
#endif
		auto start = std::chrono::high_resolution_clock::now();
        lex_sort_pred pred(true);
        std::sort(ip_pool.begin(), ip_pool.end(), pred);
        cout_ips(ip_pool);

		ip_filter filter(ip_pool);
        std::cout << "filter by first byte search 1 and output" << std::endl;
        cout_ips(filter.filtering(1));
        std::cout << "filter by first and second bytes and output" << std::endl;
		cout_ips(filter.filtering(46, 70));
        std::cout << "filter by any byte and output" << std::endl;
		cout_ips(filter.filtering_any_place(46));

		auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Executed time is (microseconds): " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
