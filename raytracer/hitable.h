#pragma once

#include "ray.h"
#include "aabb.h"
#include <memory>

class material;

struct hit_record
{
	float t;
	vec3 p;
	vec3 normal;
	std::shared_ptr<material> mat_ptr;
	float u;
	float v;
	bool front_face;

	inline void set_face_normal( const ray& r, const vec3& outward_normal )
	{
		front_face = dot( r.direction(), outward_normal ) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hitable
{
public:
	virtual ~hitable() {}
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const = 0;
	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const = 0;
};