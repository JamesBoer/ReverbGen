/*
The ReverbGen library is distributed under the Boost Software License (BSL)
https://www.boost.org/LICENSE_1_0.txt
See LICENSE file for details.
Copyright (c) 2020 James Boer
*/


#include "catch.hpp"
#include <array>
#include "../Source/ReverbGen.h"
#include "../Source/RgInternal.h"

TEST_CASE("Test Histogram", "[Histogram]")
{
	SECTION("Test equality")
	{
		std::array<float, 11> hist1 = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<float, 11> hist2 = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == 0.0f);
	}

	SECTION("Test max diff")
	{
		std::array<float, 11> hist1 = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<float, 11> hist2 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == Approx(1.0f));
	}

	SECTION("Test first single column shift")
	{
		std::array<float, 11> hist1 = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<float, 11> hist2 = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == Approx(0.1f));
	}

	SECTION("Test last single column shift")
	{
		std::array<float, 11> hist1 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		std::array<float, 11> hist2 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == Approx(0.1f));
	}

	SECTION("Test single column shift by five")
	{
		std::array<float, 11> hist1 = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<float, 11> hist2 = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == Approx(0.5f));
	}

	SECTION("Test single half column shift by five")
	{
		std::array<float, 11> hist1 = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<float, 11> hist2 = { 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == Approx(0.25f));
	}

	SECTION("Test small max diff")
	{
		std::array<float, 3> hist1 = { 1.0f, 0.0f, 0.0f };
		std::array<float, 3> hist2 = { 0.0f, 0.0f, 1.0f };

		float diff = GetHistogramDifference(hist1, hist2);
		REQUIRE(diff == 1.0f);
	}


}