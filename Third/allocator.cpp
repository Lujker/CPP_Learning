#include "lib.h"
#include <chrono>

using namespace MemoryUtils;
int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[])
{
    try
    {
		auto start = std::chrono::high_resolution_clock::now();
		CustomAllocationStrategy allocation_area{};

		CustomAllocator<int> custom_int_allocator{ allocation_area };
		custom_vector<int> vector{ custom_int_allocator };
		for (int i = 0u; i < 100; ++i)
		{
			vector.push_back(i);
			std::cout << vector.at(i) << " ";
		}

		vector.resize(16u);
		for (int val : vector)
		{
			std::cout << val << " ";
		}

		CustomAllocator<int> custom_int_allocator_copy = vector.get_allocator();
		custom_unique_ptr<int> ptr1 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator_copy, 100);
		custom_unique_ptr<int> ptr2 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator_copy, 500);
		custom_unique_ptr<int> ptr3 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator_copy, 1000);
		custom_unique_ptr<int> ptr4 = make_custom_unique<CustomAllocator<int>>(custom_int_allocator_copy, 1500);
		std::cout << *ptr1 << " " << *ptr2 << " " << *ptr3 << " " << *ptr4 << " ";

		CustomAllocator<float> custom_float_allocator{ custom_int_allocator };
		custom_list<float> list{ { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f }, custom_float_allocator };
		for (float val : list)
		{
			std::cout << val << " ";
		}

		CustomAllocator<std::pair<double, double>> custom_pair_allocator{ allocation_area };
		custom_map<double, double> map{ { { 1.0, 100.0 }, { 2.0, 200.0 } }, custom_pair_allocator };
		for (const auto& it : map)
		{
			std::cout << "{" << it.first << " : " << it.second << "} ";
		}

		CustomAllocator<double> custom_double_allocator{ allocation_area };
		custom_set<double> set{ { 1000.0, 2000.0, 3000.0 }, custom_double_allocator };
		for (double val : set)
		{
			std::cout << val << " ";
		}
		std::cout << std::endl;
		CustomAllocator<char> custom_char_allocator{ allocation_area };
		custom_string string1{ "First allocated string without SBO ", custom_char_allocator };
		custom_string string2{ "Second allocated string without SBO ", custom_char_allocator };
		custom_string string3{ "Third allocated string without SBO ", custom_char_allocator };
		custom_string result_string = string1 + string2 + string3;
		std::cout << result_string << std::endl;

		auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Executed time is (microseconds): " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
