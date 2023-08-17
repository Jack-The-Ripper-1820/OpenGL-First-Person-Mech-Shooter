#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

struct DirectionalLight {
	vec3 color;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;
uniform vec3 eyePosition; //camera position

void main()
{
	vec4 ambientColor = vec4(directionalLight.color, 1.f) * directionalLight.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.f);
	vec4 diffuseColor = vec4(directionalLight.color, 1.f) * directionalLight.diffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0, 0, 0, 0);

	// there has to be diffuse light there for specular to exist
	if (diffuseFactor > 0.f) {
		vec3 fragToEye = normalize(eyePosition - FragPos); // normalize for only the direction
		vec3 reflectedVertex = normalize(reflect(directionalLight.direction, normalize(Normal)));//reflect across the normal

		float specularFactor = dot(fragToEye, reflectedVertex);

		if (specularFactor > 0.f) {
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(directionalLight.color * material.specularIntensity * specularFactor, 1.f);
		}
	}

	color = texture(theTexture, TexCoord) * (ambientColor + diffuseColor + specularColor);

}