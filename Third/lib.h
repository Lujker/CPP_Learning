#pragma once

#include <limits>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>

int version();

// Common interface for interaction with STL
// containers and algorithms. You can manually change
// allocation algorithm with different 'AllocationStrategy'
//
// In this implementation was not implented 'adress' and 'max_size'
// unnecessary functions for.

template<typename T, class AllocationStrategy>
class Allocator
{
	static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");
public:
	using value_type = T;

	template<typename U, class AllocStrategy>
	friend class Allocator;

	template<typename U>
	struct rebind
	{
		using other = Allocator<U, AllocationStrategy>;
	};
public:
	Allocator() = default;

	explicit Allocator(AllocationStrategy& strategy) noexcept
		: m_allocation_strategy(&strategy) {}

	Allocator(const Allocator& other) noexcept
		: m_allocation_strategy(other.m_allocation_strategy) {}

	template<typename U>
	Allocator(const Allocator<U, AllocationStrategy>& other) noexcept
		: m_allocation_strategy(other.m_allocation_strategy) {}

	T* allocate(std::size_t count_objects)
	{
		assert(m_allocation_strategy && "Not initialized allocation strategy");
		return static_cast<T*>(m_allocation_strategy->allocate(count_objects * sizeof(T)));
	}

	void deallocate(void* memory_ptr, std::size_t count_objects)
	{
		assert(m_allocation_strategy && "Not initialized allocation strategy");
		m_allocation_strategy->deallocate(memory_ptr, count_objects * sizeof(T));
	}

	template<typename U, typename... Args>
	void construct(U* ptr, Args&&... args)
	{
		new (reinterpret_cast<void*>(ptr)) U{ std::forward<Args>(args)... };
	}

	template<typename U>
	void destroy(U* ptr)
	{
		ptr->~U();
	}
private:
	AllocationStrategy* m_allocation_strategy = nullptr;
};

template<typename T, typename U, class AllocationStrategy>
bool operator==(const Allocator<T, AllocationStrategy>& lhs, const Allocator<U, AllocationStrategy>& rhs)
{
	return lhs.m_allocation_strategy == rhs.m_allocation_strategy;
}

template<typename T, typename U, class AllocationStrategy>
bool operator!=(const Allocator<T, AllocationStrategy>& lhs, const Allocator<U, AllocationStrategy>& rhs)
{
	return !(lhs == rhs);
}

namespace MemoryUtils {
	std::size_t getAlignmentPadding(std::size_t not_aligned_address, std::size_t alignment);

	// Current chunk implementation works only with size
	// aligned by 4 bytes, because HEADER_SIZE now also 4 bytes.
	// Can modify it with HEADER_SIZE without problems.

	template<std::size_t CHUNK_SIZE>
	class Chunk
	{
		static constexpr std::size_t HEADER_SIZE = 4u;
		static_assert(CHUNK_SIZE% HEADER_SIZE == 0, "CHUNK_SIZE must be multiple of the four");
		static_assert(CHUNK_SIZE > HEADER_SIZE, "CHUNK_SIZE must be more than HEADER_SIZE");
	public:
		Chunk()
		{
			m_blocks.resize(CHUNK_SIZE);
			std::uint32_t* init_header = reinterpret_cast<std::uint32_t*>(m_blocks.data());
			*init_header = CHUNK_SIZE - HEADER_SIZE;
			m_max_block = init_header;
			m_free_blocks.insert(init_header);
		}

		bool isInside(const std::uint8_t* address) const noexcept
		{
			const std::uint8_t* start_chunk_address = reinterpret_cast<const std::uint8_t*>(m_blocks.data());
			const std::uint8_t* end_chunk_address = start_chunk_address + CHUNK_SIZE;
			return (start_chunk_address <= address) && (address <= end_chunk_address);
		}

		std::uint8_t* tryReserveBlock(std::size_t allocation_size)
		{
			const std::size_t not_aligned_address = reinterpret_cast<std::size_t>(m_max_block) + allocation_size;
			const std::size_t alignment_padding = getAlignmentPadding(not_aligned_address, HEADER_SIZE);
			const std::uint32_t allocation_size_with_alignment = static_cast<std::uint32_t>(allocation_size + alignment_padding);
			if ((!m_max_block) || (allocation_size_with_alignment > *m_max_block)) // Check on enaught memory for allocation
			{
				return nullptr;
			}

			// Find min available by size memory block
			const auto min_it = std::min_element(m_free_blocks.cbegin(), m_free_blocks.cend(), [allocation_size_with_alignment](const std::uint32_t* lhs, const std::uint32_t* rhs)
				{
					if (*rhs < allocation_size_with_alignment)
					{
						return true;
					}

					return (*lhs < *rhs) && (*lhs >= allocation_size_with_alignment);
				});

			assert(min_it != m_free_blocks.cend() && "Internal logic error with reserve block, something wrong in implementation...");
			assert(**min_it >= allocation_size_with_alignment && "Internal logic error with reserve block, something wrong in implementation...");

			std::uint32_t* header_address = *min_it;
			std::uint32_t* new_header_address =
				reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(header_address) + HEADER_SIZE + allocation_size_with_alignment);
			if (m_free_blocks.find(new_header_address) == m_free_blocks.cend()) // check if there is free memory in the current block
			{
				const std::uint32_t old_block_size = *header_address;
				const std::uint32_t difference = old_block_size - HEADER_SIZE;
				if (difference >= allocation_size_with_alignment) // check if there is enough space for another block
				{
					const std::uint32_t new_block_size = difference - allocation_size_with_alignment;
					*new_header_address = new_block_size;
					m_free_blocks.insert(new_header_address);
				}
			}

			m_free_blocks.erase(header_address);
			*header_address = static_cast<std::uint32_t>(allocation_size);
			if (header_address == m_max_block) // if the maximum block were changed, then need to find the maximum block again
			{
				// Find max block by size
				const auto max_it = std::max_element(m_free_blocks.cbegin(), m_free_blocks.cend(), [](const std::uint32_t* lhs, const std::uint32_t* rhs)
					{
						return (*lhs) < (*rhs);
					});

				// If there are no free blocks, therefore the memory in this chunk is over
				m_max_block = (max_it != m_free_blocks.cend()) ? (*max_it) : (nullptr);
			}

			return reinterpret_cast<std::uint8_t*>(header_address) + HEADER_SIZE;
		}

		void releaseBlock(std::uint8_t* block_ptr)
		{
			std::uint8_t* header_address = block_ptr - HEADER_SIZE;
			const std::uint32_t size_relized_block = *header_address;
			if ((!m_max_block) || (size_relized_block > *m_max_block)) // if the relized block is greater than the maximum, then need to replace it
			{
				m_max_block = reinterpret_cast<std::uint32_t*>(header_address);
			}

			m_free_blocks.insert(reinterpret_cast<std::uint32_t*>(header_address));
			auto forward_it = m_free_blocks.find(reinterpret_cast<std::uint32_t*>(header_address));
			auto backward_it = tryDefragment(forward_it, m_free_blocks.end());
			tryDefragment(std::make_reverse_iterator(backward_it), m_free_blocks.rend());
		}
	private:
		template<typename DstIterator, typename SrcIterator>
		constexpr DstIterator getIterator(SrcIterator it) const
		{
			using iterator = std::set<std::uint32_t*>::iterator;
			using reverse_iterator = std::set<std::uint32_t*>::reverse_iterator;
			if constexpr ((std::is_same_v<SrcIterator, iterator>) && (std::is_same_v<DstIterator, reverse_iterator>))
			{
				return std::make_reverse_iterator(it);
			}
			else if constexpr ((std::is_same_v<SrcIterator, reverse_iterator>) && (std::is_same_v<DstIterator, iterator>))
			{
				return it.base();
			}
			else
			{
				return it;
			}
		}

		template<typename Iterator>
		Iterator tryDefragment(Iterator start_it, Iterator end_it)
		{
			// primitive defragmentation algorithm - connects two neighboring
			// free blocks into one with linear complexity

			auto current_it = start_it++;
			auto next_it = start_it;
			std::uint32_t* current_header_address = *current_it;
			if ((current_it != end_it) && (next_it != end_it))
			{
				std::uint32_t* next_header_address = *next_it;
				const std::uint32_t current_block_size = *current_header_address;
				const std::uint32_t* available_current_block_address =
					reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(current_header_address) + HEADER_SIZE + current_block_size);
				if (available_current_block_address == next_header_address)
				{
					const std::uint32_t next_block_size = *next_header_address;
					const std::uint32_t new_current_block_size = current_block_size + HEADER_SIZE + next_block_size;
					*current_header_address = new_current_block_size;
					if (new_current_block_size > *m_max_block)
					{
						m_max_block = reinterpret_cast<std::uint32_t*>(current_header_address);
					}

					auto delete_it = getIterator<std::set<std::uint32_t*>::iterator>(next_it);
					return getIterator<Iterator>(m_free_blocks.erase(delete_it));
				}
			}

			return current_it;
		}
	public:
		std::vector<std::uint8_t*> m_blocks;
		std::set<std::uint32_t*> m_free_blocks;
		std::uint32_t* m_max_block;
	};
}

// Strategy for manipulation memory chunks, like
// a primitive malloc allocator.
//
// Warning: if you try to deallocate some random block
// of the memory, most of all it will be an undefined behavior,
// because current implementation doesn't check this possible situation.

template<std::size_t CHUNK_SIZE = 16'384u>
class CustomAllocationStrategy
{
	static_assert(CHUNK_SIZE != 0u, "Chunk size must be more, than zero");
	static_assert(CHUNK_SIZE <= std::numeric_limits<std::uint32_t>::max(),
		"Chunk size must be less or equal max value of the uint32_t");
public:
	void* allocate(std::size_t size)
	{
		assert(size < CHUNK_SIZE && "Incorrect chunk size for future usage");

		if (size == 0u)
		{
			return nullptr;
		}

		for (auto& chunk : m_chunks)
		{
			void* allocated_block = chunk.tryReserveBlock(size);
			if (allocated_block) //if the block was not reserved, then memory in the chunk has run out
			{
				return allocated_block;
			}
		}

		m_chunks.push_back(MemoryUtils::Chunk<CHUNK_SIZE>{});
		auto& chunk = m_chunks.back();
		std::uint8_t* allocated_block = chunk.tryReserveBlock(size);
		return allocated_block;
	}

	void deallocate(void* memory_ptr, std::size_t size)
	{
		if ((!memory_ptr) || (size == 0u))
		{
			return;
		}

		std::uint8_t* deallocation_ptr = static_cast<std::uint8_t*>(memory_ptr);
		for (auto& chunk : m_chunks)
		{
			if (chunk.isInside(deallocation_ptr))
			{
				chunk.releaseBlock(deallocation_ptr);
			}
		}
	}
private:
	std::deque<MemoryUtils::Chunk<CHUNK_SIZE>> m_chunks{ 1u };
};
namespace MemoryUtils {
	template<typename T, std::size_t CHUNK_SIZE = 16'384u>
	using CustomAllocator = Allocator<T, CustomAllocationStrategy<CHUNK_SIZE>>;

	template<typename T>
	using CustomAllocatorWithStackChunks = Allocator<T, CustomAllocationStrategy<1'024u>>;

	template<typename T>
	using CustomAllocatorWithHeapChunks = Allocator<T, CustomAllocationStrategy<16'384u>>;

	template<typename T>
	using custom_vector = std::vector<T, CustomAllocator<T>>;

	template<typename T>
	using custom_list = std::list<T, CustomAllocator<T>>;

	template<typename T>
	using custom_set = std::set<T, std::less<T>, CustomAllocator<T>>;

	template<typename T>
	using custom_unordered_set = std::unordered_set<T, std::hash<T>, std::equal_to<T>, CustomAllocator<T>>;

	template<typename K, typename V>
	using custom_map = std::map<K, V, std::less<K>, CustomAllocator<std::pair<const K, V>>>;

	template<typename K, typename V>
	using custom_unordered_map = std::unordered_map<K, std::hash<K>, std::equal_to<K>, CustomAllocator<std::pair<const K, V>>>;

	using custom_string = std::basic_string<char, std::char_traits<char>, CustomAllocator<char>>;

	template<typename T>
	using custom_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

	template<class Allocator, typename T = typename Allocator::value_type, typename... Args>
	custom_unique_ptr<T> make_custom_unique(Allocator allocator, Args&&... args)
	{
		const auto custom_deleter = [allocator](T* ptr) mutable
		{
			allocator.destroy(ptr);
			allocator.deallocate(ptr, 1u);
		};

		void* memory_block = allocator.allocate(1u);
		if (memory_block)
		{
			T* object_block = static_cast<T*>(memory_block);
			allocator.construct(object_block, std::forward<Args>(args)...);
			return custom_unique_ptr<T>{ object_block, custom_deleter };
		}

		return nullptr;
	}
}