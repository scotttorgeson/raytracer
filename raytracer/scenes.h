#pragma once

#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "hitable_list.h"
#include "moving_sphere.h"
#include "bvh_node.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include <vector>
#include <memory>

hitable_list simple_scene()
{
	hitable_list world;
	world.add( std::make_shared<sphere>( vec3( 0.0f, 0.0f, -1.0f ), 0.5f, std::make_shared<lambertian>( vec3( 0.1f, 0.2f, 0.5f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 0.0f, -100.5f, -1.0f ), 100.0f, std::make_shared<lambertian>( vec3( 0.8f, 0.8f, 0.0f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 1.0f, 0.0f, -1.0f ), 0.5f, std::make_shared<metal>( vec3( 0.8f, 0.6f, 0.2f ), 0.3f ) ) );
	world.add( std::make_shared<sphere>( vec3( -1.0f, 0.0f, -1.0f ), 0.5f, std::make_shared<dielectric>( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -1.0f, 0.0f, -1.0f ), -0.45f, std::make_shared<dielectric>( 1.5f ) ) );
	return world;
}

hitable_list random_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, std::make_shared<lambertian>( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
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
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<lambertian>( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ) ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<metal>( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<dielectric>( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<dielectric>( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<lambertian>( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<metal>( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	return world;
}

hitable_list moving_sphere_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, std::make_shared<lambertian>( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
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
					world.add( std::make_shared<moving_sphere>( center, center2, 0.2f, std::make_shared<lambertian>( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ), 0.0f, 1.0f ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<metal>( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<dielectric>( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<dielectric>( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<lambertian>( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<metal>( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	return world;
}

hitable_list bvh_scene()
{
	hitable_list world;

	auto checker = std::make_shared<checker_texture>( vec3( 0.2f, 0.3f, 0.1f ), vec3( 0.9f, 0.9f, 0.9f ) );
	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, std::make_shared<lambertian>( checker ) ) );
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
					world.add( std::make_shared<moving_sphere>( center, center2, 0.2f, std::make_shared<lambertian>( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ), 0.0f, 1.0f ) );

				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<metal>( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<dielectric>( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<dielectric>( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<lambertian>( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<metal>( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	return hitable_list( std::make_shared<bvh_node>( world, 0.f, 1.f ) );
}

hitable_list plane_scene()
{
	hitable_list world;

	world.add( std::make_shared<sphere>( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, std::make_shared<lambertian>( vec3( 0.5f, 0.5f, 0.5f ) ) ) );
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
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<lambertian>( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ) ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<metal>( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) ) );
				}
				else // glass
				{
					world.add( std::make_shared<sphere>( center, 0.2f, std::make_shared<dielectric>( 1.5f ) ) );
				}
			}
		}
	}

	world.add( std::make_shared<sphere>( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<dielectric>( 1.5f ) ) );
	world.add( std::make_shared<sphere>( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<lambertian>( vec3( 0.4f, 0.2f, 0.1f ) ) ) );
	world.add( std::make_shared<sphere>( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, std::make_shared<metal>( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) ) );
	world.add( std::make_shared<plane>( vec3( 0.0f, 0.0f, -1.5f ), vec3( 0.0f, 0.0f, -1.0f ), std::make_shared<dielectric>( 1.5f ) ) );
	return world;
}

hitable_list triangle_scene()
{
	hitable_list world;
	world.add( std::make_shared<plane>( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ), std::make_shared<lambertian>( vec3( 0.0f, 0.2f, 0.8f ) ) ) );

	const vec3 bottom_left( -1.0f, 0.0f, -1.0f );
	const vec3 bottom_right( 1.0f, 0.0f, -1.0f );
	const vec3 top_left( -1.0f, 0.0f, 1.0f );
	const vec3 top_right( 1.0f, 0.0f, 1.0f );
	const vec3 point( 0.0f, 1.7f, 0.0f );

	world.add( std::make_shared<triangle>( bottom_left, bottom_right, point, std::make_shared<metal>( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );
	world.add( std::make_shared<triangle>( bottom_left, top_left, point, std::make_shared<metal>( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );
	world.add( std::make_shared<triangle>( top_left, top_right, point, std::make_shared<metal>( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );
	world.add( std::make_shared<triangle>( top_right, bottom_right, point, std::make_shared<metal>( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) ) );

	return hitable_list( std::make_shared<bvh_node>( world, 0.f, 1.f ) );
}

hitable_list empty_scene()
{
	hitable_list world;
	return world;
}

hitable_list two_spheres_scene()
{
	hitable_list objects;

	auto checker = std::make_shared<checker_texture>( vec3( 0.f, 0.3f, 0.1f ), vec3( 0.9f, 0.9f, 0.9f ) );

	objects.add( std::make_shared<sphere>( vec3( 0.f, -10.f, 0.f ), 10.f, std::make_shared<lambertian>( checker ) ) );
	objects.add( std::make_shared<sphere>( vec3( 0.f, 10.f, 0.f ), 10.f, std::make_shared<lambertian>( checker ) ) );

	return objects;
}

hitable_list two_perlin_spheres_scene()
{
	hitable_list objects;

	auto pertex = std::make_shared<noise_texture>( 4.f );

	objects.add( std::make_shared<sphere>( vec3( 0.f, -1000.f, 0.f ), 1000.f, std::make_shared<lambertian>( pertex ) ) );
	//objects.add( std::make_shared<sphere>( vec3( 0.f, -1000.f, 0.f ), 1000.f, std::make_shared<dielectric>( 1.5f ) ) );
	objects.add( std::make_shared<sphere>( vec3( 0.f, 2.f, 0.f ), 2.f, std::make_shared<lambertian>( pertex ) ) );

	return objects;
}

hitable_list earth_scene()
{
	auto earth_texture = std::make_shared<image_texture>( "planets//earthmap.jpg" );
	auto earth_surface = std::make_shared<lambertian>( earth_texture );
	auto globe = std::make_shared<sphere>( vec3( 0.f, 0.f, 0.f ), 2.f, earth_surface );
	return hitable_list( globe );
}

hitable_list simple_light_scene()
{
	hitable_list objects;

	auto pertex = std::make_shared<noise_texture>( 4.f );
	objects.add( std::make_shared<sphere>( vec3( 0.f, -1000.f, 0.f ), 1000.f, std::make_shared<lambertian>( pertex ) ) );
	objects.add( std::make_shared<sphere>( vec3( 0.f, 2.f, 0.f ), 2.f, std::make_shared<lambertian>( pertex ) ) );

	auto difflight = std::make_shared<diffuse_light>( vec3( 4.f, 4.f, 4.f ) );
	//objects.add( std::make_shared<xy_rect>( 3.f, 5.f, 1.f, 3.f, -2.f, difflight ) );
	objects.add( std::make_shared<sphere>( vec3( 0.f, 8.f, 0.f ), 2.f, difflight ) );

	return objects;
}

hitable_list cornell_box_scene()
{
	hitable_list objects;

	auto red = std::make_shared<lambertian>( vec3( 0.65f, 0.05f, 0.05f ) );
	auto white = std::make_shared<lambertian>( vec3( 0.73f, 0.73f, 0.73f ) );
	auto green = std::make_shared<lambertian>( vec3( 0.12f, 0.45f, 0.15f ) );
	auto light = std::make_shared<diffuse_light>( vec3( 15.f, 15.f, 15.f ) );

	objects.add( std::make_shared<yz_rect>( 0.f, 555.f, 0.f, 555.f, 555.f, green ) );
	objects.add( std::make_shared<yz_rect>( 0.f, 555.f, 0.f, 555.f, 0.f, red ) );
	objects.add( std::make_shared<xz_rect>( 213.f, 343.f, 227.f, 332.f, 554.f, light ) );
	objects.add( std::make_shared<xz_rect>( 0.f, 555.f, 0.f, 555.f, 0.f, white ) );
	objects.add( std::make_shared<xz_rect>( 0.f, 555.f, 0.f, 555.f, 555.f, white ) );
	objects.add( std::make_shared<xy_rect>( 0.f, 555.f, 0.f, 555.f, 555.f, white ) );

	std::shared_ptr<hitable> box1 = std::make_shared<box>( vec3( 0.f, 0.f, 0.f ), vec3( 165.f, 330.f, 165.f ), white );
	box1 = std::make_shared<rotate_y>( box1, 15.f );
	box1 = std::make_shared<translate>( box1, vec3( 265.f, 0.f, 295.f ) );
	objects.add( box1 );

	std::shared_ptr<hitable> box2 = std::make_shared<box>( vec3( 0.f, 0.f, 0.f ), vec3( 165.f, 165.f, 165.f ), white );
	box2 = std::make_shared<rotate_y>( box2, -18.f );
	box2 = std::make_shared<translate>( box2, vec3( 130.f, 0.f, 65.f ) );
	objects.add( box2 );

	return objects;
}

hitable_list cornell_box_smoke_scene()
{
	hitable_list objects;

	auto red = std::make_shared<lambertian>( vec3( 0.65f, 0.05f, 0.05f ) );
	auto white = std::make_shared<lambertian>( vec3( 0.73f, 0.73f, 0.73f ) );
	auto green = std::make_shared<lambertian>( vec3( 0.12f, 0.45f, 0.15f ) );
	auto light = std::make_shared<diffuse_light>( vec3( 7.f, 7.f, 7.f ) );

	objects.add( std::make_shared<yz_rect>( 0.f, 555.f, 0.f, 555.f, 555.f, green ) );
	objects.add( std::make_shared<yz_rect>( 0.f, 555.f, 0.f, 555.f, 0.f, red ) );
	objects.add( std::make_shared<xz_rect>( 113.f, 443.f, 127.f, 432.f, 554.f, light ) );
	objects.add( std::make_shared<xz_rect>( 0.f, 555.f, 0.f, 555.f, 0.f, white ) );
	objects.add( std::make_shared<xz_rect>( 0.f, 555.f, 0.f, 555.f, 555.f, white ) );
	objects.add( std::make_shared<xy_rect>( 0.f, 555.f, 0.f, 555.f, 555.f, white ) );

	std::shared_ptr<hitable> box1 = std::make_shared<box>( vec3( 0.f, 0.f, 0.f ), vec3( 165.f, 330.f, 165.f ), white );
	box1 = std::make_shared<rotate_y>( box1, 15.f );
	box1 = std::make_shared<translate>( box1, vec3( 265.f, 0.f, 295.f ) );

	std::shared_ptr<hitable> box2 = std::make_shared<box>( vec3( 0.f, 0.f, 0.f ), vec3( 165.f, 165.f, 165.f ), white );
	box2 = std::make_shared<rotate_y>( box2, -18.f );
	box2 = std::make_shared<translate>( box2, vec3( 130.f, 0.f, 65.f ) );

	objects.add( std::make_shared<constant_medium>( box1, 0.01f, vec3( 0.f, 0.f, 0.f ) ) );
	objects.add( std::make_shared<constant_medium>( box2, 0.01f, vec3( 1.f, 1.f, 1.f ) ) );

	return objects;
}

hitable_list final_scene()
{
	hitable_list boxes1;
	auto ground = std::make_shared<lambertian>( vec3( 0.48f, 0.83f, 0.53f ) );

	const int boxes_per_side = 20;
	for ( int i = 0; i < boxes_per_side; i++ ) {
		for ( int j = 0; j < boxes_per_side; j++ ) {
			auto w = 100.f;
			auto x0 = -1000.f + i * w;
			auto z0 = -1000.f + j * w;
			auto y0 = 0.f;
			auto x1 = x0 + w;
			auto y1 = random_float( 1.f, 101.f );
			auto z1 = z0 + w;

			boxes1.add( std::make_shared<box>( vec3( x0, y0, z0 ), vec3( x1, y1, z1 ), ground ) );
		}
	}

	hitable_list objects;

	objects.add( std::make_shared<bvh_node>( boxes1, 0.f, 1.f ) );

	auto light = std::make_shared<diffuse_light>( vec3( 7.f, 7.f, 7.f ) );
	objects.add( std::make_shared<xz_rect>( 123.f, 423.f, 147.f, 412.f, 554.f, light ) );

	auto center1 = vec3( 400.f, 400.f, 200.f );
	auto center2 = center1 + vec3( 30.f, 0.f, 0.f );
	auto moving_sphere_material = std::make_shared<lambertian>( vec3( 0.7f, 0.3f, 0.1f ) );
	objects.add( std::make_shared<moving_sphere>( center1, center2, 50.f, moving_sphere_material, 0.f, 1.f ) );

	objects.add( std::make_shared<sphere>( vec3( 260.f, 150.f, 45.f ), 50.f, std::make_shared<dielectric>( 1.5f ) ) );
	objects.add( std::make_shared<sphere>(
		vec3( 0.f, 150.f, 145.f ), 50.f, std::make_shared<metal>( vec3( 0.8f, 0.8f, 0.9f ), 1.f )
		) );

	auto boundary = std::make_shared<sphere>( vec3( 360.f, 150.f, 145.f ), 70.f, std::make_shared<dielectric>( 1.5f ) );
	objects.add( boundary );
	objects.add( std::make_shared<constant_medium>( boundary, 0.2f, vec3( 0.2f, 0.4f, 0.9f ) ) );
	boundary = std::make_shared<sphere>( vec3( 0.f, 0.f, 0.f ), 5000.f, std::make_shared<dielectric>( 1.5f ) );
	objects.add( std::make_shared<constant_medium>( boundary, .0001f, vec3( 1.f, 1.f, 1.f ) ) );

	auto emat = std::make_shared<lambertian>( std::make_shared<image_texture>( "planets//earthmap.jpg" ) );
	objects.add( std::make_shared<sphere>( vec3( 400.f, 200.f, 400.f ), 100.f, emat ) );
	auto pertext = std::make_shared<noise_texture>( 0.1f );
	objects.add( std::make_shared<sphere>( vec3( 220.f, 280.f, 300.f ), 80.f, std::make_shared<lambertian>( pertext ) ) );

	hitable_list boxes2;
	auto white = std::make_shared<lambertian>( vec3( .73f, .73f, .73f ) );
	int ns = 1000;
	for ( int j = 0; j < ns; j++ ) {
		boxes2.add( std::make_shared<sphere>( vec3( random_float( 0.f, 165.f ), random_float( 0.f, 165.f ), random_float( 0.f, 165.f ) ), 10.f, white ) );
	}

	objects.add( std::make_shared<translate>(
		std::make_shared<rotate_y>(
			std::make_shared<bvh_node>( boxes2, 0.f, 1.f ), 15.f ),
		vec3( -100.f, 270.f, 395.f )
		)
	);

	return objects;
}
