#pragma once

#include "hitable.h"
#include "material.h"

class sphere : public hitable 
{
public:
	sphere() {}
	sphere( vec3 cen, float r, std::shared_ptr<material> m ) :center( cen ), radius( r ), mat_ptr( m ) {}
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override;

	vec3 center;
	float radius;
	std::shared_ptr<material> mat_ptr;

private:
	static void get_sphere_uv( const vec3& p, float& u, float& v )
	{
		const float pi = 3.1415926535897932385f;
		auto theta = acos( -p.y() );
		auto phi = atan2( -p.z(), p.x() ) + pi;

		u = phi / ( 2.f * pi );
		v = theta / pi;
	}
};

bool sphere::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	vec3 oc = r.origin() - center;
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
			rec.normal = ( rec.p - center ) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv( rec.normal, rec.u, rec.v );
			return true;
		}
		temp = ( -b + sqrt( b * b - a * c ) ) / a;
		if ( temp < t_max && temp > t_min )
		{
			rec.t = temp;
			rec.p = r.point_at_parameter( rec.t );
			rec.normal = ( rec.p - center ) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv( rec.normal, rec.u, rec.v );
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box( float time0, float time1, aabb& output_box ) const
{
	output_box = aabb( center - vec3( radius, radius, radius ), center + vec3( radius, radius, radius ) );
	return true;
}
