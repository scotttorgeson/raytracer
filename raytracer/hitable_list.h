#pragma once

#include "hitable.h"
#include <vector>
#include <memory>

class hitable_list : public hitable
{
public:
	hitable_list() {}
	hitable_list( std::shared_ptr<hitable> object ) { add( object ); }
	virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
	virtual bool bounding_box( float time0, float time1, aabb& output_box ) const override;

	void add( std::shared_ptr<hitable> object ) { objects.push_back( object ); }
	void clear() { objects.clear(); }

	std::vector< std::shared_ptr<hitable> > objects;
};

bool hitable_list::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;
	for ( const auto& object : objects )
	{
		if ( object->hit( r, t_min, closest_so_far, temp_rec ) )
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

bool hitable_list::bounding_box( float time0, float time1, aabb& output_box ) const
{
	if ( objects.empty() )
		return false;

	aabb temp_box;
	bool first_box = true;

	for ( const auto& object : objects )
	{
		if ( !object->bounding_box( time0, time1, temp_box ) )
			return false;
		output_box = first_box ? temp_box : surrounding_box( output_box, temp_box );
		first_box = false;
	}

	return true;
}