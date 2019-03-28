#include "globals.h"

const int SSAA = 4;
vec4 lightPos = vec4( 0, -0.5, -0.7, 1.0 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.5f * vec3( 1, 1, 1 );
vec4 cameraPos = vec4( 0.0, 0.0, -3.0, 1.0 );
float focalLength = SCREEN_HEIGHT;
const float yaw = 5 * M_PI / 180;
const float shadow_bias = 0.01;


#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FULLSCREEN_MODE false