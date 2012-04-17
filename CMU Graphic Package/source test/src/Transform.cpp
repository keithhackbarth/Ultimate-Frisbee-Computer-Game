/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Transform.cpp,v 1.3 2002/10/23 18:50:28 jsaks Exp $
*/

#include <CarnegieMellonGraphics.h>
#include <Daemon.h>
#include <Debugging.h>

const double Transform::SCALE = 10000.0;

Transform::Transform() {
  setRotation(0);
  setScale(1,1);
}

Transform::Transform(double rotation) {
  setRotation(rotation);
  setScale(1,1);
}

Transform::Transform(double sx,double sy) {
  setRotation(0);
  setScale(sx,sy);
}

Transform::Transform(double rotation,double sx, double sy) {
  setRotation(rotation);
  setScale(sx,sy);
}

void Transform::setRotation(double rot) {
  theta = rot;
}

void Transform::setScale(double sx,double sy) {
  setScaleX(sx);
  setScaleY(sy);
}

void Transform::setScaleX(double sx) {
  scaleX = sx;
}

void Transform::setScaleY(double sy) {
  scaleY = sy;
}

double Transform::getRotation() const {
  return theta;
}

double Transform::getScaleX() const {
  return scaleX;
}

double Transform::getScaleY() const {
  return scaleY;
}

ostream& operator<<(ostream &s, const Transform &transform) {
  s << "Transform(" << transform.getRotation() << "," << transform.getScaleX()
    << "," << transform.getScaleY() << ")" << endl;
  return s;
}
