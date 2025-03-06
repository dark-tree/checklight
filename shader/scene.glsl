
struct SceneUniform {
	mat4 view;
	mat4 projection;
	mat4 prevView;
	mat4 prevProjection;
	float time;
	float near;
	float far;
	vec3 dirLightDirection;
	vec3 dirLightColor;
	vec3 ambientColor;
};
