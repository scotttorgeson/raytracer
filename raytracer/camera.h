#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "ray.h"
#include "random.h"

class camera
{
public:
	camera( vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist, float _time0 = 0.f, float _time1 = 0.f ) // vfov is top to bottom in degrees
	{
		lens_radius = aperture / 2.0f;
		float theta = vfov * ( float )M_PI / 180.0f;
		float half_height = tan( theta / 2.0f );
		float half_width = aspect * half_height;
		origin = lookfrom;
		w = unit_vector( lookfrom - lookat );
		u = unit_vector( cross( vup, w ) );
		v = cross( w, u );
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		horizontal = 2.0f * half_width * focus_dist * u;
		vertical = 2.0f * half_height * focus_dist * v;
		time0 = _time0;
		time1 = _time1;
	}

	ray get_ray( float s, float t )
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray( origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, random_float( time0, time1 ) );
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
	float time0, time1;
};
