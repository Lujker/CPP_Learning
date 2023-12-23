#pragma once

#include <string>
#include <vector>

// lexicographically sort (and reverse)
struct lex_sort_pred {
	lex_sort_pred(bool is_reverse) : is_reverse(is_reverse) {}
	~lex_sort_pred() = default;
	// lexicographically sort (and reverse)
	bool operator()(const auto& first, const auto& second) {
		for (size_t i = 0; i < first.size() && i < second.size(); ++i) {
			if (is_reverse ? first.at(i) > second.at(i) : first.at(i) < second.at(i)) {
				return true;
			}
			if (is_reverse ? first.at(i) < second.at(i) : first.at(i) > second.at(i)) {
				return false;
			}
		}
		return false;
	}
private:
	bool is_reverse = false;
};

struct ip_filter {
	ip_filter(const std::vector<std::vector<int>>& ips) : _ips(ips) {}
	~ip_filter() = default;

	std::vector<std::vector<int>> filtering(int first);

	std::vector<std::vector<int>> filtering(int first, int second);

	std::vector<std::vector<int>> filtering_any_place(int first);

	bool filtering_exist(const std::vector<int>& serching_ip);

private:
	const std::vector<std::vector<int>>& _ips;
};

std::vector<std::string> split(const std::string& str, char d);

void cout_ips(const std::vector<std::vector<int>>& ip_pool);

int version();