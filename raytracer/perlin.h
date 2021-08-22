#pragma once

#include "random.h"

class perlin
{
public:
	perlin()
	{
		ranvec = new vec3[point_count];
		for ( int i = 0; i < point_count; ++i )
		{
			ranvec[i] = unit_vector( vec3( random_float( -1.f, 1.f ), random_float( -1.f, 1.f ), random_float( -1.f, 1.f ) ) );
		}

		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();
	}

	~perlin()
	{
		delete[] ranvec;
		delete[] perm_x;
		delete[] perm_y;
		delete[] perm_z;
	}

	float noise( const vec3& p ) const
	{
		auto u = p.x() - floor( p.x() );
		auto v = p.y() - floor( p.y() );
		auto w = p.z() - floor( p.z() );

		/*u = u * u * ( 3.f - 2.f * u );
		v = v * v * ( 3.f - 2.f * v );
		w = w * w * ( 3.f - 2.f * w );*/

		auto i = static_cast< int >( floor( p.x() ) );
		auto j = static_cast< int >( floor( p.y() ) );
		auto k = static_cast< int >( floor( p.z() ) );
		vec3 c[2][2][2];

		for ( int di = 0; di < 2; di++ )
		{
			for ( int dj = 0; dj < 2; dj++ )
			{
				for ( int dk = 0; dk < 2; dk++ )
				{
					c[di][dj][dk] = ranvec[
						perm_x[( i + di ) & 255] ^
						perm_y[( j + dj ) & 255] ^
						perm_z[( k + dk ) & 255]
					];
				}
			}
		}

		return perlin_interp( c, u, v, w );
	}

	float turb( const vec3& p, int depth = 7 ) const
	{
		float accum = 0.f;
		vec3 temp_p = p;
		float weight = 1.f;

		for ( int i = 0; i < depth; ++i )
		{
			accum += weight * noise( temp_p );
			weight *= 0.5f;
			temp_p *= 2.0f;
		}

		return fabs( accum );
	}

private:
	static const int point_count = 256;
	vec3* ranvec;
	int* perm_x;
	int* perm_y;
	int* perm_z;

	static float trilinear_interp( float c[2][2][2], float u, float v, float w )
	{
		float accum = 0.f;
		for ( int i = 0; i < 2; ++i )
		{
			for ( int j = 0; j < 2; ++j )
			{
				for ( int k = 0; k < 2; ++k )
				{
					accum += ( i * u + ( 1 - i ) * ( 1 - u ) )
						* ( j * v + ( 1 - j ) * ( 1 - v ) )
						* ( k * w + ( 1 - k ) * ( 1 - w ) )
						* c[i][j][k];
				}
			}
		}

		return accum;
	}

	static float perlin_interp( vec3 c[2][2][2], float u, float v, float w )
	{
		float uu = u * u * ( 3.f - 2.f * u );
		float vv = v * v * ( 3.f - 2.f * v );
		float ww = w * w * ( 3.f - 2.f * w );

		float accum = 0.f;
		for ( int i = 0; i < 2; ++i )
		{
			for ( int j = 0; j < 2; ++j )
			{
				for ( int k = 0; k < 2; ++k )
				{
					vec3 weight_v( u - i, v - j, w - k );
					accum += ( i * uu + ( 1 - i ) * ( 1 - uu ) )
						* ( j * vv + ( 1 - j ) * ( 1 - vv ) )
						* ( k * ww + ( 1 - k ) * ( 1 - ww ) )
						* dot( c[i][j][k], weight_v );
				}
			}
		}
		return accum;
	}

	static int* perlin_generate_perm()
	{
		auto p = new int[point_count];

		for ( int i = 0; i < perlin::point_count; ++i )
		{
			p[i] = i;
		}

		permute( p, point_count );

		return p;
	}

	static void permute( int* p, int n )
	{
		for ( int i = n - 1; i > 0; i-- )
		{
			int target = random_int( 0, i );
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}
};