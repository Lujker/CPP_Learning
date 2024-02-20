#include "lib.h"
#include "version.h"


int version() {
    return PROJECT_VERSION_PATCH;
}

std::size_t MemoryUtils::getAlignmentPadding(std::size_t not_aligned_address, std::size_t alignment)

{
	if ((alignment != 0u) && (not_aligned_address % alignment != 0u))
	{
		const std::size_t multiplier = (not_aligned_address / alignment) + 1u;
		const std::size_t aligned_address = multiplier * alignment;
		return aligned_address - not_aligned_address;
	}

	return 0u;
}
