/*
The ReverbGen library is distributed under the Boost Software License (BSL)
https://www.boost.org/LICENSE_1_0.txt
See LICENSE file for details.
Copyright (c) 2020 James Boer
*/

#include "catch.hpp"
#include <array>
#include <random>
#include "../Source/ReverbGen.h"
#include "../Source/RgInternal.h"

using namespace RevGen;

SignatureData GenerateBaselineSignature(std::mt19937 & r, size_t presetIndex)
{
	SignatureData signature;
	signature.presetIndex = presetIndex;
	for (auto & f : signature.distances)
		f = std::uniform_real_distribution<float>(0.0f, 1.0f)(r);
	for (auto & f : signature.absorbCoeffs)
		f = std::uniform_real_distribution<float>(0.0f, 1.0f)(r);
	NormalizeHistogram(signature.distances);
	NormalizeHistogram(signature.absorbCoeffs);
	return signature;
}

SignatureData GenerateDerivedSignature(std::mt19937 & r, const SignatureData & baseline)
{
	SignatureData signature;
	signature.presetIndex = baseline.presetIndex;
	for (size_t i = 0; i < signature.distances.size(); ++i)
		signature.distances[i] = std::clamp(std::normal_distribution<float>(baseline.distances[i], 0.05f)(r), 0.0f, 1.0f);
	for (size_t i = 0; i < signature.absorbCoeffs.size(); ++i)
		signature.absorbCoeffs[i] = std::clamp(std::normal_distribution<float>(baseline.absorbCoeffs[i], 0.05f)(r), 0.0f, 1.0f);
	NormalizeHistogram(signature.distances);
	NormalizeHistogram(signature.absorbCoeffs);
	return signature;
}

std::vector<SignatureData> GenerateSignatures(std::mt19937 & r, size_t numPresets, size_t numVariations)
{
	std::vector<SignatureData> signatures;
	for (size_t i = 0; i < numPresets; ++i)
	{
		auto baseline = GenerateBaselineSignature(r, i);
		signatures.push_back(baseline);
		for (size_t j = 0; j < numVariations - 1; ++j)
			signatures.push_back(GenerateDerivedSignature(r, baseline));
	}
	return signatures;
}

TEST_CASE("Test Signatures", "[Signatures]")
{
	SECTION("Test equality")
	{
		std::mt19937 r(1234567);

		size_t numPresets = 5;
		size_t signaturesPerPreset = 100;
		auto signatures = GenerateSignatures(r, numPresets, signaturesPerPreset);
		auto reducedSignatures = ReduceSignatures(signatures);

		REQUIRE(reducedSignatures.size() <= (numPresets * signaturesPerPreset));
	}

}