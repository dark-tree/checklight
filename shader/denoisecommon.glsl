
float normalEdgeStoppingWeight(vec3 centerNormal, vec3 sampleNormal, float power) {
	return pow(clamp(dot(centerNormal, sampleNormal), 0.0, 1.0), power);
}

float depthEdgeStoppingWeight(float centerDepth, float sampleDepth, float phi) {
	return exp(-abs(centerDepth - sampleDepth) / phi);
}

float planeDistanceWeight(vec3 centerPos, vec3 samplePos, vec3 normal) {
	return 1 - min(abs(dot(centerPos - samplePos, normal)) * 2, 1);
}

vec4 edgeAvoidingBlur(int radius, float stepSize, vec2 center, float centerDepth, vec3 centerNormal, float ageWeight,
	sampler2D normalsSampler, sampler2D illuminationSampler, sampler2D positionSampler, float age, vec3 centerWS) {

	vec3 illumSum = vec3(0.0);
	float weightSum = 0.0;

	float illumTexelSize = 1.0 / float(textureSize(illuminationSampler, 0).x);

	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			if (x == 0 && y == 0) {
				continue;
			}

			vec2 pos = center + vec2(x, y) * illumTexelSize * stepSize * ageWeight;

			if (pos.x < 0.0 || pos.x > 1.0 || pos.y < 0.0 || pos.y > 1.0) {
				continue;
			}

			vec4 currNormal = texture(normalsSampler, pos);
			float currDepth = currNormal.w;

			float weightNormal = min(normalEdgeStoppingWeight(centerNormal.xyz, currNormal.xyz, 128.0) + age, 1.0);

			vec3 currPos = texture(positionSampler, pos).xyz;
			float weightPlane = planeDistanceWeight(centerWS, currPos, centerNormal.xyz);
			
			illumSum += texture(illuminationSampler, pos).rgb * weightNormal * weightPlane;
			weightSum += weightNormal * weightPlane;
		}
	}

	return vec4(illumSum, weightSum);
}

