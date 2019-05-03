#include "raytracer.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec3;
using glm::mat4;

extern vec3 lightPos;
extern vec3 lightColor;
extern vec3 indirectLight;
extern mat3 R;
extern vector<Triangle> triangles;
extern vec3 cameraPos;
extern vec3 lightSample[SOFT_SHADOW_SAMPLES];

// update rotation matrix
void update_R(float yaw) {
	float s = sin(yaw);
  	R[0][0] = R[2][2] = cos(yaw);
  	R[0][2] = s;
  	R[2][0] = -s;
}

// calculate total illumination
vec3 Light( const Intersection& i ) {
	return DirectLight( i ) + indirectLight;
}

// calculate direct illumination
vec3 DirectLight( const Intersection& i ) {
	// look for shadows
	vec3 directIlluminationSum(0.0f, 0.0f, 0.0f);
	vec3 start = i.position;

	for( int j = 0; j < SOFT_SHADOW_SAMPLES; j++ ) {
		vec3 dir = glm::normalize(lightSample[j] - i.position);
		start += (float(shadow_bias) * dir);
		Intersection closest_intersection;
		vec3 color = lightColor;
		if ( ClosestIntersection( start, dir, triangles, closest_intersection ) ) {
			if ( closest_intersection.distance < glm::length(lightSample[j] - start) ) {
				color = vec3(0, 0, 0);
			}
		}

		// calculate direct lighting
		vec3 normal = triangles[i.triangleIndex].normal;
		float radius = glm::distance(lightSample[j], i.position);
		vec3 r = normalize(lightPos - i.position);
		float x = max( dot(r, normal), float(0.0) ) / ( 4 * M_PI * pow(radius, 2) );
		vec3 D =  x * color;
		directIlluminationSum += D;
	}
	directIlluminationSum /= SOFT_SHADOW_SAMPLES;
	return directIlluminationSum;
}

// find closest intersection shooting ray from start with direction "dir"
bool ClosestIntersection( vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection )
{
  bool flag = false;

  float m = std::numeric_limits<float>::max();

  for (size_t i = 0; i < triangles.size(); i++)
  {
    vec3 v0 = triangles[i].v0;
    vec3 v1 = triangles[i].v1;
    vec3 v2 = triangles[i].v2;
    vec3 e1 = vec3( v1.x-v0.x,v1.y-v0.y,v1.z-v0.z );
    vec3 e2 = vec3( v2.x-v0.x,v2.y-v0.y,v2.z-v0.z );
    vec3 b = vec3( start.x-v0.x, start.y-v0.y, start.z-v0.z );
    mat3 A( vec3(-dir), e1, e2 );
    vec3 x = glm::inverse( A ) * b;

    if( x.y >= 0 && x.z >= 0 && (x.y+x.z) <= 1 && x.x >= 0) {
      if(x.x < m){
        closestIntersection.distance = x.x;
        m = closestIntersection.distance;
        closestIntersection.position = v0 + x.y*e1 + x.z*e2;
        closestIntersection.triangleIndex = i;
        flag = true;
      }
    }
  }

  return flag;
}

// calculate camtoworld matrix from camerapos to (0,0,0)
mat4 lookAt() {
	vec3 tmp = vec3(0,1,0);
	vec3 to = vec3(0.0, 0.0, 0.0);
	vec3 from = cameraPos;
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

// generate/update light samples array
void generateLightSample() {
    lightSample[0] = lightPos;

    #pragma omp parallel for
    for (int i = 1; i < SOFT_SHADOW_SAMPLES; ++i)
    {
        float x = glm::linearRand(-SOFT_SHADOW_OFFSET, SOFT_SHADOW_OFFSET);
        float y = glm::linearRand(-SOFT_SHADOW_OFFSET, SOFT_SHADOW_OFFSET);
        float z = glm::linearRand(-SOFT_SHADOW_OFFSET, SOFT_SHADOW_OFFSET);
        lightSample[i] = vec3(lightPos[0] + x, lightPos[1] + y, lightPos[2] + z);
    }
}
