/*
 * VolumeLight.hpp
 *
 *  Created on: 17.09.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_VOLUMELIGHT_VOLUMELIGHT_HPP_
#define LOGIC_VOLUMELIGHT_VOLUMELIGHT_HPP_

#include <boost/shared_ptr.hpp>
#include <glm/glm.hpp>
#include <Graphics/Color.hpp>

using namespace std;
using namespace sgl;

class VolumeLight
{
    friend class LightManagerVolume;
public:
    VolumeLight(const glm::vec2 &pos, float rad, const Color &col) {
        position = pos;
        radius = rad;
        color = col;
    }

    inline void setPosition(const glm::vec2 &pos) { position = pos; }
    inline void setRadius(float rad) { radius = rad; }
    inline void setColor(const Color &col) { color = col; }
    inline glm::vec2 getPosition() { return position; }
    inline float getRadius() { return radius; }
    inline Color getColor() { return color; }

private:
    glm::vec2 position;
    float radius;
    Color color;
};

typedef boost::shared_ptr<VolumeLight> VolumeLightPtr;



#endif /* LOGIC_VOLUMELIGHT_VOLUMELIGHT_HPP_ */
