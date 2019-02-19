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


#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
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
float focalLength = 1.2;
vec4 cameraPos( 0.0, 0.0, -3.0, 1.0 );
mat4 R;
mat4 camToWorld;
float yaw;
vec4 lightPos( 0, -0.5, -0.7, 1.0 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 to = vec3(0.0, 0.0, 0.0);


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update();
void Draw(screen* screen);
bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
mat4 lookAt( vec3 from, vec3 to);
vec3 DirectLight( const Intersection& i );

int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  LoadTestModel( triangles );
	yaw = 0;
	R = mat4(
		cos(yaw), 0, -sin(yaw), 0,
		0       , 1, 0        , 0,
		sin(yaw), 0, cos(yaw) , 0,
		0       , 0, 0        , 1
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
			float px = -(2 * ((x + 0.5) / SCREEN_WIDTH) - 1);
			float py = 2 * ((y + 0.5) / SCREEN_HEIGHT) - 1;

      vec4 start = cameraPos;
			vec4 pixel = vec4(px,py,-focalLength,1);
			//vec4 pixel = vec4(x-(SCREEN_WIDTH/2), y-(SCREEN_HEIGHT/2), focalLength, 1.0);
			mat4 camToWorld = lookAt(vec3(cameraPos), to);
			pixel = camToWorld*pixel;
			vec4 dir = normalize(pixel - start);
			//vec4 dir = R * pixel;
      Intersection closestIntersection;
      if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
				vec3 color = DirectLight( closestIntersection );
        PutPixelSDL(screen, x, y, color);
      }
      else {
        vec3 black(0.0, 0.0, 0.0);
        PutPixelSDL(screen, x, y, black);
      }

    }
  }
}

/*Place updates of parameters here*/
bool Update()
{
//  static int t = SDL_GetTicks();
//  /* Compute frame time */
//  int t2 = SDL_GetTicks();
//  float dt = float(t2-t);
//  t = t2;

  //cout << "Render time: " << dt << " ms." << endl;
	vec4 forward( R[2][0], R[2][1], R[2][2], 1 );


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
          cameraPos += forward;
		      break;
	      case SDLK_DOWN:
		      /* Move camera backwards */
          cameraPos -= forward;
		      break;
	      case SDLK_LEFT:
		      /* Move camera left */
					yaw = 0.1;
					R = mat4(
						cos(yaw), 0, -sin(yaw), 0,
				    0       , 1, 0        , 0,
				  	sin(yaw), 0, cos(yaw) , 0,
				    0       , 0, 0        , 1
					);

					cameraPos = R * cameraPos;
		      break;
	      case SDLK_RIGHT:
		      /* Move camera right */
					yaw = -0.1;
					R = mat4(
						cos(yaw), 0, -sin(yaw), 0,
				    0       , 1, 0        , 0,
				  	sin(yaw), 0, cos(yaw) , 0,
				    0       , 0, 0        , 1
					);
					cameraPos = R * cameraPos;
		      break;
	      case SDLK_ESCAPE:
		      /* Move camera quit */
		      return false;
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
	vec3 normal = vec3(triangles[i.triangleIndex].normal);
	float radius = glm::length(vec3(lightPos) - vec3(i.position));
	vec3 r = normalize(vec3(lightPos) - vec3(i.position));
	float x = max( dot(r, normal), float(0.0) ) / ( 4 * M_PI * pow(radius, 2) );
	vec3 D =  x * lightColor;
	return D * triangles[i.triangleIndex].color;
	//float(2.0 - radius) * vec3(1.0,1.0,1.0)
}

mat4 lookAt( vec3 from, vec3 to) {
	vec3 tmp = vec3(0,1,0);
	vec3 forward = normalize(from - to);
	vec3 right = cross(normalize(tmp), forward);
	vec3 up = cross(forward, right);

	mat4 camToWorld;
	camToWorld[0][0] = right.x;
	camToWorld[0][1] = right.y;
	camToWorld[0][2] = right.z;
	camToWorld[1][0] = up.x;
	camToWorld[1][1] = up.y;
	camToWorld[1][2] = up.z;
	camToWorld[2][0] = forward.x;
	camToWorld[2][1] = forward.y;
	camToWorld[2][2] = forward.z;

	camToWorld[3][0] = from.x;
	camToWorld[3][1] = from.y;
	camToWorld[3][2] = from.z;

	camToWorld[0][3] = 0;
	camToWorld[1][3] = 0;
	camToWorld[2][3] = 0;
	camToWorld[3][3] = 1;

	return camToWorld;
}
