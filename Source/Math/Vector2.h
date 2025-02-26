#ifndef MATH_VECTOR2_H_
#define MATH_VECTOR2_H_

#include "Math/Math.h"	// VFPU Math


class v2
{
public:
	v2() {}
	v2( float _x, float _y ) : x( _x ), y( _y ) {}

	v2 operator+( const v2 & v ) const
	{
		return std::move(v2( x + v.x, y + v.y ));
	}

	v2 operator-( const v2 & v ) const
	{
		return std::move(v2( x - v.x, y - v.y ));
	}

	v2 operator+() const
	{
		return *this;
	}

	v2 operator-() const
	{
		return std::move(v2( -x, -y ));
	}

	v2 operator*( float s ) const
	{
		return std::move(v2( x * s, y * s ));
	}

	inline friend v2 operator*( float s, const v2 & v )
	{
		return std::move(v2( v.x * s, v.y * s ));
	}

	v2 operator/( float s ) const
	{
		float r( 1.0f / s );
		return std::move(v2( x * r, y * r ));
	}

	const v2 & operator+=( const v2 & rhs )
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	const v2 & operator*=( float s )
	{
		x *= s;
		y *= s;
		return *this;
	}

	float Length() const
	{
		return std::move(sqrtf( (x*x)+(y*y) ));
	}

	float LengthSq() const
	{
		return std::move((x*x)+(y*y));
	}

	float x, y;
};

#endif // MATH_VECTOR2_H_
