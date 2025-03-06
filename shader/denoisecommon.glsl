
float normalEdgeStoppingWeight(vec3 centerNormal, vec3 sampleNormal, float power) {
	return pow(clamp(dot(centerNormal, sampleNormal), 0.0, 1.0), power);
}

float depthEdgeStoppingWeight(float centerDepth, float sampleDepth, float phi) {
	return exp(-abs(centerDepth - sampleDepth) / phi);
}

vec4 edgeAvoidingBlur(int radius, float stepSize, vec2 center, float centerDepth, vec3 centerNormal, float ageWeight, sampler2D normalsSampler, sampler2D illuminationSampler) {
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

			float weightNormal = normalEdgeStoppingWeight(centerNormal.xyz, currNormal.xyz, 128.0);
			float weightDepth = depthEdgeStoppingWeight(centerDepth, currDepth, 0.00001);
			
			illumSum += texture(illuminationSampler, pos).rgb * weightNormal * weightDepth;
			weightSum += weightNormal * weightDepth;
		}
	}

	return vec4(illumSum, weightSum);
}

