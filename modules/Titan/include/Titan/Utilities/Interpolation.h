//Titan Engine, by Atlas X Games 
// Interpolation.h - header for the class that gives static templates for interpolation functions
#pragma once

namespace Titan {
	class TTN_Interpolation final {
	public:

		//linear interpolation function
		template<typename T>
		static T Lerp(T p0, T p1, float t);

		//inverse linear interpolation function
		static float InverseLerp(float p0, float p1, float result);

		//Remap functions, maps a value from one range to another
		static float ReMap(float originalMin, float originalMax, float newMin, float newMax, float value);

		//cubic interpolation function
		template<typename T>
		static T CatmullRom(T p0, T p1, T p2, T p3, float t);

		//cubic interpolation function
		template<typename T>
		static T Bezier(T p0, T p1, T p2, T p3, float t);

	private:
		//cubic interpolation function helper
		template<typename T>
		static T Bezier2(T p0, T p1, T p2, float t);
	};

	//linear interpolation function
	template<typename T>
	inline T TTN_Interpolation::Lerp(T p0, T p1, float t)
	{
		return (1.0f - t) * p0 + t * p1;
	}

	//inverse linear interpolation function
	inline float TTN_Interpolation::InverseLerp(float p0, float p1, float result)
	{
		return (result - p0) / (p1 - p0);
	}

	//remap function that remaps a value from one range to another
	inline float TTN_Interpolation::ReMap(float originalMin, float originalMax, float newMin, float newMax, float value) {
		float t = TTN_Interpolation::InverseLerp(originalMin, originalMax, value);
		return TTN_Interpolation::Lerp(newMin, newMax, t);
	}

	//cubic interpolation function
	template<typename T>
	inline T TTN_Interpolation::CatmullRom(T p0, T p1, T p2, T p3, float t)
	{
		return 0.5f * (2.0f * p1 + t * (-p0 + p2)
			+ t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3)
			+ t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3));
	}

	//cubic interpolation function
	template<typename T>
	inline T TTN_Interpolation::Bezier(T p0, T p1, T p2, T p3, float t)
	{
		return Lerp(Bezier2(p0, p1, p2, t), Bezier2(p1, p2, p3, t));
	}

	//cubic interpolation function helper
	template<typename T>
	inline T TTN_Interpolation::Bezier2(T p0, T p1, T p2, float t)
	{
		return Lerp(Lerp(p0, p1, t), Lerp(p1, p2, t), t);
	}
}