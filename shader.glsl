out vec3 outColor;

in vec3 iColor;
in float iFactor;

float valueIncr() {
	return 0.1f;
}

void main()
{
	float test = floor(vec3(2.5f, 1.5f, 1.4f)).x;

	float val = 0.0f;
	for (int i = 0; i < 5; i++) 
		val += valueIncr();
	
	outColor = vec3(val * test, iFactor, iColor.x);
}