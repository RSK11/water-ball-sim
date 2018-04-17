#version 120

uniform vec3 lightPos;
uniform mat4 VM;
uniform sampler2D texture;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;
uniform float alpha;

varying vec3 posit; // passed from the vertex shader
varying vec3 norms;
varying vec2 vTex;

void main()
{
	vec3 n = normalize(norms);
	vec3 l = normalize((VM * vec4(lightPos, 1.0)).xyz - posit);
	vec3 cd = kd * max(0, dot(l,n));

	if (s >= 0)
	{
		vec3 e = normalize(-posit);
		vec3 h = normalize(l + e);
		vec3 cs = ks * pow(max(0, dot(h,n)), s);
		vec3 color = ka + cd + cs;
		gl_FragColor = vec4(color, alpha);
	}
	else
	{
		vec4 color = texture2D(texture, vTex) + vec4(cd, alpha);
		gl_FragColor = color;
	}
}
