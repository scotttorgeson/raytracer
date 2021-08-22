#pragma once

#include "hitable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hitable
{
public:
	constant_medium( std::shared_ptr<hitable> b, float d, std::shared_ptr<texture> a )
		: boundary( b ), neg_inv_density( -1.f / d ), phase_function( std::make_shared<isotropic>( a ) )
	{}

	constant_medium( std::shared_ptr<hitable> b, float d, vec3 c )
		: boundary( b ), neg_inv_density( -1.f / d ), phase_function( std::make_shared<isotropic>( c ) )
	{}

	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const override;

	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override
	{
		return boundary->bounding_box( time0, time1, output_box );
	}

	std::shared_ptr<hitable> boundary;
	std::shared_ptr<material> phase_function;
	float neg_inv_density;
};

bool constant_medium::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	hit_record rec1, rec2;

	if ( !boundary->hit( r, -FLT_MAX, FLT_MAX, rec1 ) )
		return false;

	if ( !boundary->hit( r, rec1.t + 0.0001f, FLT_MAX, rec2 ) )
		return false;

	if ( rec1.t < t_min )
		rec1.t = t_min;

	if ( rec2.t > t_max )
		rec2.t = t_max;

	if ( rec1.t >= rec2.t )
		return false;

	if ( rec1.t < 0.f )
		rec1.t = 0.f;

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = ( rec2.t - rec1.t ) * ray_length;
	const auto hit_distance = neg_inv_density * log( drand48() );

	if ( hit_distance > distance_inside_boundary )
		return false;

	rec.t = rec1.t + hit_distance / ray_length;
	rec.p = r.at( rec.t );

	rec.normal = vec3( 1.f, 0.f, 0.f );
	rec.front_face = true;
	rec.mat_ptr = phase_function;

	return true;
}
