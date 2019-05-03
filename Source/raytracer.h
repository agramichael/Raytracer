#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <stdint.h>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <vector>
#include <glm/gtc/random.hpp>
#include "SDLauxiliary.h"
#include "TestModelH.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

#define _USE_MATH_DEFINES
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FULLSCREEN_MODE false
#define focalLength SCREEN_HEIGHT
#define shadow_bias 0.01
#define SSAA 4
#define SOFT_SHADOW_SAMPLES 4
#define SOFT_SHADOW_OFFSET 0.02f
#define APERTURE 3
#define DOF_SAMPLES 4

vec3 cameraPos = vec3( 0.0, 0.0, -3.0 );
vector<Triangle> triangles;
mat3 R = mat3(
		cos(0) , 0, sin(0),
		0      , 1, 0     ,
		-sin(0), 0, cos(0)
	);
float SSAA_INV = float(1)/SSAA;
const float yaw (5 * M_PI / 180);
vec3 lightPos = vec3( 0, -0.5, -0.7 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.5f * vec3( 1, 1, 1 );
vec3 lightSample[SOFT_SHADOW_SAMPLES];

struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

#endif