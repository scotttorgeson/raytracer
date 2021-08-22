#pragma once

#include "hitable.h"

class xy_rect : public hitable
{
public:
	xy_rect() {}
	xy_rect( float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat )
		: x0( _x0 ), x1( _x1 ), y0( _y0 ), y1( _y1 ), k( _k ), mp( mat ) {}

	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const override;

	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override
	{
		output_box = aabb( vec3( x0, y0, k - 0.0001f ), vec3( x1, y1, k + 0.0001f ) );
		return true;
	}

	std::shared_ptr<material> mp;
	float x0, x1, y0, y1, k;
};

bool xy_rect::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	auto t = ( k - r.origin().z() ) / r.direction().z();
	if ( t < t_min || t > t_max )
		return false;
	auto x = r.origin().x() + t * r.direction().x();
	auto y = r.origin().y() + t * r.direction().y();
	if ( x < x0 || x > x1 || y < y0 || y > y1 )
		return false;
	rec.u = ( x - x0 ) / ( x1 - x0 );
	rec.v = ( y - y0 ) / ( y1 - y0 );
	rec.t = t;
	auto outward_normal = vec3( 0.f, 0.f, 1.f );
	rec.set_face_normal( r, outward_normal );
	rec.mat_ptr = mp;
	rec.p = r.at( t );
	return true;
}