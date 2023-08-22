#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

out vec4 color;

const int N_POINT_LIGHTS = 3;
const int N_SPOT_LIGHTS = 3;

struct Light {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight {
	PointLight base;
	vec3 direction;
	float edgeAngle;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[N_POINT_LIGHTS];
uniform SpotLight spotLights[N_SPOT_LIGHTS];

uniform Material material;
uniform vec3 eyePosition; //camera position

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;

float CalcDirectionalShadowFactor(DirectionalLight light) {
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5;

	float current = projCoords.z;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(directionalLight.direction);

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;

	if (projCoords.z > 1.0) {
		shadow = 0.0;
	}

	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor) {
	vec4 ambientColor = vec4(light.color, 1.f) * light.ambientIntensity;

	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.f);
	vec4 diffuseColor = vec4(light.color * light.diffuseIntensity * diffuseFactor, 1.f);

	vec4 specularColor = vec4(0.f, 0.f, 0.f, 0.f);

	if (diffuseFactor > 0.f) {
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));

		float specularFactor = dot(fragToEye, reflectedVertex);

		if (specularFactor > 0.f) {
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.f);
		}
	}

	return (ambientColor + (1.f - shadowFactor) * (diffuseColor + specularColor));
}

vec4 CalcDirectionalLight() {
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pointLight) {
	vec3 direction = FragPos - pointLight.position;
	float distance = length(direction);
	direction = normalize(direction);

	vec4 color = CalcLightByDirection(pointLight.base, direction, 0.f);
	float attenuation = pointLight.exponent * distance * distance +
		pointLight.linear * distance +
		pointLight.constant;

	return color / attenuation;
}

vec4 CalcSpotLight(SpotLight spotLight) {
	vec3 rayDirection = normalize(FragPos - spotLight.base.position);
	float spotLightFactor = dot(rayDirection, spotLight.direction);

	if (spotLightFactor > spotLight.edgeAngle) {
		vec4 color = CalcPointLight(spotLight.base);

		return color * (1.0f - (1.0f - spotLightFactor) * (1.0f / (1.0f - spotLight.edgeAngle)));
	}

	else {
		return vec4(0.f, 0.f, 0.f, 0.f);
	}
}

vec4 CalcPointLights() {
	vec4 totalColor = vec4(0, 0, 0, 0);
	for (int i = 0; i < pointLightCount; i++)
	{
		totalColor += CalcPointLight(pointLights[i]);
	}

	return totalColor;
}

vec4 CalcSpotLights() {
	vec4 totalColor = vec4(0.f, 0.f, 0.f, 0.f);
	for (int i = 0; i < spotLightCount; i++) {
		totalColor += CalcSpotLight(spotLights[i]);
	}

	return totalColor;
}

void main()
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();

	color = texture(theTexture, TexCoord) * finalColor;

}