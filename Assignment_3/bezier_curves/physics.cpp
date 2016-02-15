/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ physics.cpp
 * Description ..... Physics Engine support things
 * Date ............ 22.07.2009
 * Created by ...... Paul Melis
 */

#ifdef OS_X
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include "physics.h"
#include "plymodel.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

btDefaultCollisionConfiguration     *collisionConfiguration;
btCollisionDispatcher               *dispatcher;
btAxisSweep3                        *overlappingPairCache;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld             *dynamicsWorld;

btHingeConstraint   *rotateConstraint, *hingeConstraint1, *hingeConstraint2;
btHingeConstraint   *grabberConstraint1, *grabberConstraint2;

static GLint    teapot_dlist;
static float    *teapot_vertices = NULL;
static int      *teapot_triangles = NULL;

typedef struct
{
    btCollisionShape *shape;
    btRigidBody *body;
    int bit;
    int mask;
}
object_info;

enum Objects
{
    FLOOR=0,
    ARM_SEGMENT_1,
    ARM_SEGMENT_2,
    ARM_SEGMENT_3,
    GRABBER_1,
    GRABBER_2,
    BLOCK_1,
    BLOCK_2,
    TEAPOT
};

const int num_objects = 9;

#define BIT(i)      (1<<(i))
#define OBJBIT(o)   (objects[o].bit)
#define ARMBITS     (OBJBIT(ARM_SEGMENT_1)|OBJBIT(ARM_SEGMENT_2)|OBJBIT(ARM_SEGMENT_3))
#define GRABBERBITS (OBJBIT(GRABBER_1)|OBJBIT(GRABBER_2))

static object_info
objects[num_objects] =
{
    // Floor
    { NULL, NULL, BIT(1), OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT)|OBJBIT(GRABBER_1)|OBJBIT(GRABBER_2) },
    // Arm segments
    { NULL, NULL, BIT(2), OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT) },
    { NULL, NULL, BIT(3), OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT) },
    { NULL, NULL, BIT(4), OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT) },
    // Grabbers
    { NULL, NULL, BIT(5), OBJBIT(FLOOR)|OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT) },
    { NULL, NULL, BIT(6), OBJBIT(FLOOR)|OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT) },
    // Blocks and teapot
    { NULL, NULL, BIT(7), ARMBITS|GRABBERBITS|OBJBIT(FLOOR)|OBJBIT(BLOCK_2)|OBJBIT(TEAPOT) },
    { NULL, NULL, BIT(8), ARMBITS|GRABBERBITS|OBJBIT(FLOOR)|OBJBIT(BLOCK_1)|OBJBIT(TEAPOT) },
    { NULL, NULL, BIT(9), ARMBITS|GRABBERBITS|OBJBIT(FLOOR)|OBJBIT(BLOCK_1)|OBJBIT(BLOCK_2) },
};

extern "C" void
setup_physics(void)
{
    btScalar    mass;
    btVector3   localInertia;

    btCollisionShape    *shape;
    btRigidBody         *body;
    btTransform         xform;

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    btVector3 worldAabbMin(-10000, -10000, -10000);
    btVector3 worldAabbMax(10000, 10000, 10000);
    overlappingPairCache = new btAxisSweep3(worldAabbMin, worldAabbMax, 1024);

    solver = new btSequentialImpulseConstraintSolver;
    solver->setRandSeed(123456);

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
        overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Floor

    shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0.0);

    xform.setIdentity();

    btDefaultMotionState *ms = new btDefaultMotionState(xform);
    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0., ms, shape, btVector3(0,0,0)));
    dynamicsWorld->addRigidBody(body, objects[FLOOR].bit, objects[FLOOR].mask);

    objects[FLOOR].shape = shape;
    objects[FLOOR].body = body;

    //
    // Arms with hinges
    //

    btRigidBody *b1, *b2, *b3;

    // Segment 1

    shape = new btBoxShape(0.5*btVector3(1, 4, 1));

    mass = 1.0;
    shape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(0, 2, 0));
    ms = new btDefaultMotionState(xform);

    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[ARM_SEGMENT_1].bit, objects[ARM_SEGMENT_1].mask);
    b1 = body;

    objects[ARM_SEGMENT_1].shape = shape;
    objects[ARM_SEGMENT_1].body = body;

    // Segment 2

    shape = new btBoxShape(0.5*btVector3(0.8, 6, 0.8));

    mass = 1.0;
    shape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(0, 7, 0));
    ms = new btDefaultMotionState(xform);

    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[ARM_SEGMENT_2].bit, objects[ARM_SEGMENT_2].mask);
    b2 = body;

    objects[ARM_SEGMENT_2].shape = shape;
    objects[ARM_SEGMENT_2].body = body;

    // Segment 3

    shape = new btBoxShape(0.5*btVector3(0.6, 4, 0.6));

    mass = 1.0;
    shape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(0, 12, 0));
    ms = new btDefaultMotionState(xform);

    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[ARM_SEGMENT_3].bit, objects[ARM_SEGMENT_3].mask);
    b3 = body;

    objects[ARM_SEGMENT_3].shape = shape;
    objects[ARM_SEGMENT_3].body = body;

    // Hinges

    btVector3   axisInA, axisInB;
    btHingeConstraint *h;

    axisInA = btVector3(0, 1, 0);
    axisInB = axisInA;

    h = new btHingeConstraint(*(objects[FLOOR].body), *b1,
        btVector3(0, 0, 0), btVector3(0, -2, 0),
        axisInA, axisInB);
    h->setLimit(-1.5*M_PI, 1.5*M_PI);
    rotateConstraint = h;

    dynamicsWorld->addConstraint(h);


    axisInA = btVector3(1, 0, 0);
    axisInB = axisInA;

    h = new btHingeConstraint(*b1, *b2,
        btVector3(0, 2, 0), btVector3(0, -3, 0),
        axisInA, axisInB);
    h->setLimit(-0.5*M_PI, 0.5*M_PI);
    hingeConstraint1 = h;

    dynamicsWorld->addConstraint(h);

    h = new btHingeConstraint(*b2, *b3,
        btVector3(0, 3, 0), btVector3(0, -2, 0),
        axisInA, axisInB);
    h->setLimit(-0.5*M_PI, 0.5*M_PI);
    hingeConstraint2 = h;

    dynamicsWorld->addConstraint(h);

    //
    // Grabbers
    //

    btBoxShape *box1, *box2;
    btCompoundShape *cshape;

    // Grabber 1

    box1 = new btBoxShape(0.5*btVector3(0.1, 1, 0.1));
    box2 = new btBoxShape(0.5*btVector3(0.5, 0.1, 0.1));

    cshape = new btCompoundShape();
    xform.setIdentity();
    xform.setOrigin(btVector3(0, 0.5, 0));
    cshape->addChildShape(xform, box1);
    xform.setIdentity();
    xform.setOrigin(btVector3(0.5*0.5-0.5*0.1, 1+0.05, 0));
    cshape->addChildShape(xform, box2);

    mass = 1.0;
    cshape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(0, 14, 0.08));
    ms = new btDefaultMotionState(xform);

    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, cshape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[GRABBER_1].bit, objects[GRABBER_1].mask);

    axisInA = btVector3(0, 0, 1);
    axisInB = axisInA;

    h = new btHingeConstraint(*b3, *body,
        btVector3(0, 2, 0.08), btVector3(0, 0, 0),
        axisInA, axisInB, true);
    h->setLimit(0.3, 0.5*M_PI);

    dynamicsWorld->addConstraint(h);

    grabberConstraint1 = h;

    objects[GRABBER_1].shape = cshape;
    objects[GRABBER_1].body = body;

    // Grabber 2

    box1 = new btBoxShape(0.5*btVector3(0.1, 1, 0.1));
    box2 = new btBoxShape(0.5*btVector3(0.5, 0.1, 0.1));

    cshape = new btCompoundShape();
    xform.setIdentity();
    xform.setOrigin(btVector3(0, 0.5, 0));
    cshape->addChildShape(xform, box1);
    xform.setIdentity();
    xform.setOrigin(btVector3(-(0.5*0.5-0.5*0.1), 1+0.05, 0));
    cshape->addChildShape(xform, box2);

    mass = 1.0;
    cshape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(0, 14, -0.08));
    ms = new btDefaultMotionState(xform);

    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, cshape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[GRABBER_2].bit, objects[GRABBER_2].mask);

    axisInA = btVector3(0, 0, 1);
    axisInB = axisInA;

    h = new btHingeConstraint(*b3, *body,
        btVector3(0, 2, -0.08), btVector3(0, 0, 0),
        axisInA, axisInB, true);
    h->setLimit(-0.5*M_PI, -0.3);

    dynamicsWorld->addConstraint(h);

    grabberConstraint2 = h;

    objects[GRABBER_2].shape = cshape;
    objects[GRABBER_2].body = body;

    //
    // Boxes on which to put stuff
    //

    // Box 1

    mass = 0.5;
    shape = new btBoxShape(0.5*btVector3(1.5, 6.0, 1.5));
    shape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(9.7, 3.01, 0.0));

    ms = new btDefaultMotionState(xform);
    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[BLOCK_1].bit, objects[BLOCK_1].mask);

    objects[BLOCK_1].shape = shape;
    objects[BLOCK_1].body = body;

    // Box 2

    mass = 0.5;
    shape = new btBoxShape(0.5*btVector3(1.5, 4.0, 1.5));
    shape->calculateLocalInertia(mass, localInertia);

    xform.setIdentity();
    xform.setOrigin(btVector3(-6, 2.01, 4));

    ms = new btDefaultMotionState(xform);
    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, localInertia));
    dynamicsWorld->addRigidBody(body, objects[BLOCK_2].bit, objects[BLOCK_2].mask);

    objects[BLOCK_2].shape = shape;
    objects[BLOCK_2].body = body;

    // And a teapot

    if (teapot_vertices == NULL)
    {
        teapot_vertices = (float*)malloc(3*sizeof(float)*ply_num_vertices);
        teapot_triangles = (int*)malloc(3*sizeof(int)*ply_num_triangles);

        int i, j, k, t;

        // Note: we scale the pot smaller here
        for (i = 0; i < ply_num_vertices; i++)
        {
            teapot_vertices[3*i] = 0.018*ply_vertices[i].x;
            teapot_vertices[3*i+1] = 0.018*ply_vertices[i].y;
            teapot_vertices[3*i+2] = 0.018*ply_vertices[i].z;
        }

        for (t = 0; t < ply_num_triangles; t++)
        {
            teapot_triangles[3*t] = ply_triangles[t].v[0];
            teapot_triangles[3*t+1] = ply_triangles[t].v[1];
            teapot_triangles[3*t+2] = ply_triangles[t].v[2];
        }

        teapot_dlist = glGenLists(1);
        glNewList(teapot_dlist, GL_COMPILE);

        glBegin(GL_TRIANGLES);
        for (t = 0; t < ply_num_triangles; t++)
        {
            i = teapot_triangles[3*t];
            j = teapot_triangles[3*t+1];
            k = teapot_triangles[3*t+2];
            glNormal3f(ply_triangles[t].n.x, ply_triangles[t].n.y, ply_triangles[t].n.z);
            glVertex3f(teapot_vertices[3*i], teapot_vertices[3*i+1], teapot_vertices[3*i+2]);
            glVertex3f(teapot_vertices[3*j], teapot_vertices[3*j+1], teapot_vertices[3*j+2]);
            glVertex3f(teapot_vertices[3*k], teapot_vertices[3*k+1], teapot_vertices[3*k+2]);
        }
        glEnd();

        glEndList();
    }

    btCollisionDispatcher *dispatcher = static_cast<btCollisionDispatcher *>(dynamicsWorld->getDispatcher());
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

    btTriangleIndexVertexArray *m_indexVertexArrays = new btTriangleIndexVertexArray(
        ply_num_triangles, teapot_triangles, 3*sizeof(int),
        ply_num_vertices, teapot_vertices, 3*sizeof(float));

    mass = 1.0;
    btGImpactMeshShape *trimesh = new btGImpactMeshShape(m_indexVertexArrays);
    trimesh->calculateLocalInertia(mass, localInertia);
    trimesh->setLocalScaling(btVector3(1.f,1.f,1.f));
    trimesh->setMargin(0.04f);
    trimesh->updateBound();

    xform.setIdentity();
    xform.setOrigin(btVector3(9.7, 6.7, 0));

    ms = new btDefaultMotionState(xform);
    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, ms, trimesh, localInertia));
    dynamicsWorld->addRigidBody(body, objects[TEAPOT].bit, objects[TEAPOT].mask);

    objects[TEAPOT].shape = trimesh;
    objects[TEAPOT].body = body;

    for (int i = 0; i < num_objects; i++)
        objects[i].body->setActivationState(DISABLE_DEACTIVATION);

    // Prime the simulation, so that all constraints will have been met initially
    for (int i = 0; i < 20; i++)
        dynamicsWorld->stepSimulation(0.01, 10);
}

extern "C" void
reset_physics(void)
{
    // http://www.bulletphysics.com/Bullet/phpBB3/viewtopic.php?f=9&t=3143&p=12304&hilit=resetting#p12304

    for (int i = 0; i < num_objects; i++)
        dynamicsWorld->removeRigidBody(objects[i].body);

    // Reset body properties

    for (int i = 0; i < num_objects; i++)
    {
        objects[i].body->forceActivationState(ACTIVE_TAG);
        objects[i].body->setDeactivationTime(0);
    }

    overlappingPairCache->resetPool(dispatcher);

    solver->reset();

    for (int i = 0; i < num_objects; i++)
        dynamicsWorld->addRigidBody(objects[i].body, objects[i].bit, objects[i].mask);

}

extern "C" void
destroy_physics(void)
{
    // Ugly, but it works
    dynamicsWorld->removeConstraint(grabberConstraint2);
    dynamicsWorld->removeConstraint(grabberConstraint1);
    dynamicsWorld->removeConstraint(hingeConstraint2);
    dynamicsWorld->removeConstraint(hingeConstraint1);
    dynamicsWorld->removeConstraint(rotateConstraint);
    delete rotateConstraint;
    delete hingeConstraint1;
    delete hingeConstraint2;
    delete grabberConstraint1;
    delete grabberConstraint2;

    for (int i = 0; i < num_objects; i++)
    {
        dynamicsWorld->removeCollisionObject(objects[i].body);

		if (objects[i].body->getMotionState())
			delete objects[i].body->getMotionState();

        delete objects[i].body;
        delete objects[i].shape;
    }

    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}

extern "C" void
sim_step(float step, float rotate_joint_vel, float hinge_joint1_vel,
    float hinge_joint2_vel, float grabber_joint_vel)
{
    rotateConstraint->enableAngularMotor(true, rotate_joint_vel, 10000);
    hingeConstraint1->enableAngularMotor(true, hinge_joint1_vel, 10000);
    hingeConstraint2->enableAngularMotor(true, hinge_joint2_vel, 10000);
    grabberConstraint1->enableAngularMotor(true, grabber_joint_vel, 10000);
    grabberConstraint2->enableAngularMotor(true, -grabber_joint_vel, 10000);

    dynamicsWorld->stepSimulation(step, 20);
}

static void
draw_box(btBoxShape *box, btRigidBody *body, btTransform& t)
{
    GLfloat m[16];

    btTransform trans;
    body->getMotionState()->getWorldTransform(trans);

    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT);

        glEnable(GL_RESCALE_NORMAL);
        glEnable(GL_NORMALIZE);

        // Dang, that's convenient!
        trans.getOpenGLMatrix(m);
        glMultMatrixf(m);

        t.getOpenGLMatrix(m);
        glMultMatrixf(m);

        // We get *half* the side lengths back
        btVector3 sz = box->getHalfExtentsWithMargin();
        glScalef(sz.getX(), sz.getY(), sz.getZ());
        glutSolidCube(2.0);

    glPopAttrib();
    glPopMatrix();
}

extern "C" void
draw_objects(void)
{
    btCollisionObject *obj;
    btCollisionShape *shape;
    btRigidBody *body;
    btTransform xform;
    GLfloat m[16];

    // Note: we skip object [0], which is the floor
    for (int i = 1; i < dynamicsWorld->getNumCollisionObjects(); i++)
    {
        obj = dynamicsWorld->getCollisionObjectArray()[i];
        shape = obj->getCollisionShape();
        body = btRigidBody::upcast(obj);

        if (body && body->getMotionState())
        {
            btBoxShape *box = dynamic_cast<btBoxShape*>(shape);
            if (box)
            {
                btTransform doh;
                doh.setIdentity();
                draw_box(box, body, doh);
            }
            else if (dynamic_cast<btCompoundShape*>(shape))
            {
                btCompoundShape *cshape = static_cast<btCompoundShape*>(shape);
                for (int c = 0; c < cshape->getNumChildShapes(); c++)
                {
                    btBoxShape *box = static_cast<btBoxShape*>(cshape->getChildShape(c));
                    glPushMatrix();
                        draw_box(box, body, cshape->getChildTransform(c));
                    glPopMatrix();
                }
            }
            else if (dynamic_cast<btGImpactMeshShape*>(shape))
            {
                // Teapot, as we don't have any other gimpact meshes
                btTransform trans;
                body->getMotionState()->getWorldTransform(trans);
                trans.getOpenGLMatrix(m);

                glMultMatrixf(m);
                // We draw the teapot slightly larger here, as the collision
                // margin will keep objects separated even when they are in
                // contact and this looks kind of weird.
                glScalef(1.03, 1.03, 1.03);
                glCallList(teapot_dlist);
            }
        }
    }
}
