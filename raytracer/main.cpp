#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"

std::default_random_engine generator;
std::uniform_real_distribution<float> distr( 0.0f, 1.0f );
float drand48()
{
	float ret;
	
	do
	{
		ret = distr( generator );
	} while ( ret == 1.0f );

	return ret;
}

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

	//int nx = 200;
	//int ny = 100;

	//const int nx = 800;
	//const int ny = 400;

	//int nx = 1200;
	//int ny = 800;

	int nx = 1920;
	int ny = 1080;

	const int ns = 200;
	const int threadCount = 4;

	hitable_list *world = random_scene();

	const vec3 lookfrom( 13.0f, 2.0f, 3.0f );
	const vec3 lookat( 0.0f, 0.0f, 0.0f );
	const float dist_to_focus = 10.0f;
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

	for ( int i = 0; i < threadCount; i++ )
	{
		delete[] images[i];
	}
	delete[] images;

	for ( int i = 0; i < world->list_size; i++ )
	{
		sphere* sphere_hitable = reinterpret_cast< sphere* >( world->list[i] );
		delete sphere_hitable->mat_ptr;
		delete sphere_hitable;
	}

	delete[] world->list;
	delete world;

	return 0;
}