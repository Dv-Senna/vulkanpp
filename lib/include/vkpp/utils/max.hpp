#pragma once



namespace vkpp::utils
{
	template <class T>
	T max(const T &start, const T &end)
	{
		T best {start};

		for (T it {start}; it != end; ++it)
		{
			if (*it > *best)
				best = it;
		}

		return best;
	}
} // namespace vkpp::utils
