#include "trafficscene.h"
#include "shapes/car.h"
#include <iostream>
#include <ostream>

TrafficScene::TrafficScene() {
    std::cout << "created traffic scene" << std::endl;
    m_cars.clear();

    Car* car = new Car();
    m_cars.push_back(car);

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

        car->setVelocity(0.02);

        car->goForward();

        if (time % 20 == 0) {
            car->turnRight();
            std::cout << "car turn" << std::endl;
        } else {
            car->goForward();
        }


    }
}
