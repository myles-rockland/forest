#include "Light.h"

vec3 Light::GetPosition() const { return position; }
vec3 Light::GetAmbient() const { return ambient; }
vec3 Light::GetDiffuse() const { return diffuse; }
vec3 Light::GetSpecular() const { return specular; }