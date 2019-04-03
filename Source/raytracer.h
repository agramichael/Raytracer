#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <stdint.h>
#include <glm/gtx/transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <glm/gtc/random.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FULLSCREEN_MODE false
#define focalLength SCREEN_HEIGHT
#define shadow_bias 0.01
#define SSAA 4
#define SOFT_SHADOW_SAMPLES 10
#define SOFT_SHADOW_OFFSET 0.02f

struct Intersection
{
	vec4 position;
	float distance;
	int triangleIndex;
};

class Triangle
{
public:
	glm::vec4 v0;
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 normal;
	glm::vec3 color;

	Triangle( glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
	  glm::vec3 e1 = glm::vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
	  glm::vec3 e2 = glm::vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
	  glm::vec3 normal3 = glm::normalize( glm::cross( e2, e1 ) );
	  normal.x = normal3.x;
	  normal.y = normal3.y;
	  normal.z = normal3.z;
	  normal.w = 1.0;
	}
};


// FUNCTIONS
bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
mat4 lookAt();
vec3 DirectLight( const Intersection& i );
void update_R(float y);
vec3 Light( const Intersection& i );
float ShadowToLight(vec4 from, vec3 to);
void generateLightSample();

#endif