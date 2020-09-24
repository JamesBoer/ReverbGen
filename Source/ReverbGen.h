/*
The ReverbGen library is distributed under the Boost Software License (BSL)
https://www.boost.org/LICENSE_1_0.txt
See LICENSE file for details.
Copyright (c) 2020 James Boer
*/
#pragma once

#include <vector>
#include <array>
#include <limits>

namespace RevGen
{

	struct Vec3
	{
		Vec3() {}
		Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
		void Set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
		Vec3 operator -() const { Vec3 v; v.Set(-x, -y, -z); return v; }
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	inline float DotProduct(const Vec3 & a, const Vec3 & b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}


	const std::array<float, 11> DistanceBinValues =
	{
		2.0f,
		4.0f,
		8.0f,
		16.0f,
		32.0f,
		64.0f,
		128.0f,
		256.0f,
		512.0f,
		1024.0f,
		std::numeric_limits<float>::max()
	};


	const size_t InvalidIndex = std::numeric_limits<size_t>::max();

	struct PresetPair
	{
		size_t presetIndexA = 0;
		size_t presetIndexB = InvalidIndex;
		float ratio = 1.0f;
	};

	using DistArray = std::array<float, DistanceBinValues.size()>;
	using AbsCoeffArray = std::array<float, 10>;
	struct SignatureData
	{
		size_t presetIndex = InvalidIndex;
		DistArray distances;
		AbsCoeffArray absorbCoeffs;
	};

	DistArray CreateHistogramFromDistances(std::vector<float> distances);
	PresetPair CalculatePresetPair(size_t indexA, float diffA, size_t indexB, float diffB);
	float GetSignatureDifference(const SignatureData & a, const SignatureData & b);
	std::vector<SignatureData> ReduceSignatures(const std::vector<SignatureData> & signatures);
	std::vector<Vec3> GetReciprocals(const std::vector<Vec3> & scanDirs);

	const float DistWeightAdj = 1.6f;

	float GetDistanceWeight(float distance);
	float GetReverseDistanceWeight(float distance);

}

