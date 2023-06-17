#pragma once

#include <ostream>


namespace vkpp::utils
{
	struct Version
	{
		int major, minor, patch;
	};

	inline std::ostream &operator<<(std::ostream &stream, const vkpp::utils::Version &version)
	{
		stream << version.major << "." << version.minor << "." << version.patch;
		return stream;
	}

} // namespace vkpp::utils
