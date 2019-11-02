# ShaderDebugger

ShaderDebugger is a library built for [SHADERed](https://github.com/dfranx/SHADERed).
It can be used to run shaders on CPU or debug them (get variable values, run them line by line, etc...).

## How?
ShaderDebugger uses [glsl-parser](https://github.com/dfranx/glsl-parser) to parse 
GLSL into AST. The AST is converted to the bytecode with the [aGen](https://github.com/dfranx/aGen) library. The bytecode can then be executed using the [BlueVM](https://github.com/dfranx/BlueVM).

## How to compile

## Example
```c++
bv_library* libGLSL = sd::Library::GLSL();
sd::ShaderDebugger vs;
vs.SetSource(sd::GLSLTranslator(), sd::ShaderType::Vertex, vertexCode, "main", libGLSL);

vs.SetValue("matVP", glm::mat4(1));

for (int i = 0; i < 3; i++) {
    vs.SetValue("position", vertices[i]);
    vs.SetValue("color", glm::vec3(i\*0.1f, i\*0.2f, i\*0.3f, 1.0f));
    vs.Execute("main"); // execute whole vertex shader

    glm::vec3 oPos = vs.GetValue<glm::vec3>("oPos");
    ...
}
```

Or to execute line by line:
```c++
while (!vs.IsFinished()) {
    vs.Step();

    // get various info now using these functions: GetCurrentFunction, GetFunctionStack, GetVariableValue, etc...
}
```

## Limitations
List of things that currently don't work but I plan to fix them:
### GLSL
- doesn't support macros, interface blocks, local & structure member arrays & global variable initalization (unless it has const keyword)
- parsed, but not stored & not implemented: storage, auxilary, precision, memory, invariant
- parsed & stored but not implemented: smooth, flat, noperspective

### HLSL
- not implemented yet

### Both
- function overloading (BlueVM)