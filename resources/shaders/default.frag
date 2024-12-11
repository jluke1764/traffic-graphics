#version 330 core

const int directional = 0;
const int point = 1;
const int spot = 2;

struct Light {
    int type;
    vec4 direction;
    vec4 color;
    vec4 position;
    vec3 function;
    float angle;
    float penumbra;
};

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec4 worldSpacePosition;
in vec3 worldSpaceNormal;
in vec2 UV;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float k_a;
uniform float k_d;
uniform float k_s;

uniform vec4 O_a;
uniform vec4 O_d;
uniform vec4 O_s;


uniform float shininess;
uniform vec4 cameraPosition;

uniform int num_lights;
uniform Light[10] lights;

uniform bool has_texture;
uniform float blend;
uniform sampler2D tex;


void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    // fragColor = vec4(1.0);

    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
    // fragColor = abs(worldSpaceNormal);
    fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    vec3 directionToCamera = vec3(cameraPosition) - vec3(worldSpacePosition);
    directionToCamera = normalize(directionToCamera);

    //make sure normal points toward viewer
    vec3 normal = normalize(worldSpaceNormal);
    // if (dot(normal,vec4(directionToCamera,0))<0) {
    //     vec4 normal = -worldSpaceNormal;
    // }

    //add ambient term
    fragColor[0] += k_a*O_a[0];
    fragColor[1] += k_a*O_a[1];
    fragColor[2] += k_a*O_a[2];

    // loop through the lights
    for(int i=0; i<num_lights; i++) {
        Light light = lights[i];
        vec3 directionToLight;
        float f_att = 1.f;
        float intensity = 1;
        float visible = 1.f;
        float epsilon = 1e-3;
        vec3 function;

        if(light.type == directional) {
            directionToLight = normalize(vec3(-light.direction));
        } else if (light.type == point) {
            function = light.function;
            float distance = distance(light.position,worldSpacePosition);
            f_att = min(1.f,1/(function[0] + distance*function[1] + pow(distance, 2)*function[2]));
            directionToLight = normalize(vec3(light.position-worldSpacePosition));
        } else if (light.type == spot) {
            // calculate the intensity
            directionToLight = normalize(vec3(light.position-worldSpacePosition));
            float x = acos(dot(normalize(vec3(-light.direction)), directionToLight));
            float theta_inner = light.angle-light.penumbra;
            float theta_outer = light.angle;

            float falloff = -2*pow((x-theta_inner)/(theta_outer-theta_inner),3) + 3*pow((x-theta_inner)/(theta_outer-theta_inner),2);
            if (x<=theta_inner) {
                intensity = 1.f;
            } else if (x>theta_inner && x<=theta_outer){
                intensity = max(1-falloff,0.f);
            } else {
                intensity = 0.f;
            }

            //calculate the attenuation
            vec3 function = light.function;
            float distance = distance(light.position,worldSpacePosition);
            f_att = min(1.f,1/(function[0] + distance*function[1] + pow(distance, 2)*function[2]));
            // fragColor = vec4(1.f);
            // return;

        }
        float diffuse_dot_product = dot(normal, directionToLight);

        if (diffuse_dot_product<0) {
            diffuse_dot_product = 0.f;
        }

        //texture mapping
        if (has_texture) {
            vec4 texture = texture(tex, UV);
            fragColor += intensity*f_att*light.color*(blend*texture+(1-blend)*O_d*k_d)*diffuse_dot_product;
        } else {
            fragColor += intensity*f_att*light.color*k_d*O_d*diffuse_dot_product;
        }

        fragColor[0] += intensity*f_att*light.color[0]*k_d*O_d[0]*diffuse_dot_product;
        fragColor[1] += intensity*f_att*light.color[1]*k_d*O_d[1]*diffuse_dot_product;
        fragColor[2] += intensity*f_att*light.color[2]*k_d*O_d[2]*diffuse_dot_product;

        //add specular term
        vec3 R = reflect(-directionToLight, normal);
        R = normalize(R);


        float specular_dot_product = dot(R, directionToCamera);

        if (specular_dot_product>0) {
            fragColor[0] += intensity*f_att*light.color[0]*k_s*O_s[0]*pow(specular_dot_product,shininess);
            fragColor[1] += intensity*f_att*light.color[1]*k_s*O_s[1]*pow(specular_dot_product,shininess);
            fragColor[2] += intensity*f_att*light.color[2]*k_s*O_s[2]*pow(specular_dot_product,shininess);
        }

    }
    // if (has_texture) {
    //     fragColor = vec4(UV[0], UV[1], 0, 1);
    // } else {
    //     fragColor = vec4(1.f);
    // }
    // if (has_texture) {
    //     fragColor = vec4(texture(tex,UV));
    // } else {
    //     fragColor = vec4(1.f);
    // }
    fragColor = texture(tex, UV);
}
