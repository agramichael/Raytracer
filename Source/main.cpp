#include "raytracer.h"

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update();
void Draw(screen* screen);
bool ClosestIntersection( vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
mat4 lookAt();
vec3 DirectLight( const Intersection& i );
void update_R(float y);
vec3 Light( const Intersection& i );
void generateLightSample();

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

/*Place your drawing here*/
void Draw(screen* screen)
{
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  // cout << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << "\n";

#pragma omp parallel for schedule(static,10)
  for( int x = 0; x < SCREEN_WIDTH; x++ )
  {
    for( int y = 0; y < SCREEN_HEIGHT; y++ )
    {
			vec3 color;
			vec3 start = cameraPos;

			float px = -(x - SCREEN_WIDTH/2);
			float py = -(y - SCREEN_HEIGHT/2);
			vec4 pixel = vec4(px, py, -focalLength, 1);
			mat4 camToWorld = lookAt();
			vec4 p = camToWorld * pixel;
			vec3 dir = normalize( vec3(p) - start);
			vec3 focal_point = start + float(1) * dir;

			for( int i = -SSAA/2; i < SSAA/2; i++ ) {
			 	for( int j = -SSAA/2; j < SSAA/2; j++ ) {
					pixel.x += float(i)*SSAA_INV;
					pixel.y += float(j)*SSAA_INV;
					pixel = camToWorld * pixel;
					dir = normalize( vec3(pixel) - start);

					Intersection closestIntersection;
		    //   		if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
						// color += Light( closestIntersection ) * triangles[closestIntersection.triangleIndex].color;
		    //  		}
					vec3 partial_color = vec3(0, 0, 0);
					for (int k = 0; k < DOF_SAMPLES; k++) {
						vec3 rand = vec3(glm::linearRand(-1, 1), glm::linearRand(-1, 1), 0);
						rand *= APERTURE;
						vec3 pixl = start + dir;
						pixl += rand;

						dir = glm::normalize(focal_point - vec3(pixel));

						// if (x==0 && y==0) cout << "Starting point: " << pixel.x << ", " << pixel.y << ", " << pixel.z << "\n";

			      		if( ClosestIntersection( start, dir, triangles, closestIntersection ) ){
							partial_color += Light( closestIntersection ) * triangles[closestIntersection.triangleIndex].color;
			     		}
					}
					partial_color /= DOF_SAMPLES;
					color += partial_color;
			 	}
			}
			color /= SSAA*SSAA;
			PutPixelSDL(screen, x, y, color);
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

  //cout << "Render time: " << dt << " ms." << endl;o
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

void update_R(float yaw) {
	float s = sin(yaw);
  	R[0][0] = R[2][2] = cos(yaw);
  	R[0][2] = s;
  	R[2][0] = -s;
}

vec3 Light( const Intersection& i ) {
	return DirectLight( i ) + indirectLight;
}

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
