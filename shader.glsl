out vec4 outColor;
uniform sampler2D inTex;

void main()
{
	vec4 smp = texture(inTex, vec2(0.0f, 0.0f));
	float val = smp.x;
	
	for (int i = 0; i < 5; i++) 
		val *= 0.5f;
		
	outColor = vec4(val, smp.xyz);
}