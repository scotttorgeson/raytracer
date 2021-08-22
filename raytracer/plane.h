#pragma once

#include "hitable.h"
#include "material.h"

class plane : public hitable
{
public:
	plane() {}
	plane( vec3 pt, vec3 norm, std::shared_ptr<material> m ) : point( pt ), normal( norm ), mat_ptr( m ) {}
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override;

	vec3 point;
	vec3 normal;
	std::shared_ptr<material> mat_ptr;
};

bool plane::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	float d = dot( point, -normal );
	float t = -( d + dot( r.origin(), normal ) ) / dot( r.direction(), normal );
	if ( t > t_min && t < t_max )
	{
		rec.t = t;
		rec.p = r.point_at_parameter( rec.t );
		rec.set_face_normal( r, normal );
		rec.mat_ptr = mat_ptr;
		return true;
	}

	return false;
}

bool plane::bounding_box( float time0, float time1, aabb& output_box ) const
{
	output_box = aabb( vec3( -FLT_MAX, -FLT_MAX, -FLT_MAX ), vec3( FLT_MAX, FLT_MAX, FLT_MAX ) );
	return true;
}