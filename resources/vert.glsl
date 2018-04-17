#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat3 T1;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space
attribute vec2 aTex;

varying vec3 posit; // Pass to fragment shader
varying vec3 norms;
varying vec2 vTex;

void main()
{
	gl_Position = P * MV * aPos;
	posit = (MV * aPos).xyz;
	norms = normalize((MV * vec4(aNor, 0.0)).xyz);
	vTex = (vec3(aTex, 1.0) * T1).xy;
}
