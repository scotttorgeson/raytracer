#include "random.h"
#include <random>

std::default_random_engine generator;
std::uniform_real_distribution<float> distr( 0.0f, 1.0f );
float drand48()
{
	float ret;

	do
	{
		ret = distr( generator );
	} while ( ret == 1.0f );

	return ret;
}

vec3 random_in_unit_disk()
{
	vec3 p;
	do
	{
		p = 2.0f * vec3( drand48(), drand48(), 0.0f ) - vec3( 1.0f, 1.0f, 0.0f );
	} while ( dot( p, p ) >= 1.0f );
	return p;
}

vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0f * vec3( drand48(), drand48(), drand48() ) - vec3( 1.0f, 1.0f, 1.0f );
	} while ( p.squared_length() >= 1.0f );
	return p;
}