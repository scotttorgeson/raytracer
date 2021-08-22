#pragma once

#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "hitable_list.h"
#include "moving_sphere.h"
#include "bvh_node.h"
#include <vector>
#include <memory>

hitable_list simple_scene()
{
	hitable_list world;
	world.add( std::make_shared<sphere>( vec3( 0.0f, 0.0f, -1.0f ), 0.5f, new lambertian( vec3( 0.1f, 0.2f, 0.5f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 0.0f, -100.5f, -1.0f ), 100.0f, new lambertian( vec3( 0.8f, 0.8f, 0.0f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 1.0f, 0.0f, -1.0f ), 0.5f, new metal( vec3( 0.8f, 0.6f, 0.2f ), 0.3f ) ) );
	world.add( std::make_shared<sphere>( vec3( -1.0f, 0.0f, -1.0f ), 0.5f, new dielectric( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -1.0f, 0.0f, -1.0f ), -0.45f, new dielectric( 1.5f ) ) );
	return world;
}

hitable_list random_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = drand48();
			vec3 center( a + 0.9f*drand48(), 0.2f, b + 0.9f*drand48() );
			if ( ( center - vec3( 4.0f, 0.2f, 0.0f ) ).length() > 0.9f )
			{
				if ( choose_mat < 0.8f ) // diffuse
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new lambertian( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ) ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new metal( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new dielectric( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	return world;
}

hitable_list moving_sphere_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = drand48();
			vec3 center( a + 0.9f*drand48(), 0.2f, b + 0.9f*drand48() );
			if ( ( center - vec3( 4.0f, 0.2f, 0.0f ) ).length() > 0.9f )
			{
				if ( choose_mat < 0.8f ) // diffuse
				{
					auto center2 = center + vec3( 0.f, random_float( 0.0f, 0.5f ), 0.f );
					world.add( std::make_shared<moving_sphere>( center, center2, 0.2f, new lambertian( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ), 0.0f, 1.0f ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new metal( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new dielectric( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	return world;
}

hitable_list bvh_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = drand48();
			vec3 center( a + 0.9f*drand48(), 0.2f, b + 0.9f*drand48() );
			if ( ( center - vec3( 4.0f, 0.2f, 0.0f ) ).length() > 0.9f )
			{
				if ( choose_mat < 0.8f ) // diffuse
				{
					auto center2 = center + vec3( 0.f, random_float( 0.0f, 0.5f ), 0.f );
					world.add( std::make_shared<moving_sphere>( center, center2, 0.2f, new lambertian( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ), 0.0f, 1.0f ) );

				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new metal( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new dielectric( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	return hitable_list( std::make_shared<bvh_node>( world, 0.f, 1.f ) );
}

hitable_list plane_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = drand48();
			vec3 center( a + 0.9f*drand48(), 0.2f, b + 0.9f*drand48() );
			if ( ( center - vec3( 4.0f, 0.2f, 0.0f ) ).length() > 0.9f )
			{
				if ( choose_mat < 0.8f ) // diffuse
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new lambertian( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ) ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new metal( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, new dielectric( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	world.add( std::make_shared<plane>( vec3( 0.0f, 0.0f, -1.5f ), vec3( 0.0f, 0.0f, -1.0f ), new dielectric( 1.5f ) ) );
	return world;
}

hitable_list triangle_scene()
{
	hitable_list world;
	world.add( std::make_shared<plane>( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ), new lambertian( vec3( 0.0f, 0.2f, 0.8f ) ) ) );

	const vec3 bottom_left( -1.0f, 0.0f, -1.0f );
	const vec3 bottom_right( 1.0f, 0.0f, -1.0f );
	const vec3 top_left( -1.0f, 0.0f, 1.0f );
	const vec3 top_right( 1.0f, 0.0f, 1.0f );
	const vec3 point( 0.0f, 1.7f, 0.0f );

	world.add( std::make_shared<triangle>( bottom_left, bottom_right, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );
	world.add( std::make_shared<triangle>( bottom_left, top_left, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );
	world.add( std::make_shared<triangle>( top_left, top_right, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );
	world.add( std::make_shared<triangle>( top_right, bottom_right, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );

	return hitable_list( std::make_shared<bvh_node>( world, 0.f, 1.f ) );
}

hitable_list empty_scene()
{
	hitable_list world;
	return world;
}
