#pragma once
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

typedef std::vector< std::string > stringvector_t;


class vov_t
{
	private:
	protected:
		std::vector< stringvector_t > m_vov;

	public:
		vov_t(void) {}

		~vov_t(void) {}

		std::vector< stringvector_t > vector(void) { return m_vov; }

		void
		split(const stringvector_t& sv, const std::size_t n = 2)
		{
			std::size_t div(sv.size() / n);
			std::size_t mod(sv.size() % n);

			for (std::size_t idx = 0; idx < n; idx++) {
				stringvector_t tmp;
				std::size_t start_idx(idx * div + std::min(idx, mod));
				std::size_t end_idx(idx + 1 * div + std::min(idx + 1, mod));

				for (std::size_t inner_idx = start_idx; inner_idx != end_idx; inner_idx++)
					tmp.push_back(sv[inner_idx]);

				m_vov.push_back(tmp);
			}

			return;
		}

		std::vector< stringvector_t >
		unity_extractor(vov_t& vov, stringvector_t& sv)
		{
			std::vector< stringvector_t > remaining;
			sv.clear();

			for (std::size_t idx = 0; idx < vov.m_vov.size(); idx++)
				if (1 == vov.m_vov[idx].size())
					sv = vov.m_vov[idx];
				else
					remaining.push_back(vov.m_vov[idx]);

			return remaining;
		}
};

