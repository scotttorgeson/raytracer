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
#include "scenes.h"

#include <SDL.h>
#include <SDL_syswm.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Surface* surface = nullptr;
SDL_DisplayMode displayMode;
SDL_SysWMinfo wmInfo;
bool quit = false;

Uint32 timer_callback( Uint32 interval, void* param )
{
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = nullptr;
	userevent.data2 = nullptr;
	event.type = SDL_USEREVENT;
	event.user = userevent;
	SDL_PushEvent( &event );
	return interval;
}

bool init_sdl( int windowWidth, int windowHeight )
{
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		return false;

	if ( SDL_GetDesktopDisplayMode( 0, &displayMode ) != 0 )
		return false;

	int rendererIndex = -1;
	for ( int i = 0; i < SDL_GetNumRenderDrivers(); i++ )
	{
		SDL_RendererInfo rendererInfo = {};
		SDL_GetRenderDriverInfo( i, &rendererInfo );

		if ( strcmp( rendererInfo.name, "direct3d" ) == 0 )
		{
			rendererIndex = i;
			break;
		}
	}

	if ( rendererIndex < 0 )
		return false;

	window = SDL_CreateWindow( "raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN );
	if ( !window )
		return false;

	renderer = SDL_CreateRenderer( window, rendererIndex, SDL_RENDERER_ACCELERATED );
	if ( !renderer )
		return false;

	SDL_VERSION( &wmInfo.version );
	if ( !SDL_GetWindowWMInfo( window, &wmInfo ) )
		return false;

	surface = SDL_CreateRGBSurface( 0, windowWidth, windowHeight, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
	if ( !surface )
		return false;

	SDL_AddTimer( 1000, timer_callback, nullptr );

	return true;
}

void cleanup_sdl()
{
	if ( surface )
	{
		SDL_FreeSurface( surface );
		surface = nullptr;
	}

	if ( renderer )
	{
		SDL_DestroyRenderer( renderer );
		renderer = nullptr;
	}

	if ( window )
	{
		SDL_DestroyWindow( window );
		window = nullptr;
	}

	SDL_Quit();
}

vec3 ray_color( const ray& r, const vec3& background, const hitable& world, int depth )
{
	hit_record rec;

	if ( depth <= 0 )
		return vec3( 0.f, 0.f, 0.f );

	if ( !world.hit( r, 0.001f, FLT_MAX, rec ) )
		return background;

	ray scattered;
	vec3 attenuation;
	vec3 emitted = rec.mat_ptr->emitted( rec.u, rec.v, rec.p );

	if ( !rec.mat_ptr->scatter( r, rec, attenuation, scattered ) )
		return emitted;

	return emitted + attenuation * ray_color( scattered, background, world, depth - 1 );
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

void raytrace( camera* cam, const int ny, const int nx, const int ns, hitable* world, pixel* im, vec3 background, int max_depth )
{
	for ( int j = ny - 1; j >= 0; j-- )
	{
		for ( int i = 0; i < nx; i++ )
		{
			vec3 col( 0.0f, 0.0f, 0.0f );
			for ( int s = 0; s < ns; s++ )
			{
				if ( quit )
					return;

				float u = float( i + drand48() ) / float( nx );
				float v = float( j + drand48() ) / float( ny );

				ray r = cam->get_ray( u, v );
				vec3 p = r.point_at_parameter( 2.0f );
				//col += color( r, world, 0 );
				col += ray_color( r, background, *world, max_depth );
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

void raytrace_thread( int nx, int ny, int ns, int threadCount, pixel** images, char* imageFilePath )
{
	//hitable_list world = random_scene();
	//hitable_list world = plane_scene();
	//hitable_list world = triangle_scene();
	//hitable_list world = empty_scene();
	//hitable_list world = moving_sphere_scene();
	//hitable_list world = bvh_scene();
	//hitable_list world = two_spheres_scene();
	//hitable_list world = two_perlin_spheres_scene();
	//hitable_list world = earth_scene();
	//hitable_list world = simple_light_scene();
	hitable_list world = cornell_box_scene();

	//vec3 background( 0.7f, 0.8f, 1.f );
	vec3 background( 0.f, 0.f, 0.f );

	const int max_depth = 50;
	//const vec3 lookfrom( 13.0f, 2.0f, 3.0f );
	//const vec3 lookat( 0.0f, 0.0f, 0.0f );

	//const vec3 lookfrom( 26.0f, 3.0f, 6.0f );
	//const vec3 lookat( 0.0f, 2.0f, 0.0f );

	const vec3 lookfrom( 278.f, 278.f, -800.f );
	const vec3 lookat( 278.f, 278.f, 0.f );
	const float vfov = 40.f;

	//const float dist_to_focus = 10.0f;
	const float dist_to_focus = lookfrom.length();
	const float aperture = 0.1f;
	//const float vfov = 20.0f;
	camera cam( lookfrom, lookat, vec3( 0.0f, 1.0f, 0.0f ), vfov, float( nx ) / float( ny ), aperture, dist_to_focus, 0.0, 1.0 );

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

		threads[i] = std::thread( raytrace, &cam, ny, nx, samples, &world, images[i], background, max_depth );
	}

	for ( int i = 0; i < threadCount; i++ )
	{
		threads[i].join();
	}
	timer.Stop();

	if ( !quit )
	{
		std::cout << "Finished rendering in " << timer.GetSeconds() << " seconds.\n";

		std::ofstream imageFile( imageFilePath, std::ios::trunc | std::ios::out );
		if ( !imageFile )
		{
			std::cout << "Failed to open file: " << imageFilePath << "\n";
		}
		else
		{
			std::cout << "Outputting to file: " << imageFilePath << "\n";

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
		}
	}

	delete [] threads;
}

// 32.5 minutes for 1920x1080, 200 samples per pixel, 4 threads
void high_quality( int& nx, int& ny, int& ns )
{
	nx = 1920;
	ny = 1080;
	ns = 200;
}

void mid_quality( int& nx, int& ny, int& ns )
{
	nx = 192 * 4;
	ny = 108 * 4;
	ns = 50;
}

// 14.5 seconds for 192*2x108*2, 20 samples per pixel, 3 threads
void fast_quality( int& nx, int& ny, int& ns )
{
	nx = 192*2;
	ny = 108*2;
	ns = 20;
}

void low_res_high_sample( int& nx, int& ny, int& ns )
{
	nx = 192 * 2;
	ny = 108 * 2;
	ns = 200;
}

void mid_res_high_sample( int& nx, int& ny, int& ns )
{
	nx = 192 * 4;
	ny = 108 * 4;
	ns = 200;
}

void high_res_high_sample( int& nx, int& ny, int& ns )
{
	nx = 1920;
	ny = 1080;
	ns = 200;
}

void square_low_res_high_sample( int& nx, int& ny, int& ns )
{
	nx = 600;
	ny = 600;
	ns = 200;
}

// http://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// https://github.com/petershirley/raytracinginoneweekend
int main(int argc, char* argv[])
{
#ifdef CHECK_MEM_LEAKS
	// Uncomment this and set it to the number of the allocation to track down, it will break when that allocation happens
	//_crtBreakAlloc = 1148;
#endif

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

	const int threadCount = 3;

	int nx, ny, ns;
	//high_quality( nx, ny, ns );
	//mid_quality( nx, ny, ns );
	//fast_quality( nx, ny, ns );
	//low_res_high_sample( nx, ny, ns );
	//mid_res_high_sample( nx, ny, ns );
	//high_res_high_sample( nx, ny, ns );
	square_low_res_high_sample( nx, ny, ns );

	if ( !init_sdl( nx, ny ) )
	{
		cleanup_sdl();
		return 0;
	}

	pixel** images = new pixel*[threadCount];
	for ( int i = 0; i < threadCount; i++ )
	{
		images[i] = new pixel[nx*ny];
	}

	std::thread raytraceThread = std::thread( raytrace_thread, nx, ny, ns, threadCount, images, argv[1] );

	for ( ;; )
	{
		SDL_Event e;
		while ( SDL_PollEvent( &e ) != 0 )
		{
			switch ( e.type )
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				{
					switch ( e.key.keysym.sym )
					{
					case SDLK_ESCAPE:
						quit = true;
						break;
					}
				}
			case SDL_USEREVENT:
				{
					SDL_LockSurface( surface );

					int pixelOffset = 0;
					for ( int j = ny - 1; j >= 0; j-- )
					{
						for ( int i = 0; i < nx; ++i )
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

							unsigned char copySource[4] = { static_cast< unsigned char >( ir ), static_cast< unsigned char >( ig ), static_cast< unsigned char >( ib ), 255 };
							SDL_memcpy( reinterpret_cast< char* >( surface->pixels ) + pixelOffset * 4, copySource, 4 );
							pixelOffset++;
						}
					}

					SDL_UnlockSurface( surface );

					SDL_BlitSurface( surface, nullptr, SDL_GetWindowSurface( window ), nullptr );
					SDL_UpdateWindowSurface( window );
					break;
				}
			}
		}

		if ( quit )
			break;
	}

	cleanup_sdl();

	raytraceThread.join();

	// todo: better memory management
	for ( int i = 0; i < threadCount; i++ )
	{
		delete[] images[i];
	}
	delete[] images;

#ifdef CHECK_MEM_LEAKS
	_CrtDumpMemoryLeaks();
#endif // CHECK_MEM_LEAKS

	return 0;
}