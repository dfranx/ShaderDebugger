out vec3 outColor;
uniform sampler2D inTex;

float incr(vec3 b) { return 1.0f; }
float incr(vec2 b) { return 2.0f; }

void main()
{
	mat2 mat_test1 = mat2(1.0f);
	mat2 mat_test2 = mat2(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f));
	mat2 mat_test3 = 2.0f * mat2(1.0f, 2.0f, 3.0f, 4.0f) * 2.0f;
	mat3 mat_test4 = mat3(mat_test3) + mat3(1.0f);

	vec2 mat_mul_test1 = mat_test2 * vec2(10, 20);
	vec2 mat_mul_test2 = vec2(10, 20) * mat_test2;
	
	vec3 mat_get1 = mat_test4[0];
	float mat_get2 = mat_test4[0][0];

	mat_test4[0][0] = 6.0f;
	mat_test4[1] = vec3(1.0f, 1.0f, 1.0f);
	
	vec4 stest = vec4(3.0f);
	stest.xz = 1.0f;
	stest.xz++;

	vec3 divide_test = vec3(2)+ivec3(3)/ivec3(2);
	divide_test.xyz++;

	uvec3 mult_test = uvec3(-1);
	float scalar_test = 2.0f+(2.*(3./2.));
	ivec3 add_test = 1.0f-(vec3(1.0f, 2.0f, 3.0f) + ivec3(2) + bvec3(true, false, false));

	bvec2 v2_test = bvec2(true, false);

	vec4 smp = texture(inTex, vec2(0.0f));
	smp[0] = 1.923f;
	float arr_test = smp[0];

	float test = floor(vec3(2.5f, 1.5f, 1.4f)).x;
	vec3 a = vec3(.5) + vec3(0.6f);

	a.r = 2.4f;
	a.r++;
	float hey = a.r;
	float there = floor(hey*.8)*.5;

	vec3 swzl = vec3(3.0f, 2.0f, 1.0f);
	swzl.zyx = vec3(3.0f, 2.0f, 1.0f);
	vec3 field = swzl.zyx;

	vec3 another = swzl.zzz + 0.5f;

	vec3 t = vec3(0.0f) + 1.0f;
	vec3 t2 = 1.1f + vec3(0.0f);

	if (t > false)
		t.z++;

	if (t == 1.0f)
		t.y++;

	if (t == t2)
		t.x++;

	if (t2 >= 1.1f)
		t2.x++;

	if (t2 != vec3(2.1f, 1.1f, 1.1f))
		discard;

	float val = true;
	for (int i = 0; i < 5; i++) 
		val += 0.1f;
		
	outColor = vec3(val * test, incr(mat_get1.xz), incr(mat_get1));
}