/*
The ReverbGen library is distributed under the Boost Software License (BSL)
https://www.boost.org/LICENSE_1_0.txt
See LICENSE file for details.
Copyright (c) 2020 James Boer
*/
#pragma once


template <class T>
void NormalizeHistogram(T & h)
{
	float total = 0.0f;
	for (auto v : h)
		total += v;
	for (auto & v : h)
		v /= total;
}

template <class T>
float GetHistogramDifference(const T & h1, const T & h2)
{
	assert(h1.size() == h2.size());

	// Track difference between histograms a and b
	float difference = 0.0f;

	// Accumulate excess values when equalizing histogram bins
	float accumulation = 0.0f;

	// Calculates cost of moving a full value (1.0) between adjacent bins,
	// based on the total histogram size.
	const float workScale = 1.0f / static_cast<float>(h1.size() - 1);
	for (size_t i = 0; i < h1.size(); ++i)
	{
		// Calculate a delta between the two histograms
		float delta = std::abs(h1[i] - h2[i]);

		// Determine the minimum value between the delta and our current
		// accumulation.  We can reduce our current accumulation and local
		// delta by this amount and no more.
		float reduction = std::min(delta, accumulation);
		accumulation -= reduction;
		delta -= reduction;

		// Store leftover delta value as accumulation for future bins to use
		accumulation += delta;

		// Add the difference from the last bin as the current accumulation
		// times the workScale
		difference += (accumulation * workScale);
	}
	return std::min(difference, 1.0f);
}

