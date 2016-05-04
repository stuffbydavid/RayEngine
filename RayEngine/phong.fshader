#version 420 core
#define MAXLIGHTS	16

in vec2 vTexCoord;
in vec3 vPosWorld;
in vec3 vNormalWorld;
out vec4 fColor;
uniform sampler2D uSampler;
uniform vec3 uEyePos;
uniform float uLights;
uniform vec3 uLightPos[MAXLIGHTS];
uniform vec4 uLightColor[MAXLIGHTS];
uniform float uLightRange[MAXLIGHTS];
uniform vec4 uAmbient;
uniform vec4 uSpecular;
uniform vec4 uDiffuse;
uniform float uShininess;

void main(void) {
	vec3 totalDiffuse = vec3(0.0), totalSpecular = vec3(0.0);
	vec4 texColor = uDiffuse * texture2D(uSampler, vTexCoord);

	if (texColor.a < 0.1)
		discard;
	
	// Go through the lights
	for (int i = 0; i < uLights; i++) {
		vec3 incidence = normalize(uLightPos[i] - vPosWorld);
			
		// Calculate attenuation (falloff)
		float distance = length(uLightPos[i] - vPosWorld);
		float attenuation = max(1.0 - distance / uLightRange[i], 0.0);
		
		// Diffuse factor
		float diffuseFactor = max(dot(vNormalWorld, incidence), 0.0) * attenuation;
		totalDiffuse += diffuseFactor * uLightColor[i].rgb;
		
		// Specular factor
		if (uShininess > 0.0) {
			vec3 reflection = -reflect(incidence, vNormalWorld);
			vec3 toEye = normalize(uEyePos - vPosWorld);
			float specularFactor = pow(max(dot(reflection, toEye), 0.0), uShininess) * attenuation;
			totalSpecular += specularFactor * uSpecular.rgb;
		}
	}

	// Create color
	fColor.rgb = texColor.rgb * (uAmbient.rgb + totalDiffuse) + totalSpecular;
	fColor.a = texColor.a; // Alpha stays
}