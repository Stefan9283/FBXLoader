#version 330 core
in vec3 toFragShaderColor;

void main()
{
	gl_FragColor = vec4(toFragShaderColor, 1.0f);
};
