#include "raytracer.h"
#include "TestModelH.h"
#include "SDLauxiliary.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

vec3 cameraPos = vec3( 0.0, 0.0, -3.0 );
vector<Triangle> triangles;
mat3 R;
float SSAA_INV;
const float yaw (5 * M_PI / 180);
vec3 lightPos = vec3( 0, -0.5, -0.7 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.5f * vec3( 1, 1, 1 );
vec3 lightSample[SOFT_SHADOW_SAMPLES];

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update();
void Draw(screen* screen);

int main()
{
  SSAA_INV = float(1)/SSAA;
  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  LoadTestModel( triangles );
	R = mat3(
		cos(0) , 0, sin(0),
		0      , 1, 0     ,
		-sin(0), 0, cos(0)
	);

	generateLightSample();

  while( Update() )
    {
      Draw(screen);
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

// draw one frame
void Draw(screen* screen)
{
  // clear screen buffer
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

// shoot ssaa * ssaa rays for each pixel and average the color values
#pragma omp parallel for schedule(static, 10)
  for( int x = 0; x < SCREEN_WIDTH; x++ )
  {
    for( int y = 0; y < SCREEN_HEIGHT; y++ )
    {
			vec3 color;
			vec3 start = cameraPos;
			float px = -(x - SCREEN_WIDTH/2);
			float py = y - SCREEN_HEIGHT/2;
			for( int i = -SSAA/2; i < SSAA/2; i++ ) {
			 	for( int j = -SSAA/2; j < SSAA/2; j++ ) {
					vec4 pixel = vec4(px + float(i)*SSAA_INV, py + float(j)*SSAA_INV, -focalLength, 1);
					mat4 camToWorld = lookAt();
					pixel = camToWorld * pixel;
					vec3 dir = normalize( vec3(pixel) - start);

					Intersection closestIntersection;
		      		if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
						color += Light( closestIntersection ) * triangles[closestIntersection.triangleIndex].color;
		     		}

		     		// depth of field attempt

		   //   		vec3 partial_color = vec3(0, 0, 0);
					// for (int k = 0; k < DOF_SAMPLES; k++) {
					// 	vec3 rand = vec3(glm::linearRand(-1, 1), glm::linearRand(-1, 1), 0);
					// 	rand *= APERTURE;
					// 	vec3 pixl = start + dir;
					// 	pixl += rand;

					// 	dir = glm::normalize(focal_point - vec3(pixel));

			  //     		if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
					// 		partial_color += Light( closestIntersection ) * triangles[closestIntersection.triangleIndex].color;
			  //    		}
					// }
					// partial_color /= DOF_SAMPLES;
			 	}
			}
			color /= SSAA*SSAA;
			PutPixelSDL(screen, x, y, color);
    }
  }
}

// update camera and light positions as well as the light samples array
bool Update()
{
	vec3 right(R[0][0], R[0][1], R[0][2]);
	vec3 down(R[1][0], R[1][1], R[1][2]);
	vec3 forward(R[2][0], R[2][1], R[2][2]);

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
				update_R(-yaw);
				cameraPos = R * cameraPos;			
			  	break;
			case SDLK_RIGHT:
			  	/* Move camera right */
				update_R(yaw);
				cameraPos = R * cameraPos;
				break;
			case SDLK_ESCAPE:
				// quit
			  	return false;
			case SDLK_w:
				// move light forwards
				lightPos[2] += 0.1f;
				generateLightSample();
				break;
			case SDLK_s:
				// move light backwards
				lightPos[2] -= 0.1f;
				generateLightSample();
				break;
			case SDLK_a:
				// move light left
				lightPos[0] -= 0.1f;
				generateLightSample();
				break;
			case SDLK_d:
				// move light right
				lightPos[0] += 0.1f;
				generateLightSample();
				break;
			case SDLK_q:
				// move light up
				lightPos[1] -= 0.1f;
				generateLightSample();
				break;
			case SDLK_e:
				// move light down
				lightPos[1] += 0.1f;
				generateLightSample();
				break;
	      }
	  }
    }
return true;
}