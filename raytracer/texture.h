#pragma once

#include "vec3.h"
#include "perlin.h"
#include "include_stb_image.h"

class texture
{
public:
	virtual vec3 value( float u, float v, const vec3& p ) const = 0;
};

class solid_color : public texture
{
public:
	solid_color() {}
	solid_color( vec3 c ) : color_value( c ) {}
	solid_color( float red, float green, float blue ) : solid_color( vec3( red, green, blue ) ) {}

	virtual vec3 value( float u, float v, const vec3& p ) const override
	{
		return color_value;
	}

private:
	vec3 color_value;
};

class checker_texture : public texture
{
public:
	checker_texture() {}
	checker_texture( std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd ) : even( _even ), odd( _odd ) {}
	checker_texture( vec3 c1, vec3 c2 ) : even( std::make_shared<solid_color>( c1 ) ), odd( std::make_shared<solid_color>( c2 ) ) {}

	virtual vec3 value( float u, float v, const vec3& p ) const override
	{
		auto sines = sin( 10.f * p.x() ) * sin( 10.f * p.y() ) * sin( 10.f * p.z() );
		if ( sines < 0 )
		{
			return odd->value( u, v, p );
		}
		else
		{
			return even->value( u, v, p );
		}
	}

	std::shared_ptr<texture> odd;
	std::shared_ptr<texture> even;
};

class noise_texture : public texture
{
public:
	noise_texture() {}
	noise_texture( float sc ) : scale( sc ) {}

	virtual vec3 value( float u, float v, const vec3& p ) const override
	{
		return vec3( 1.f, 1.f, 1.f ) * 0.5f * ( 1.f + sin( scale * p.z() + 10.f * noise.turb( scale * p ) ) );
	}

	perlin noise;
	float scale;
};

float clamp_float( float val, float min, float max )
{
	if ( val < min )
		return min;

	if ( val > max )
		return max;

	return val;
}

class image_texture : public texture
{
public:
	const static int bytes_per_pixel = 3;
	
	image_texture()
		: data( nullptr ), width( 0 ), height( 0 ), bytes_per_scanline( 0 ) {}

	image_texture( const char* filename )
	{
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load( filename, &width, &height, &components_per_pixel, components_per_pixel );

		if ( !data )
		{
			width = height = 0;
		}

		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture()
	{
		delete data;
	}

	virtual vec3 value( float u, float v, const vec3& p ) const override
	{
		if ( data == nullptr )
			return vec3( 0.f, 1.f, 1.f );

		u = clamp_float( u, 0.f, 1.f );
		v = 1.f - clamp_float( v, 0.f, 1.f );

		auto i = static_cast< int >( u * width );
		auto j = static_cast< int >( v * height );

		if ( i >= width ) i = width - 1;
		if ( j >= height ) j = height - 1;

		const auto color_scale = 1.f / 255.f;
		auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
		return vec3( color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2] );
	}

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};