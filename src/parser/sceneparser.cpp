#include "sceneparser.h"
#include "scenefilereader.h"
#include "scenedata.h"
#include <glm/gtx/transform.hpp>

// #include <chrono>
#include <iostream>

struct Word {
    std::string word;
    Word* left;
    Word* right;
};

void dfsPrintTree(Word* w, std::string sentence) {
    // Task 4: Debug this function!!! (Hint: you may need to write/add some logic...)
    // Missing base case
    // wrong order for adding word to sentence
    if (w==nullptr) {
        return;
    }
    std::string newSentence = sentence + w->word;
    if (w->left == nullptr && w->right == nullptr) {
        std::cout << newSentence << std::endl;
    }
    dfsPrintTree(w->left, newSentence);
    dfsPrintTree(w->right, newSentence);
}

Word* initTree(std::vector<Word> &words) {
    // STUDENTS - DO NOT TOUCH THIS FUNCTION
    words.reserve(8);
    words.push_back(Word{"2D graphics ", nullptr, nullptr});
    words.push_back(Word{"3D graphics ", nullptr, nullptr});
    words.push_back(Word{"making ", &words[0], &words[1]});
    words.push_back(Word{"CS123 ", nullptr, nullptr});
    words.push_back(Word{"love ", &words[3], &words[2]});
    words.push_back(Word{"bugs ", nullptr, nullptr});
    words.push_back(Word{"hate ", nullptr, &words[5]});
    words.push_back(Word{"I ", &words[4], &words[6]});
    return &words[7];
}

void SceneParser::debugDFS() {
    // Task 4: Uncomment this function
    std::vector<Word> words;
    Word* root = initTree(words);
    std::string sentence = "";
    dfsPrintTree(root, sentence);
}

void SceneParser::dfs(SceneNode* node, RenderData &renderData, glm::mat4 ctm, Car* car) {
    if (node==nullptr) {
        return;
    }

    std::cout << node->name << std::endl;
    if (node->name.find("truck") != std::string::npos and car == nullptr) {
        Car temp = Car();
        renderData.cars.push_back(temp);
        car = &renderData.cars.back();
        // car->setPosition(glm::vec3(renderData.cameraData.pos));
    }

    glm::mat4 T(1.f);
    glm::mat4 S(1.f);
    glm::mat4 R(1.f);
    glm::mat4 M(1.f);
    for (SceneTransformation* transformation : node->transformations) {
        if (transformation->type == TransformationType::TRANSFORMATION_SCALE) {
            // S = glm::scale(transformation->scale);
            // ctm = ctm*S;
            ctm = glm::scale(ctm, transformation->scale);
        } else if (transformation->type == TransformationType::TRANSFORMATION_ROTATE) {
            // R = glm::rotate(transformation->angle, transformation->rotate);
            // ctm = ctm*R;
            ctm = glm::rotate(ctm, transformation->angle, transformation->rotate);
        } else if (transformation->type == TransformationType::TRANSFORMATION_TRANSLATE) {
            // T = glm::translate(transformation->translate);
            // ctm = ctm*T;
            ctm = glm::translate(ctm, transformation->translate);
        } else if (transformation->type == TransformationType::TRANSFORMATION_MATRIX) {
            M = transformation->matrix;
            // ctm = ctm*M;
            ctm*=M;
        }
    }

    for (ScenePrimitive* primitive : node->primitives) {
        RenderShapeData shape_data;
        shape_data.primitive = *primitive;
        shape_data.ctm = ctm;
        shape_data.carPtr = car;
        renderData.shapes.push_back(shape_data);
        // std::cout << int(shape_data.primitive.type) << std::endl;
        if (node->name.find("body") != std::string::npos and car != nullptr) {
            car->addBodyShape(&renderData.shapes.back());
        } else if (node->name.find("wheel") != std::string::npos and car != nullptr) {
            car->addWheelShape(&renderData.shapes.back());
        }
    }

    for (SceneLight* light : node->lights) {
        SceneLightData light_data;
        light_data.id = light->id;
        light_data.type = light->type;
        light_data.color = light->color;
        light_data.function = light->function;
        light_data.penumbra = light->penumbra;
        light_data.angle = light->angle;
        light_data.width = light->width;
        light_data.height = light->height;

        light_data.pos = ctm*glm::vec4(0,0,0,1);
        light_data.dir = ctm*light->dir;

        renderData.lights.push_back(light_data);
    }

    for (SceneNode* child : node->children) {
        dfs(child, renderData, ctm, car);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    //get the root node
    SceneNode* root = fileReader.getRootNode();
    //clear renderData.shapes
    renderData.shapes.clear();
    renderData.lights.clear();
    renderData.cars.clear();


    //traverse the tree in a depth-first manner
    dfs(root, renderData, glm::mat4(1.0f), nullptr);
    // std::cout << renderData.shapes[0].ctm << std::endl;
    // std::cout << renderData.shapes.size() << std::endl;


    return true;
}

SceneNode* SceneParser::findNodeByName(SceneNode* root, const std::string &name) {
    if (!root) return nullptr;
    std::stack<SceneNode*> s;
    s.push(root);
    while(!s.empty()) {
        SceneNode* node = s.top(); s.pop();
        if (node->name == name) {
            return node;
        }
        for (auto *child : node->children) {
            s.push(child);
        }
    }
    return nullptr;
}
