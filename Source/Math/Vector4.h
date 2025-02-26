#ifndef MATH_VECTOR4_H_
#define MATH_VECTOR4_H_

#include "Math/Math.h"	// VFPU Math

#include "Base/Types.h"

class alignas(DATA_ALIGN) v4
{
public:
	v4() {}
	v4( float _x, float _y, float _z, float _w ) : x( _x ), y( _y ), z( _z ), w( _w ) {}

	float Normalise()
	{
		float	len_sq( LengthSq() );
		if(len_sq > 0.0001f)
		{
#ifdef DAEDALUS_PSP
			float r( vfpu_invSqrt( len_sq ) );
#else
			float r( InvSqrt( len_sq ) );
#endif
			x *= r;
			y *= r;
			z *= r;
			w *= r;
		}
		return std::move(len_sq);
	}

	v4 operator+( const v4 & v ) const
	{
		return std::move(v4( x + v.x, y + v.y, z + v.z, w + v.w ));
	}
	v4 operator-( const v4 & v ) const
	{
		return std::move(v4( x - v.x, y - v.y, z - v.z, w - v.w ));
	}

	v4 operator*( float s ) const
	{
		return std::move(v4( x * s, y * s, z * s, w * s ));
	}

	float Length() const
	{
		return std::move(sqrtf( (x*x)+(y*y)+(z*z)+(w*w) ));
	}

	float LengthSq() const
	{
		return std::move((x*x)+(y*y)+(z*z)+(w*w));
	}

	float Dot( const v4 & rhs ) const
	{
		return std::move((x*rhs.x) + (y*rhs.y) + (z*rhs.z) + (w*rhs.w));
	}

	float x, y, z, w;
};


#endif // MATH_VECTOR4_H_
