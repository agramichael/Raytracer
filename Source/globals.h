#ifndef GLOBALS_H
#define GLOBALS_H

#include "SDL.h"
#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

vec4 lightPos = vec4( 0, -0.5, -0.7, 1.0 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.5f * vec3( 1, 1, 1 );
vec4 cameraPos = vec4( 0.0, 0.0, -3.0, 1.0 );
vector<Triangle> triangles;
mat4 R;
float SSAA_INV;
const float yaw (5 * M_PI / 180);

struct Intersection
{
	vec4 position;
	float distance;
	int triangleIndex;
};

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FULLSCREEN_MODE false
#define focalLength SCREEN_HEIGHT
#define shadow_bias 0.01
#define SSAA 4


#endif