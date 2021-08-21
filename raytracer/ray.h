#pragma once
#include "vec3.h"

class ray
{
public:
	ray() {}
	ray( const vec3& origin, const vec3& direction, float time = 0.0 ) { orig = origin; dir = direction; tm = time; }
	vec3 origin() const { return orig; }
	vec3 direction() const { return dir; }
	vec3 point_at_parameter( float t ) const { return orig + t * dir; }
	float time() const { return tm; }

	vec3 orig;
	vec3 dir;
	float tm;
};