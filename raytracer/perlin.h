#pragma once

#include "random.h"

class perlin
{
public:
	perlin()
	{
		ranfloat = new float[point_count];
		for ( int i = 0; i < point_count; ++i )
		{
			ranfloat[i] = drand48();
		}

		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();
	}

	~perlin()
	{
		delete[] ranfloat;
		delete[] perm_x;
		delete[] perm_y;
		delete[] perm_z;
	}

	float noise( const vec3& p ) const
	{
		auto i = static_cast< int >( 4.f * p.x() ) & 255;
		auto j = static_cast< int >( 4.f * p.y() ) & 255;
		auto k = static_cast< int >( 4.f * p.z() ) & 255;

		return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}

private:
	static const int point_count = 256;
	float* ranfloat;
	int* perm_x;
	int* perm_y;
	int* perm_z;

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