#include "trafficscene.h"
#include "shapes/car.h"
#include <iostream>
#include <ostream>

TrafficScene::TrafficScene() {
    std::cout << "created traffic scene" << std::endl;
    m_cars.clear();

    SceneColor red = {1, 0, 0, 1};

    // glm::vec3 loc1 = glm::vec3(0, 0, 0);

    glm::vec3 loc1 = block2positionCoordinates(0, 0);
    Car* car1 = new Car(red, loc1, 90);
    m_cars.push_back(car1);
    std::cout << "car1" << std::endl;


    glm::vec3 loc2 = glm::vec3(3, 0, 3);
    SceneColor blue = {0, 0, 1, 1};
    Car* car2 = new Car(blue, loc2, 90);
    m_cars.push_back(car2);

    std::cout << "car2" << std::endl;



    Car* car3 = new Car(blue, block2positionCoordinates(1, 1), 90);
    m_cars.push_back(car3);

    std::cout << "car3" << std::endl;



    Car* car4 = new Car(red, block2positionCoordinates(2, 1), 90);
    m_cars.push_back(car4);

    std::cout << "car4" << std::endl;



    Car* car5 = new Car(red, block2positionCoordinates(3, 1), 90);
    m_cars.push_back(car5);

    std::cout << "car5" << std::endl;



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
            car->setDesiredPosition(car->getPosition() + glm::vec3(7, 0, 7));
        }

        car->update(time);


    }
}

glm::vec3 TrafficScene::block2positionCoordinates(int row, int col) {
    //11 rows, 11 cols
    float xpos = row+0.925;
    float zpos = col+0.025;

    return glm::vec3(xpos, 0, zpos);
}
