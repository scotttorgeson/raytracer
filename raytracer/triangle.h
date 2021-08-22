#pragma once

#include "hitable.h"
#include "material.h"

class triangle : public hitable
{
public:
	triangle() {}
	triangle( vec3 pt0, vec3 pt1, vec3 pt2, std::shared_ptr<material> m ) : point0( pt0 ), point1( pt1 ), point2( pt2 ), mat_ptr( m ) {}
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override;

	vec3 point0;
	vec3 point1;
	vec3 point2;
	std::shared_ptr<material> mat_ptr;
};

// Moller-Trumbore intersection based on Wikipedia implementatation
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool triangle::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	const float epsilon = 0.0000001f;
	vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = point1 - point0;
	edge2 = point2 - point0;
	h = cross( r.direction(), edge2 );
	a = dot( edge1, h );
	if ( a > -epsilon && a < epsilon )
		return false; // ray is parallel to the triangle, no hit

	f = 1.0f / a;
	s = r.origin() - point0;
	u = f * dot( s, h );
	if ( u < 0.0f || u > 1.0f )
		return false; // u coord is off the edge of the triangle

	q = cross( s, edge1 );
	v = f * dot( r.direction(), q );
	if ( v < 0.0f || u + v > 1.0f )
		return false; // v coord is off the edge of the triangle

	float t = f * dot( edge2, q );
	if ( t > epsilon )
	{
		// todo: can precalc this to save time
		// cross product of edges, but which direction to send back? one thats opposite the ray?
		vec3 normal = cross( edge1, edge2 );
		normal.make_unit_vector();

		rec.t = t;
		rec.p = r.point_at_parameter( rec.t );
		rec.normal = normal; 
		rec.mat_ptr = mat_ptr;
		return true;
	}
	
	// line defined by the ray direction intersects but the ray does not
	return false;
}

bool triangle::bounding_box( float time0, float time1, aabb& output_box ) const
{
	output_box = aabb( vec3( 0.f, 0.f, 0.f ), vec3( 0.f, 0.f, 0.f ) );
	output_box = expand_box( output_box, point0 );
	output_box = expand_box( output_box, point1 );
	output_box = expand_box( output_box, point2 );
	return true;
}
