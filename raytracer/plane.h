#pragma once

#include "hitable.h"
#include "material.h"

class plane : public hitable
{
public:
	plane() {}
	plane( vec3 pt, vec3 norm, material *m ) : point( pt ), normal( norm ), mat_ptr( m ) {}
	virtual ~plane() { if ( mat_ptr ) delete mat_ptr; mat_ptr = nullptr; }
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;

	vec3 point;
	vec3 normal;
	material *mat_ptr;
};

bool plane::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	float d = dot( point, -normal );
	float t = -( d + dot( r.origin(), normal ) ) / dot( r.direction(), normal );
	if ( t > t_min && t < t_max )
	{
		rec.t = t;
		rec.p = r.point_at_parameter( rec.t );
		rec.normal = normal;
		rec.mat_ptr = mat_ptr;
		return true;
	}

	return false;
}