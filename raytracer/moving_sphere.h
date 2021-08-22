#pragma once

#pragma once

#include "hitable.h"
#include "material.h"

class moving_sphere : public hitable
{
public:
	moving_sphere() {}
	moving_sphere( vec3 cen0, vec3 cen1, float r, material *m, float _time0, float _time1 ) :center0( cen0 ), center1( cen1 ), radius( r ), mat_ptr( m ), time0(_time0), time1(_time1) {}
	virtual ~moving_sphere() { if ( mat_ptr ) delete mat_ptr; mat_ptr = nullptr; }
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override;
	vec3 center( float time ) const;

	vec3 center0;
	vec3 center1;
	float radius;
	material *mat_ptr;
	float time0, time1;
};

vec3 moving_sphere::center( float time ) const
{
	return center0 + ( ( time - time0 ) / ( time1 - time0 ) ) * ( center1 - center0 );
}

bool moving_sphere::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	vec3 sphere_center = center( r.time() );
	vec3 oc = r.origin() - sphere_center;
	float a = dot( r.direction(), r.direction() );
	float b = dot( oc, r.direction() );
	float c = dot( oc, oc ) - radius * radius;
	float discriminant = b * b - a * c;
	if ( discriminant > 0.0f )
	{
		float temp = ( -b - sqrt( b * b - a * c ) ) / a;
		if ( temp < t_max && temp > t_min )
		{
			rec.t = temp;
			rec.p = r.point_at_parameter( rec.t );
			rec.normal = ( rec.p - sphere_center ) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = ( -b + sqrt( b * b - a * c ) ) / a;
		if ( temp < t_max && temp > t_min )
		{
			rec.t = temp;
			rec.p = r.point_at_parameter( rec.t );
			rec.normal = ( rec.p - sphere_center ) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool moving_sphere::bounding_box( float time0, float time1, aabb& output_box ) const
{
	aabb box0( center( time0 ) - vec3( radius, radius, radius ), center( time0 ) + vec3( radius, radius, radius ) );
	aabb box1( center( time1 ) - vec3( radius, radius, radius ), center( time1 ) + vec3( radius, radius, radius ) );
	output_box = surrounding_box( box0, box1 );
	return true;
}