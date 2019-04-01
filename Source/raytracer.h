#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "SDL.h"
#include <glm/glm.hpp>
#include "TestModelH.h"
#include "SDLauxiliary.h"
#include "globals.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

extern vec4 lightPos;
extern vec3 lightColor;
extern vec3 indirectLight;
extern vec4 cameraPos;
extern vector<Triangle> triangles;
extern mat4 R;
extern float SSAA_INV;
extern float occlusion_distance;

// FUNCTIONS
bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
mat4 lookAt();
vec3 DirectLight( const Intersection& i );
void update_R(float y);
vec3 Light( const Intersection& i );

void update_R(float yaw) {
	float s = sin(yaw);
  	R[0][0] = R[2][2] = cos(yaw);
  	R[0][2] = -s;
  	R[2][0] = s;
}

vec3 Light( const Intersection& i ) {
	return DirectLight( i ) + indirectLight;
}

vec3 DirectLight( const Intersection& i ) {
	// look for shadows
	vec4 start = i.position;
	vec4 dir = normalize(lightPos - i.position);
	start += (float(shadow_bias) * dir);
	Intersection closest_intersection;
	if ( ClosestIntersection( start, dir, triangles, closest_intersection ) ) {
		occlusion_distance = 0;
		if ( closest_intersection.distance < glm::length(lightPos - start) ) {
			occlusion_distance = closest_intersection.distance;
			return vec3( 0.0, 0.0, 0.0 );
		}
	}

	// calculate direct lighting
	vec3 normal = vec3(triangles[i.triangleIndex].normal);
	float radius = glm::length(vec3(lightPos) - vec3(i.position));
	vec3 r = normalize(vec3(lightPos) - vec3(i.position));
	float x = max( dot(r, normal), float(0.0) ) / ( 4 * M_PI * pow(radius, 2) );
	vec3 D =  x * lightColor;
	return D;
}


bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection )
{
  bool flag = false;

  float m = std::numeric_limits<float>::max();

  for (size_t i = 0; i < triangles.size(); i++)
  {
    vec4 v0 = triangles[i].v0;
    vec4 v1 = triangles[i].v1;
    vec4 v2 = triangles[i].v2;
    vec3 e1 = vec3( v1.x-v0.x,v1.y-v0.y,v1.z-v0.z );
    vec3 e2 = vec3( v2.x-v0.x,v2.y-v0.y,v2.z-v0.z );
    vec3 b = vec3( start.x-v0.x, start.y-v0.y, start.z-v0.z );
    mat3 A( vec3(-dir), e1, e2 );
    vec3 x = glm::inverse( A ) * b;

    if( x.y >= 0 && x.z >= 0 && (x.y+x.z) <= 1 && x.x >= 0) {
      if(x.x < m){
        closestIntersection.distance = x.x;
        m = closestIntersection.distance;
        closestIntersection.position = v0 + x.y*vec4(e1.x,e1.y,e1.z,0) + x.z*vec4(e2.x,e2.y,e2.z,0);
        closestIntersection.triangleIndex = i;
        flag = true;
      }
    }
  }

  return flag;
}


mat4 lookAt() {
	vec3 tmp = vec3(0,1,0);
	vec3 to = vec3(0.0, 0.0, 0.0);
	vec3 from = vec3(cameraPos);
	vec3 forward = normalize(from - to);
	vec3 right = cross(normalize(tmp), forward);
	vec3 up = cross(forward, right);

	mat4 camToWorld;
	camToWorld[0][0] = right.x;
	camToWorld[0][1] = right.y;
	camToWorld[0][2] = right.z;
	camToWorld[0][3] = 0;

	camToWorld[1][0] = up.x;
	camToWorld[1][1] = up.y;
	camToWorld[1][2] = up.z;
	camToWorld[1][3] = 0;

	camToWorld[2][0] = forward.x;
	camToWorld[2][1] = forward.y;
	camToWorld[2][2] = forward.z;
	camToWorld[2][3] = 0;

	camToWorld[3][0] = from.x;
	camToWorld[3][1] = from.y;
	camToWorld[3][2] = from.z;
	camToWorld[3][3] = 1;

	return camToWorld;
}

#endif