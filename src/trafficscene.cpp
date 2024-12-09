#include "trafficscene.h"
#include "shapes/car.h"
#include <iostream>
#include <ostream>

TrafficScene::TrafficScene() {
    std::cout << "created traffic scene" << std::endl;
    m_cars.clear();

    SceneColor red = {1, 0, 0, 1};

    glm::vec3 loc1 = glm::vec3(-7, 0, 7);

    Car* car1 = new Car(red, loc1, 0.0);
    m_cars.push_back(car1);

    // glm::vec3 loc2 = glm::vec3(-7, 0, -7);
    // SceneColor blue = {0, 0, 1, 1};
    // Car* car2 = new Car(blue, loc2, 90);
    // m_cars.push_back(car2);


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

        int clk = time % 1200;

        if (clk < 800) {
            car->drive(0, 1);
        } else if (clk < 1000) {
            car->drive(15, 1);
        } else  {
            car->drive(-15, 1);
        }


    }
}
