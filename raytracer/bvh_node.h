#pragma once

#include "hitable.h"
#include "hitable_list.h"
#include "random.h"
#include <algorithm>
#include <vector>
#include <memory>

class bvh_node : public hitable
{
public:
	bvh_node() {}

	bvh_node( const hitable_list& list, float time0, float time1 )
		: bvh_node( list.objects, 0, list.objects.size(), time0, time1 )
	{}

	bvh_node( const std::vector<std::shared_ptr<hitable>>& src_objects, std::size_t start, std::size_t end, float time0, float time1 );

	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const override;

	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override;

	std::shared_ptr<hitable> left;
	std::shared_ptr<hitable> right;
	aabb box;
};

bool bvh_node::bounding_box( float time0, float time1, aabb& output_box ) const
{
	output_box = box;
	return true;
}

bool bvh_node::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	if ( !box.hit( r, t_min, t_max ) )
		return false;

	bool hit_left = left->hit( r, t_min, t_max, rec );
	bool hit_right = right->hit( r, t_min, hit_left ? rec.t : t_max, rec );
	return hit_left || hit_right;
}

bool box_compare( const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b, int axis )
{
	aabb box_a;
	aabb box_b;

	a->bounding_box( 0.f, 0.f, box_a );
	b->bounding_box( 0.f, 0.f, box_b );

	return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare( const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b )
{
	return box_compare( a, b, 0 );
}

bool box_y_compare( const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b )
{
	return box_compare( a, b, 1 );
}

bool box_z_compare( const std::shared_ptr<hitable> a, const std::shared_ptr<hitable> b )
{
	return box_compare( a, b, 2 );
}

bvh_node::bvh_node( const std::vector<std::shared_ptr<hitable>>& src_objects, std::size_t start, std::size_t end, float time0, float time1 )
{
	auto objects = src_objects;

	int axis = random_int( 0, 2 );
	auto comparator = ( axis == 0 ) ? box_x_compare
		: ( axis == 1 ) ? box_y_compare
		: box_z_compare;

	std::size_t object_spawn = end - start;
	if ( object_spawn == 1 )
	{
		left = right = objects[start];
	}
	else if ( object_spawn == 2 )
	{
		if ( comparator( objects[start], objects[start + 1] ) )
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else
	{
		std::sort( objects.begin() + start, objects.begin() + end, comparator );

		auto mid = start + object_spawn / 2;
		left = std::make_shared<bvh_node>( objects, start, mid, time0, time1 );
		right = std::make_shared<bvh_node>( objects, mid, end, time0, time1 );
	}

	aabb box_left, box_right;

	left->bounding_box( time0, time1, box_left );
	right->bounding_box( time0, time1, box_right );

	box = surrounding_box( box_left, box_right );
}