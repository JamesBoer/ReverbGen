/*
The ReverbGen library is distributed under the Boost Software License (BSL)
https://www.boost.org/LICENSE_1_0.txt
See LICENSE file for details.
Copyright (c) 2020 James Boer
*/

#include "RgInternal.h"

namespace RevGen
{
	PresetPair CalculatePresetPair(size_t indexA, float diffA, size_t indexB, float diffB)
	{
		if (diffB < diffA)
		{
			std::swap(diffA, diffB);
			std::swap(indexA, indexB);
		}
		PresetPair pair;
		pair.presetIndexA = indexA;
		if (diffA > 0.05f || diffB < 0.4f)
		{
			pair.presetIndexB = indexB;
			float totalDiff = diffA + diffB;
			pair.ratio = (totalDiff - diffA) / totalDiff;
		}
		return pair;
	}

	DistArray CreateHistogramFromDistances(std::vector<float> distances)
	{
		DistArray histogram {};
		for (auto d : distances)
		{
			for (size_t i = 0; i < DistanceBinValues.size(); ++i)
			{
				if (d <= DistanceBinValues[i])
					histogram[i] += 1.0f;
			}
		}
		NormalizeHistogram(histogram);
		return histogram;
	}

	float GetSignatureDifference(const SignatureData & a, const SignatureData & b)
	{
		float distDiff = GetHistogramDifference(a.distances, b.distances);
		float absCoeffDiff = GetHistogramDifference(a.absorbCoeffs, b.absorbCoeffs);
		return (distDiff + absCoeffDiff) / 2.0f;
	}

	static std::vector<SignatureData> ReduceSignaturesByDifference(
		const std::vector<SignatureData> & signatures, const float diff)
	{
		// Copy signatures into intermediate structure for diff-based culling
		std::vector<SignatureData> s = signatures;

		// Mark all signatures within diff threshold of
		// another signature as invalid
		for (size_t j = 0; j < s.size(); ++j)
		{
			if (s[j].presetIndex == InvalidIndex)
				continue;
			for (size_t i = j; i < s.size(); ++i)
			{
				if (s[i].presetIndex == InvalidIndex || i == j)
					continue;
				if (GetSignatureDifference(s[i], s[j]) < diff)
					s[i].presetIndex = InvalidIndex;
			}
		}

		// Find and erase any invalid presets from the vector s
		s.erase(std::remove_if(s.begin(), s.end(), [&] (const auto & e) {
			return e.presetIndex == InvalidIndex;
		}), s.end());

		// Return vector of culled signatures
		return s;
	}

	static std::vector<SignatureData> ReduceSignaturesBySize(
		const std::vector<SignatureData> & signatures, const size_t size)
	{
		// Copy signatures into intermediate structure for size-based culling
		const auto & s = signatures;

		// Calculate all differences and store index and diff value in a vector
		std::vector<std::pair<size_t, float>> diffs;
		for (size_t j = 0; j < s.size(); ++j)
		{
			for (size_t i = j; i < s.size(); ++i)
			{
				if (i == j)
					continue;
				float delta = GetSignatureDifference(s[i], s[j]);
				diffs.emplace_back(i, delta);
			}
		}

		// Sort the diff vector from largest to smallest diff.  
		std::sort(diffs.begin(), diffs.end(), [] (const auto & a, const auto & b)
		{
			return a.second > b.second;
		});

		// Copy the top unique presets into sigs
		std::vector<SignatureData> sigs;
		sigs.reserve(size);
		std::set<size_t> indices;
		size_t index = 0;
		while (sigs.size() < size)
		{
			if (indices.find(diffs[index].first) == indices.end())
			{
				sigs.push_back(s[diffs[index].first]);
				indices.insert(diffs[index].first);
			}
			++index;
		}

		return sigs;
	}

	std::vector<SignatureData> ReduceSignatures(const std::vector<SignatureData> & signatures)
	{
		// Adjustable constants
		const float MinViableDistance = 0.02f;
		const size_t MaxSignaturesPerPreset = 5;

		// Copy signatures into intermediate structure for initial distance-based culling
		std::vector<SignatureData> sigs = ReduceSignaturesByDifference(
			signatures, MinViableDistance);

		// Organize signatures by preset index for additional processing,
		// then clear sigs, which we'll reuse later.
		std::unordered_map<size_t, std::vector<SignatureData>> sigMap;
		for (const auto & s : sigs)
			sigMap[s.presetIndex].push_back(s);
		sigs.clear();

		// Reduce number of signatures for each preset index to no more
		// than MaxSignaturesPerPreset
		for (auto & sp : sigMap)
		{
			auto & s = sp.second;

			// Reduce numbers if the vector size is greater than the threshold
			if (s.size() > MaxSignaturesPerPreset)
				s = ReduceSignaturesBySize(s, MaxSignaturesPerPreset);

			// Copy remaining elements to final output vector
			sigs.insert(sigs.end(), s.begin(), s.end());
		}

		return sigs;
	}

	std::vector<Vec3> GetReciprocals(const std::vector<Vec3> & scanDirs)
	{
		std::vector<Vec3> reciprocals;
		reciprocals.reserve(scanDirs.size());
		for (size_t j = 0; j < scanDirs.size(); ++j)
		{
			size_t bestIndex = 0;
			float bestDot = 1.0f;
			for (size_t i = 0; i < scanDirs.size(); ++i)
			{
				if (i == j)
					continue;
				float dot = DotProduct(scanDirs[i], scanDirs[j]);
				if (dot < bestDot)
				{
					bestDot = dot;
					bestIndex = i;
				}
			}
			reciprocals.push_back(scanDirs[bestIndex]);
		}
		return reciprocals;
	}

	float GetDistanceWeight(float distance)
	{
		return std::pow(std::log(std::clamp(distance, 2.0f, 1024.0f)), DistWeightAdj);
	}

	float GetReverseDistanceWeight(float distance)
	{
		return 100.0f * std::pow(0.9f, distance);
	}

}
