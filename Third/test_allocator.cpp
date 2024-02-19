#define BOOST_TEST_MODULE test_allocator

#include "lib.h"
#include <boost/test/unit_test.hpp>

using namespace MemoryUtils;
BOOST_AUTO_TEST_SUITE(test_allocator)

BOOST_AUTO_TEST_CASE(test_valid_version)
{
    BOOST_CHECK(version() > 0);
}

BOOST_AUTO_TEST_CASE(test_vector_with_custom_allocator)
{
	CustomAllocationStrategy allocation_area{};
	CustomAllocator<int> custom_int_allocator{ allocation_area };
	custom_vector<int> vector{ custom_int_allocator };
	for (int i = 0u; i < 100; ++i)
	{
		vector.push_back(i);
		std::cout << vector.at(i) << " ";
	}

	vector.resize(16u);
	BOOST_CHECK(vector.size() == 16u);
	vector.clear();
	BOOST_CHECK(!vector.size());
}

BOOST_AUTO_TEST_CASE(test_unique_ptr_with_custom_allocator)
{
	CustomAllocationStrategy allocation_area{};
	CustomAllocator<int> custom_int_allocator{ allocation_area };
	custom_unique_ptr<int> ptr1 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator, 100);
	custom_unique_ptr<int> ptr2 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator, 500);
	custom_unique_ptr<int> ptr3 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator, 1000);
	custom_unique_ptr<int> ptr4 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator, 1500);
	BOOST_CHECK(*ptr1 == 100);
	BOOST_CHECK(*ptr2 == 500);
	BOOST_CHECK(*ptr3 == 1000);
	BOOST_CHECK(*ptr4 == 1500);
}

BOOST_AUTO_TEST_CASE(test_other_container_with_custom_allocator)
{
	CustomAllocationStrategy allocation_area{};
	CustomAllocator<int> custom_int_allocator{ allocation_area };
	CustomAllocator<float> custom_float_allocator{ custom_int_allocator };
	custom_list<float> list{ { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f }, custom_float_allocator };
	BOOST_CHECK(list.size() == 6);

	CustomAllocator<std::pair<double, double>> custom_pair_allocator{ allocation_area };
	custom_map<double, double> map{ { { 1.0, 100.0 }, { 2.0, 200.0 } }, custom_pair_allocator };
	BOOST_CHECK(map.size() == 2);

	CustomAllocator<double> custom_double_allocator{ allocation_area };
	custom_set<double> set{ { 1000.0, 2000.0, 3000.0 }, custom_double_allocator };
	BOOST_CHECK(set.size() == 3);

	CustomAllocator<char> custom_char_allocator{ allocation_area };
	custom_string string1{ "First allocated string without SBO ", custom_char_allocator };
	custom_string string2{ "Second allocated string without SBO ", custom_char_allocator };
	custom_string string3{ "Third allocated string without SBO ", custom_char_allocator };
	custom_string result_string = string1 + string2 + string3;
	BOOST_CHECK(result_string.size() == string1.size() + string2.size() + string3.size());
}

BOOST_AUTO_TEST_SUITE_END()
