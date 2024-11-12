#pragma once
/******************************************************************************
 * A three components vector (templated component type, meant to be arithmetic)
 *****************************************************************************/

#include <assert.h>
#include <cmath>
#include <functional>

template <typename T>
struct TVec3
{
	/* Members */
	T x;
	T y;
	T z;

	/* Constructors */
	constexpr TVec3() = default;
	constexpr TVec3(T x, T y, T z);
	explicit TVec3(const T *t);

	/* Index Accessor */
	T &operator[](int n);
	const T &operator[](int n) const;

	/* Equality */
	bool operator==(const TVec3 &a) const;

	/* Vector space structure */
	TVec3 operator-() const;
	TVec3 &operator+=(const TVec3 &a);
	TVec3 &operator-=(const TVec3 &a);
	TVec3 &operator*=(const T &t);
	TVec3 &operator/=(const T &t);

	/* Static members */
	static inline TVec3<T> Zero = {0, 0, 0};
	static inline TVec3<T> XAxis = {1, 0, 0};
	static inline TVec3<T> YAxis = {0, 1, 0};
	static inline TVec3<T> ZAxis = {0, 0, 1};
};

typedef TVec3<float> Vec3f;
typedef TVec3<double> Vec3d;
using Vec3 = Vec3f;

/* Free functions declarations */

template <typename T>
inline TVec3<T> operator+(const TVec3<T> &a, const TVec3<T> &b);

template <typename T>
inline TVec3<T> operator-(const TVec3<T> &a, const TVec3<T> &b);

template <typename T>
inline TVec3<T> operator*(const TVec3<T> &a, const T &t);

template <typename T>
inline TVec3<T> operator*(const T &t, const TVec3<T> &a);

template <typename T>
inline T dot(const TVec3<T> &a, const TVec3<T> &b);

template <typename T>
inline TVec3<T> cross(const TVec3<T> &a, const TVec3<T> &b);

template <typename T>
inline T norm(const TVec3<T> a);

template <typename T>
inline T norm(const TVec3<T> a);

template <typename T>
TVec3<T> normalized(const TVec3<T> &a);

template <typename T>
T max(const TVec3<T> &a);

template <typename T>
T min(const TVec3<T> &a);

template <typename T>
TVec3<T> abs(const TVec3<T> &a);

/* Functions implementations */

template <typename T>
inline constexpr TVec3<T>::TVec3(T x, T y, T z)
	: x{x}, y{y}, z{z}
{
}

template <typename T>
inline TVec3<T>::TVec3(const T *t)
	: x{t[0]}, y{t[1]}, z{t[2]}
{
}

template <typename T>
inline const T &TVec3<T>::operator[](int n) const
{
	assert(n >= 0 && n <= 2);
	return (&x)[n];
}

template <typename T>
inline T &TVec3<T>::operator[](int n)
{
	assert(n >= 0 && n <= 2);
	return (&x)[n];
}

template <typename T>
inline bool TVec3<T>::operator==(const TVec3<T> &a) const
{
	return (x == a.x && y == a.y && z == a.z);
}

template <>
inline bool TVec3<float>::operator==(const TVec3<float> &a) const
{
	constexpr float epslion = 1e-6;
	return (std::fabs(x - a.x) < epslion && std::fabs(y - a.y) < epslion && std::fabs(z - a.z) < epslion);
}

template <>
inline bool TVec3<double>::operator==(const TVec3<double> &a) const
{
	constexpr double epslion = 1e-10;
	return (std::fabs(x - a.x) < epslion && std::fabs(y - a.y) < epslion && std::fabs(z - a.z) < epslion);
}

template <typename T>
inline TVec3<T> TVec3<T>::operator-() const
{
	return {-x, -y, -z};
}

template <typename T>
inline TVec3<T> &TVec3<T>::operator+=(const TVec3<T> &a)
{
	x += a.x;
	y += a.y;
	z += a.z;
	return (*this);
}

template <typename T>
inline TVec3<T> &TVec3<T>::operator-=(const TVec3<T> &a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return (*this);
}

template <typename T>
inline TVec3<T> &TVec3<T>::operator*=(const T &t)
{
	x *= t;
	y *= t;
	z *= t;
	return (*this);
}

template <typename T>
inline TVec3<T> &TVec3<T>::operator/=(const T &t)
{
	x /= t;
	y /= t;
	z /= t;
	return (*this);
}

template <typename T>
inline TVec3<T> operator+(const TVec3<T> &a, const TVec3<T> &b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
inline TVec3<T> operator-(const TVec3<T> &a, const TVec3<T> &b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
inline TVec3<T> operator*(const TVec3<T> &a, const T &t)
{
	return {a.x * t, a.y * t, a.z * t};
}

template <typename T>
inline TVec3<T> operator*(const T &t, const TVec3<T> &a)
{
	return a * t;
}

template <typename T>
inline T dot(const TVec3<T> &a, const TVec3<T> &b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

template <typename T>
inline TVec3<T> cross(const TVec3<T> &a, const TVec3<T> &b)
{
	return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x};
}

template <typename T>
inline T norm2(const TVec3<T> a)
{
	return (a.x * a.x + a.y * a.y + a.z * a.z);
}

template <typename T>
inline T norm(const TVec3<T> a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

template <typename T>
TVec3<T> normalized(const TVec3<T> &a)
{
	T len = norm(a);
	return (len == 0) ? a : a * (1.f / len);
}

template <typename T>
T max(const TVec3<T> &a)
{
	return (a.y > a.x ? (a.z > a.y ? a.z : a.y) : (a.z > a.x ? a.z : a.x));
}

template <typename T>
T min(const TVec3<T> &a)
{
	return (a.y < a.x ? (a.z < a.y ? a.z : a.y) : (a.z < a.x ? a.z : a.x));
}

template <typename T>
TVec3<T> abs(const TVec3<T> &a)
{
	return {a.x < 0 ? -a.x : a.x, a.y < 0 ? -a.y : a.y,
			a.z < 0 ? -a.z : a.z};
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const TVec3<T> &arr)
{
	os << "[";
	for (size_t i = 0; i < 3; ++i)
	{
		os << arr[i];
		if (i < 2)
		{
			os << ", ";
		}
	}
	os << "]";
	return os;
}

namespace std
{
	template <typename T>
	struct hash<TVec3<T>>
	{
		std::size_t operator()(const TVec3<T> &vec) const
		{
			std::size_t hx = std::hash<T>()(std::round(vec.x * 1000) / 1000); // 处理小数位
			std::size_t hy = std::hash<T>()(std::round(vec.y * 1000) / 1000);
			std::size_t hz = std::hash<T>()(std::round(vec.z * 1000) / 1000);
			return hx ^ (hy << 1) ^ (hz << 2);
		}
	};
}
