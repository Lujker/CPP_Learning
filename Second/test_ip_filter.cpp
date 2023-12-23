#define BOOST_TEST_MODULE test_ip_filter

#include "lib.h"
#include <boost/test/unit_test.hpp>

std::vector<std::vector<int>> g_source_test_data
{
	{84,42,249,4},
	{62,12,93,76},
	{77,93,192,43},
	{141,101,132,50},
	{89,201,7,164},
	{5,15,220,203},
	{185,69,186,227},
	{24,193,178,168},
	{65,26,31,140},
	{83,99,166,35},
	{103,254,155,74},
	{49,34,2,43},
	{91,142,208,226},
	{193,93,192,134},
	{188,68,0,249},
	{79,110,28,78},
	{71,211,2,230},
	{188,72,127,48},
	{185,217,68,209},
	{99,242,220,241},
	{5,62,159,148},
	{1,70,44,170},
	{191,23,199,126},
	{98,242,127,228},
	{59,99,48,248},
	{67,191,200,150},
	{213,6,149,66},
	{77,109,86,105},
	{123,30,177,138},
	{185,14,194,82},
	{39,46,86,85},
	{220,189,194,162},
	{5,189,200,97},
	{24,91,41,150},
	{209,197,20,206},
	{12,155,92,10},
	{12,155,92,10},
	{205,186,180,34},
	{46,161,60,35},
	{73,115,6,159},
	{222,82,198,61},
	{213,200,15,167},
	{5,101,220,79},
	{98,122,86,197},
	{14,231,215,204},
	{76,94,77,14},
	{73,58,220,10},
	{73,58,220,10},
	{73,58,220,10},
	{193,93,192,165},
	{189,122,214,227},
	{191,101,54,148},
	{84,109,80,89},
	{84,109,80,89},
	{84,109,80,89},
	{70,95,65,80},
	{70,95,65,80},
	{70,95,65,80},
	{113,182,18,30},
	{113,182,18,30},
	{144,217,161,119},
	{46,223,254,56},
	{46,223,254,56},
	{71,90,148,209},
	{156,220,69,153},
	{158,181,19,102},
	{59,84,240,60},
	{95,181,176,106},
	{189,120,152,120},
	{153,136,8,214}
};

BOOST_AUTO_TEST_SUITE(test_ip_filter)

BOOST_AUTO_TEST_CASE(test_valid_version)
{
    BOOST_CHECK(version() > 0);
}

BOOST_AUTO_TEST_CASE(test_valid_ip_sort)
{
	std::vector<int> test_1{ 1, 70, 44, 170 };
	std::vector<int> test_2{ 222,82,198,61 };

	auto data_copy = g_source_test_data;
	std::sort(data_copy.begin(), data_copy.end(), lex_sort_pred(true));
	BOOST_TEST(std::equal(data_copy.back().begin(), data_copy.back().end(), test_1.begin()));
	BOOST_TEST(std::equal(data_copy.begin()->begin(), data_copy.begin()->end(), test_2.begin()));

	data_copy = g_source_test_data;
	std::sort(data_copy.begin(), data_copy.end(), lex_sort_pred(false));
	BOOST_TEST(std::equal(data_copy.back().begin(), data_copy.back().end(), test_2.begin()));
	BOOST_TEST(std::equal(data_copy.begin()->begin(), data_copy.begin()->end(), test_1.begin()));
	
}

BOOST_AUTO_TEST_CASE(test_valid_ip_filtering)
{
	auto data_copy = g_source_test_data;
	std::sort(data_copy.begin(), data_copy.end(), lex_sort_pred(true));

	ip_filter filter(data_copy);
	auto test_1_source = filter.filtering(1);
	std::vector<int> test_1{ 1, 70, 44, 170 };
	BOOST_TEST(test_1_source.size() == 1);
	BOOST_TEST(std::equal(test_1_source.begin()->begin(), test_1_source.begin()->end(), test_1.begin()));


	auto test_2_source = filter.filtering(73);
	std::vector<std::vector<int>> test_2{
		{73,115,6,159},
		{73,58,220,10},
		{73,58,220,10},
		{73,58,220,10}
	};
	BOOST_TEST(test_2_source.size() == test_2.size());
	BOOST_TEST(std::equal(test_2_source.begin()->begin(), test_2_source.begin()->end(), test_2.begin()->begin()));
	BOOST_TEST(std::equal((test_2_source.begin() + 1)->begin(), (test_2_source.begin() + 1)->end(), (test_2.begin() + 1)->begin()));
}

BOOST_AUTO_TEST_SUITE_END()
