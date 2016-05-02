#version 420 core
#define MAXLIGHTS	16

in vec2 vTexCoord;
in vec3 vPosWorld;
in vec3 vNormalWorld;
out vec4 fColor;
uniform sampler2D uSampler;
uniform vec4 uambient;
uniform vec3 uEyePos;
uniform float uLights;
uniform vec3 uLightPos[MAXLIGHTS];
uniform vec4 uLightColor[MAXLIGHTS];
uniform float uLightRange[MAXLIGHTS];
uniform float uShininess;
uniform vec4 udiffuse;

void main(void) {
	vec3 totalDiffuse = vec3(0.0), totalSpecular = vec3(0.0);
	
	// Go through the lights
	for (int i = 0; i < uLights; i++) {
		vec3 incidence = normalize(uLightPos[i] - vPosWorld);
			
		// Calculate attenuation (falloff)
		float distance = length(uLightPos[i] - vPosWorld);
		float attenuation = max(1.0 - distance / uLightRange[i], 0.0);
		
		// Diffuse factor
		float diffuse = max(dot(vNormalWorld, incidence), 0.0) * attenuation;
		totalDiffuse += diffuse * uLightColor[i].rgb;
		
		// Specular factor
		if (uShininess > 0.0) {
			vec3 reflection = 2 * dot(incidence, vNormalWorld) * vNormalWorld - incidence;
			vec3 toEye = normalize(uEyePos - vPosWorld);
			float specular = pow(max(dot(reflection, toEye), 0.0), 1.0 / uShininess) * attenuation;
			totalSpecular += specular * uLightColor[i].rgb;
		}
	}

	// Create color
	vec4 texColor = udiffuse * texture2D(uSampler, vTexCoord);
	fColor.rgb = texColor.rgb * (uambient.rgb + totalDiffuse) + totalSpecular;
	fColor.a = texColor.a; // Alpha stays
}