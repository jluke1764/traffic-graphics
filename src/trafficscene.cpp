#include "trafficscene.h"
#include "shapes/car.h"
#include <iostream>
#include <ostream>

TrafficScene::TrafficScene() {
    std::cout << "created traffic scene" << std::endl;
    m_cars.clear();

    int num_cars = 50;

    for (int i = 0; i < num_cars; i++) {

        SceneColor color = {rand()%100*0.1, rand()%90*0.1, rand()%90*0.1, 1};


        glm::vec3 loc = block2positionCoordinates(rand()%10-5, rand()%10-5);

        if (rand()%2) {
            Car* car1 = new Car(color, loc, 0);
            m_cars.push_back(car1);

        } else {
            Car* car1 = new Car(color, loc, 90);
            m_cars.push_back(car1);

        }
        std::cout << "car " << i << std::endl;
    }

    SceneColor red = {1, 0, 0, 1};

    // // glm::vec3 loc1 = glm::vec3(4.5, 0, 3);

    glm::vec3 loc1 = block2positionCoordinates(0, 0);
    Car* car1 = new Car(red, loc1, 0);
    m_cars.push_back(car1);
    std::cout << "car1" << std::endl;


    // glm::vec3 loc2 = glm::vec3(3, 0, 3.5);
    // SceneColor blue = {0, 0, 1, 1};
    // Car* car2 = new Car(blue, block2positionCoordinates(1, 1), 90);
    // m_cars.push_back(car2);

    // std::cout << "car2" << std::endl;


    // glm::vec3 loc3 = glm::vec3(3, 0, 3.5);

    // Car* car3 = new Car({0, 1.0, 0, 1}, loc3, 90);
    // m_cars.push_back(car3);

    // std::cout << "car3" << std::endl;



    // Car* car4 = new Car(red, {), 90);
    // m_cars.push_back(car4);

    // std::cout << "car4" << std::endl;



    // Car* car5 = new Car(red, block2positionCoordinates(3, 1), 90);
    // m_cars.push_back(car5);

    // std::cout << "car5" << std::endl;



}

std::vector<RenderShapeData>& TrafficScene::getShapes() {
    m_shapes.clear();
    for (Car* car : m_cars) {
        for (RenderShapeData& shape : car->getShapeData()) {
            m_shapes.push_back(shape);
        }
    }

    return m_shapes;

}

void TrafficScene::update(int time) {
    std::cout << "new frame: " << time << std::endl;
    for (Car* car : m_cars) {

        if (time == 1) {
            car->goOneBlock();
        }

        car->goOneBlock();

        if (time%3) {
            car->rightCorner();
        }

        car->update(time);


    }
}

glm::vec3 TrafficScene::block2positionCoordinates(int row, int col) {
    //11 rows, 11 cols
    float xpos = row+0.925+0.5+0.03;
    float zpos = col+0.025+0.5;

    return glm::vec3(xpos, 0, zpos);
}
