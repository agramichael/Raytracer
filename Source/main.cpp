#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>
#include <glm/gtx/transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include "raytracer.h"
#include "globals.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

extern vec4 cameraPos;
extern vector<Triangle> triangles;
extern mat4 R;
extern float SSAA_INV;
extern const float yaw;
extern float occlusion_distance;
extern float occlusion_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update();
void Draw(screen* screen);

int main( int argc, char* argv[] )
{
  SSAA_INV = float(1)/SSAA;
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
			float px = -(x - SCREEN_WIDTH/2);
			float py = y - SCREEN_HEIGHT/2;
			for( int i = -SSAA/2; i < SSAA/2; i++ ) {
			 	for( int j = -SSAA/2; j < SSAA/2; j++ ) {
					vec4 pixel = vec4(px + float(i)*SSAA_INV, py + float(j)*SSAA_INV, -focalLength, 1);
					pixel = lookAt()*pixel;
					vec4 dir = normalize(pixel - start);

					Intersection closestIntersection;
		      		if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
						color += Light( closestIntersection ) * triangles[closestIntersection.triangleIndex].color;
						occlusion_buffer[x][y] = occlusion_distance;
		     		}
			 	}
			}
			color /= SSAA*SSAA;
			PutPixelSDL(screen, x, y, color);
    }
  }
  for (int x = 0; x < SCREEN_WIDTH; x++)
	{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        cout << x << " "  << y << " " << occlusion_buffer[x][y] << "\n";
    }
    cout << endl;
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