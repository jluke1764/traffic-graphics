#ifndef TRAFFICSCENE_H
#define TRAFFICSCENE_H

#include "parser/sceneparser.h"
#include "shapes/car.h"
#include <vector>
class TrafficScene
{
public:
    TrafficScene();
    std::vector<RenderShapeData>& getShapes();
    void update(int time);

private:
    std::vector<Car *> m_cars;
    std::vector<RenderShapeData> m_shapes;
    glm::vec3 block2positionCoordinates(int row, int col);
};


#endif // TRAFFICSCENE_H
