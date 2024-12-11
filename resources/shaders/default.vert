#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 objectSpacePosition;
layout(location = 1) in vec3 objectSpaceNormal;
layout(location = 2) in vec2 coordinate;

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec4 worldSpacePosition;
out vec3 worldSpaceNormal;
out vec2 UV;

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 model_matrix;
uniform mat3 it_model_matrix;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 model_view;
uniform mat4 model_proj;

void main() {
    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5
    worldSpacePosition = model_matrix*vec4(objectSpacePosition,1);
    worldSpaceNormal = normalize(it_model_matrix*objectSpaceNormal);
    // normal = vec4(normalize(mat3(inverse(transpose(model_matrix)))*objectSpaceNormal),0);

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = model_proj*model_view*worldSpacePosition;
    UV = coordinate;
}
