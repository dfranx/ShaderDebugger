out vec3 outColor;

in float iFactor;
in vec3 iColor;

void main()
{
	float test = floor(vec3(2.5f, 1.5f, 1.4f)).x;

	float val = 0.0f;
	for (int i = 0; i < 5; i++) 
		val += 0.1f;

	outColor = vec3(val * test, iFactor, iColor.y);
}