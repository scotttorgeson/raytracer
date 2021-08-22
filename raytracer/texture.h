#pragma once

#include "vec3.h"
#include "perlin.h"

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

	virtual vec3 value( float u, float v, const vec3& p ) const override
	{
		return vec3( 1.f, 1.f, 1.f ) * noise.noise( p );
	}

	perlin noise;
};