//#define CHECK_MEM_LEAKS
#ifdef CHECK_MEM_LEAKS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // CHECK_MEM_LEAKS

#include <iostream>
#include <fstream>
#include <thread>
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "Timer.h"

vec3 color( const ray& r, hitable *world, int depth )
{
	hit_record rec;
	if ( world->hit( r, 0.001f, FLT_MAX, rec ) )
	{
		ray scattered;
		vec3 attenuation;
		if ( depth < 50 && rec.mat_ptr->scatter( r, rec, attenuation, scattered ) )
			return attenuation * color( scattered, world, depth + 1 );
		else
			return vec3( 0.0f, 0.0f, 0.0f );
	}
	else
	{
		vec3 unit_direction = unit_vector( r.direction() );
		float t = 0.5f * ( unit_direction.y() + 1.0f );
		return ( 1.0f - t ) * vec3( 1.0f, 1.0f, 1.0f ) + t * vec3( 0.5f, 0.7f, 1.0f );
	}
}

hitable_list *simple_scene()
{
	hitable **list = new hitable*[4];
	list[0] = new sphere( vec3( 0.0f, 0.0f, -1.0f ), 0.5f, new lambertian( vec3( 0.1f, 0.2f, 0.5f ) ) );
	list[1] = new sphere( vec3( 0.0f, -100.5f, -1.0f ), 100.0f, new lambertian( vec3( 0.8f, 0.8f, 0.0f ) ) );
	list[2] = new sphere( vec3( 1.0f, 0.0f, -1.0f ), 0.5f, new metal( vec3( 0.8f, 0.6f, 0.2f ), 0.3f ) );
	list[3] = new sphere( vec3( -1.0f, 0.0f, -1.0f ), 0.5f, new dielectric( 1.5f ) );
	list[4] = new sphere( vec3( -1.0f, 0.0f, -1.0f ), -0.45f, new dielectric( 1.5f ) );
	return new hitable_list( list, 5 );
}

hitable_list *random_scene()
{
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) );
	int i = 1;
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
					list[i++] = new sphere( center, 0.2f, new lambertian( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					list[i++] = new sphere( center, 0.2f, new metal( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) );
				}
				else // glass
				{
					list[i++] = new sphere( center, 0.2f, new dielectric( 1.5f ) );
				}
			}
		}
	}

	list[i++] = new sphere( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) );
	list[i++] = new sphere( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) );
	list[i++] = new sphere( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) );
	return new hitable_list(list, i);
}

hitable_list *plane_scene()
{
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new plane( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ), new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) );
	int i = 1;
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
					list[i++] = new sphere( center, 0.2f, new lambertian( vec3( drand48() * drand48(), drand48() * drand48(), drand48() * drand48() ) ) );
				}
				else if ( choose_mat < 0.95f ) // metal
				{
					list[i++] = new sphere( center, 0.2f, new metal( vec3( 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1.0f + drand48() ), 0.5f * ( 1 + drand48() ) ), 0.5f * ( drand48() ) ) );
				}
				else // glass
				{
					list[i++] = new sphere( center, 0.2f, new dielectric( 1.5f ) );
				}
			}
		}
	}

	list[i++] = new sphere( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) );
	list[i++] = new sphere( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) );
	list[i++] = new sphere( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) );
	list[i++] = new plane( vec3( 0.0f, 0.0f, -1.5f ), vec3( 0.0f, 0.0f, -1.0f ), new dielectric( 1.5f ) );
	return new hitable_list( list, i );
}

hitable_list *triangle_scene()
{
	int n = 4;
	hitable **list = new hitable*[n + 1];
	list[0] = new plane( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ), new lambertian( vec3( 0.0f, 0.2f, 0.8f ) ) );
	int i = 1;

	const vec3 bottom_left( -1.0f, 0.0f, -1.0f );
	const vec3 bottom_right( 1.0f, 0.0f, -1.0f );
	const vec3 top_left( -1.0f, 0.0f, 1.0f );
	const vec3 top_right( 1.0f, 0.0f, 1.0f );
	const vec3 point( 0.0f, 1.7f, 0.0f );

	list[i++] = new triangle( bottom_left, bottom_right, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) );
	list[i++] = new triangle( bottom_left, top_left, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) );
	list[i++] = new triangle( top_left, top_right, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) );
	list[i++] = new triangle( top_right, bottom_right, point, new metal( vec3( 1.0f, 0.766f, 0.336f ), 0.1f ) );

	return new hitable_list( list, i );
}

hitable_list *empty_scene()
{
	return new hitable_list( nullptr, 0 );
}

struct pixel
{
	int ir;
	int ig;
	int ib;
};

inline pixel* get_pixel( pixel* im, int i, int j, int width )
{
	return &im[j * width + i];
}

void raytrace( camera* cam, const int ny, const int nx, const int ns, hitable* world, pixel* im )
{
	for ( int j = ny - 1; j >= 0; j-- )
	{
		for ( int i = 0; i < nx; i++ )
		{
			vec3 col( 0.0f, 0.0f, 0.0f );
			for ( int s = 0; s < ns; s++ )
			{
				float u = float( i + drand48() ) / float( nx );
				float v = float( j + drand48() ) / float( ny );

				ray r = cam->get_ray( u, v );
				vec3 p = r.point_at_parameter( 2.0f );
				col += color( r, world, 0 );
			}

			col /= float( ns );
			col = vec3( sqrt( col[0] ), sqrt( col[1] ), sqrt( col[2] ) );
			int ir = int( 255.99*col[0] );
			int ig = int( 255.99*col[1] );
			int ib = int( 255.99*col[2] );

			pixel* pix = get_pixel( im, i, j, nx );
			pix->ir = ir;
			pix->ig = ig;
			pix->ib = ib;
		}
	}
}

// 32.5 minutes for 1920x1080, 200 samples per pixel, 4 threads
void high_quality( int& nx, int& ny, int& ns )
{
	nx = 1920;
	ny = 1080;
	ns = 200;
}

// 14.5 seconds for 192*2x108*2, 20 samples per pixel, 3 threads
void fast_quality( int& nx, int& ny, int& ns )
{
	nx = 192*2;
	ny = 108*2;
	ns = 20;
}

// http://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// https://github.com/petershirley/raytracinginoneweekend
int main(int argc, char* argv[])
{
	std::cout << "Args: ";
	for ( int i = 0; i < argc; i++ )
	{
		std::cout << argv[i] << " ";
	}
	std::cout << "\n";

	if ( argc < 2 )
	{
		std::cout << "Please provide a filename\n";
		return 0;
	}

	std::ofstream imageFile( argv[1], std::ios::trunc | std::ios::out );
	if ( !imageFile )
	{
		std::cout << "Failed to open file: " << argv[1] << "\n";
		return 0;
	}

	std::cout << "Outputting to file: " << argv[1] << "\n";

	const int threadCount = 3;

	int nx, ny, ns;
	//high_quality( nx, ny, ns );
	fast_quality( nx, ny, ns );

	//hitable_list *world = random_scene();
	//hitable_list *world = plane_scene();
	hitable_list *world = triangle_scene();
	//hitable_list *world = empty_scene();

	const vec3 lookfrom( 13.0f, 2.0f, 3.0f );
	const vec3 lookat( 0.0f, 0.0f, 0.0f );
	//const float dist_to_focus = 10.0f;
	const float dist_to_focus = lookfrom.length();
	const float aperture = 0.1f;
	const float vfov = 20.0f;
	camera cam( lookfrom, lookat, vec3( 0.0f, 1.0f, 0.0f ), vfov, float( nx ) / float( ny ), aperture, dist_to_focus );

	pixel** images = new pixel*[threadCount];
	for ( int i = 0; i < threadCount; i++ )
	{
		images[i] = new pixel[nx*ny];
	}

	const int ns_per_thread = ns / threadCount;
	const int ns_remainder = ns % threadCount;

	std::thread* threads = new std::thread[threadCount];

	Timer timer;
	timer.Start();
	for ( int i = 0; i < threadCount; i++ )
	{
		int samples = ns_per_thread;
		if ( i == 0 )
			samples += ns_remainder;

		threads[i] = std::thread( raytrace, &cam, ny, nx, samples, world, images[i] );
	}

	for ( int i = 0; i < threadCount; i++ )
	{
		threads[i].join();
	}
	timer.Stop();
	std::cout << "Finished rendering in " << timer.GetSeconds() << " seconds.\n";

	imageFile << "P3\n" << nx << " " << ny << "\n255\n";
	for ( int j = ny - 1; j >= 0; j-- )
	{
		for ( int i = 0; i < nx; i++ )
		{
			int ir = 0;
			int ig = 0;
			int ib = 0;

			for ( int im_index = 0; im_index < threadCount; im_index++ )
			{
				pixel* pix = get_pixel( images[im_index], i, j, nx );
				ir += pix->ir;
				ig += pix->ig;
				ib += pix->ib;
			}

			ir /= threadCount;
			ig /= threadCount;
			ib /= threadCount;
			imageFile << ir << " " << ig << " " << ib << "\n";
		}
	}

	// todo: better memory management
	for ( int i = 0; i < threadCount; i++ )
	{
		delete[] images[i];
	}
	delete[] images;
	
	if ( world->list )
	{
		for ( int i = 0; i < world->list_size; i++ )
		{
			delete world->list[i];
		}

		delete[] world->list;
	}
	delete world;

	std::cout << "Press enter to continue...\n";
	char c = 0;
	while ( c != '\n' )
		c = std::cin.get();

#ifdef CHECK_MEM_LEAKS
	_CrtDumpMemoryLeaks();
#endif // CHECK_MEM_LEAKS

	return 0;
}