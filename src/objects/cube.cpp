#ifdef WIN32_VC90
#pragma warning (disable : 4251)
#endif

#include "cube.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>

#include <QDebug>

using namespace std;

#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>

Cube::Cube(const btVector3 &dim, btScalar mass) : Object() {
    init(dim.getX(), dim.getY(), dim.getZ(), mass);
}

Cube::Cube(btScalar width, btScalar height, btScalar depth, btScalar mass) : Object() {
    init(width, height, depth, mass);
}

void Cube::init(btScalar width, btScalar height, btScalar depth, btScalar mass) {
    lengths[0] = width;
    lengths[1] = height;
    lengths[2] = depth;

    shape = new btBoxShape(btVector3(width * .5, height * .5, depth * .5));

    btQuaternion qtn;
    btTransform trans;
    btDefaultMotionState *motionState;

    trans.setIdentity();
    qtn.setEuler(0.0, 0.0, 0.0);
    trans.setRotation(qtn);
    trans.setOrigin(btVector3(0, 0, 0));
    motionState = new btDefaultMotionState(trans);

    btVector3 inertia;
    shape->calculateLocalInertia(mass,inertia);
    btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, motionState, shape, inertia);

    body = new btRigidBody(bodyCI);
}

void Cube::luaBind(lua_State *s) {
    using namespace luabind;

    open(s);

    module(s)
            [
            class_<Cube,Object>("Cube")
            .def(constructor<>(), adopt(result))
            .def(constructor<btScalar, btScalar, btScalar, btScalar>(), adopt(result))
            .def(constructor<const btVector3&>(), adopt(result))
            .def(constructor<const btVector3&, btScalar>(), adopt(result))
            .def(tostring(const_self))
            ];
}

QString Cube::toString() const {
    return QString("Cube");
}

void Cube::renderInLocalFrame(QTextStream *s) {
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f, 1.0 };
    GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // GLfloat no_shininess[] = { 0.0 };
    // GLfloat low_shininess[] = { 5.0 };
    GLfloat high_shininess[] = { 100.0 };
    // GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};

    glScalef(lengths[0], lengths[1], lengths[2]);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glColor3ubv(color);

    glutSolidCube(1.0f);

    if (s != NULL) {
        if (mPreSDL == NULL) {
            *s << "box { <"
               << -lengths[0]/2.0 << ", "<< -lengths[1]/2.0 << ", " << -lengths[2]/2.0 << ">, <"
               <<  lengths[0]/2.0 << ", " << lengths[1]/2.0 << ", " <<  lengths[2]/2.0 << ">" << endl;
            if (mTexture == NULL) {
                *s << "  pigment { rgb <"
                   << color[0]/255.0 << ", " << color[1]/255.0 << ", " << color[2]/255.0 << "> }" << endl;
            } else {
                *s << mTexture << endl;
            }
        } else {
            *s << mPreSDL << endl;
        }

        *s << "  matrix <" << matrix[0] << ","  << matrix[1] << ","  << matrix[2] << "," << endl
           << "          " << matrix[4] << ","  << matrix[5] << ","  << matrix[6] << ","  << endl
           << "          " << matrix[8] << ","  << matrix[9] << ","  << matrix[10] << "," << endl
           << "          " << matrix[12] << "," << matrix[13] << "," << matrix[14] << ">" << endl;

        if (mPostSDL == NULL) {
            *s << "}" << endl << endl;
        } else {
            *s << mPostSDL << endl;
        }
    }
    // qDebug() << "Cube::renderInLocalFrame() end";
}
