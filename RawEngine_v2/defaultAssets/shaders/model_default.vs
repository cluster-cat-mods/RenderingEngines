#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBit;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform vec3 cameraPosition;

out vec3 fPos;
out vec3 fNor;
out vec2 uv;
out vec3 fTan;
out vec3 fBit;
out mat3 tbn;

void main() {
  fPos = (mMatrix * vec4(aPos, 1)).rgb;
  fTan = (mMatrix * vec4(aTan, 0)).rgb;
  fNor = (mMatrix * vec4(aNor, 0)).rgb;
  fBit = (mMatrix * vec4(aBit, 0)).rgb;
  uv = aUv;

  tbn = transpose(mat3(
      vec3(mMatrix * vec4(aTan, 0)),
      vec3(mMatrix * vec4(aBit, 0)),
      vec3(mMatrix * vec4(aNor, 0))
      ));

  gl_Position = mvpMatrix * vec4(aPos, 1.0);
}