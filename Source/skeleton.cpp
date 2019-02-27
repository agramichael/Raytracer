#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>
#include <glm/gtx/transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define FULLSCREEN_MODE false

struct Intersection
{
	vec4 position;
	float distance;
	int triangleIndex;
};

/* ----------------------------------------------------------------------------*/
/* GLOBALS                                                                     */
vector<Triangle> triangles;
float focalLength = SCREEN_HEIGHT;
vec4 cameraPos( 0.0, 0.0, -3.0, 1.0 );
mat4 R;
const float yaw = 5 * M_PI / 180;
vec4 lightPos( 0, -0.5, -0.7, 1.0 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.5f * vec3( 1, 1, 1 );
const float shadow_bias = 0.01;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update();
void Draw(screen* screen);
bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
mat4 lookAt();
vec3 DirectLight( const Intersection& i );
void update_R(float y);

int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  LoadTestModel( triangles );
	R = mat4(
		cos(0), 0, -sin(0), 0,
		0     , 1, 0      , 0,
		sin(0), 0, cos(0) , 0,
		0     , 0, 0      , 1
	);

  while( Update() )
    {
      Draw(screen);
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen)
{
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));


  for( int x = 0; x < SCREEN_WIDTH; x++ )
  {
    for( int y = 0; y < SCREEN_HEIGHT; y++ )
    {
			vec3 color;
			vec4 start = cameraPos;
			// for( int i = 0; i < 4; i++ ) {
			// 	for( int j = 0; j < 4; j++ ) {

					float px = -(x - SCREEN_WIDTH/2);
					float py = y - SCREEN_HEIGHT/2;

					vec4 pixel = vec4(px,py,-focalLength,1);
					pixel = lookAt()*pixel;
					vec4 dir = normalize(pixel - start);

					Intersection closestIntersection;
		      if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
						color += (DirectLight( closestIntersection ) + indirectLight) * triangles[closestIntersection.triangleIndex].color;
		      }
			// 	}
			// }
			// color /= 16;
			PutPixelSDL(screen, x, y, color);
    }
  }
}

void update_R(float yaw) {
	float s = sin(yaw);
  R[0][0] = R[2][2] = cos(yaw);
  R[0][2] = -s;
  R[2][0] = s;
}

/*Place updates of parameters here*/
bool Update()
{
//  static int t = SDL_GetTicks();
//  /* Compute frame time */
//  int t2 = SDL_GetTicks();
//  float dt = float(t2-t);
//  t = t2;

  //cout << "Render time: " << dt << " ms." << endl;o
	mat4 camToWorld = lookAt();
	vec4 right = -vec4( camToWorld[0][0], camToWorld[0][1], camToWorld[0][2], 1 );
	vec4 down( camToWorld[1][0], camToWorld[1][1], camToWorld[1][2], 1 );
	vec4 forward = -vec4( camToWorld[2][0], camToWorld[2][1], camToWorld[2][2], 1 );

  SDL_Event e;
  while(SDL_PollEvent(&e))
    {
      if (e.type == SDL_QUIT)
	{
	  return false;
	}
      else
	if (e.type == SDL_KEYDOWN)
	  {
	    int key_code = e.key.keysym.sym;
	    switch(key_code)
	      {
	      case SDLK_UP:
		      /* Move camera forward */
          cameraPos += yaw * forward;
		      break;
	      case SDLK_DOWN:
		      /* Move camera backwards */
          cameraPos -= yaw * forward;
		      break;
	      case SDLK_LEFT:
		      /* Move camera left */
					update_R(yaw);
					cameraPos = R * cameraPos;
		      break;
	      case SDLK_RIGHT:
		      /* Move camera right */
					update_R(-yaw);
					cameraPos = R * cameraPos;
		      break;
	      case SDLK_ESCAPE:
					// quit
		      return false;
				case SDLK_w:
					// move light forwards
					lightPos += yaw * forward;
					break;
				case SDLK_s:
					// move light backwards
					lightPos -= yaw * forward;
					break;
				case SDLK_a:
					// move light left
					lightPos -= yaw * right;
					break;
				case SDLK_d:
					// move light right
					lightPos += yaw * right;
					break;
				case SDLK_q:
					// move light up
					lightPos -= yaw * down;
					break;
				case SDLK_e:
					// move light down
					lightPos += yaw * down;
					break;
	      }
	  }
    }
return true;
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

vec3 DirectLight( const Intersection& i ) {
	// look for shadows
	vec4 start = i.position;
	vec4 dir = normalize(lightPos - i.position);
	start += (shadow_bias * dir);
	Intersection closest_intersection;
	if ( ClosestIntersection( start, dir, triangles, closest_intersection ) ) {
		if ( closest_intersection.distance < glm::length(lightPos - start) ) {
			return vec3( 0.0, 0.0, 0.0 );

			// } else {
			// 	cout << "(" << triangles[closest_intersection.triangleIndex].normal.x << ", "
      //      << triangles[closest_intersection.triangleIndex].normal.y << ", "
      //      << triangles[closest_intersection.triangleIndex].normal.z << ")" << "\n";
			// 	cout << "(" << n.x << ", "
			// 	  << n.y << ", "
			// 	  << n.z << ")" << "\n";
			// }
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
