
struct SceneUniform {
	mat4 view;
	mat4 viewInv;
	mat4 projection;
	mat4 projectionInv;
	mat4 prevView;
	mat4 prevViewInv;
	mat4 prevProjection;
	mat4 prevProjectionInv;
	float time;
	float near;
	float far;
	vec3 ambientColor;
	int giSamples;
	float giBrightness;
	bool denoise;
	bool shadows;
	vec3 portalTint;
	int portalJumps;
	bool portalGI;
	vec3 skyColor;
	float skyEmission;
	int maxSampleAge;
};
