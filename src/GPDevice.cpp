#include "GPDevice.h"

GPDevice* GPDevice::_instance = nullptr;

#include <spark/SPARK.h>
#include "sparkparticles/SparkParticleEmitter.h"
#include "sparkparticles/SparkQuadRenderer.h"
#include "sparkparticles/SparkParticleEmitter.h"

#include "BaseNode.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------
// Debug drawer
//-----------------------------------------------------------------------------------------------------------------------------------------------

#include <LinearMath/btIDebugDraw.h>

/**
 * Use the btIDebugDraw interface for generating shapes using bullet DebugDraw and rendering with gplay-engine.
 * You shouln't use this class directly to avoid to use bullet types prefer the DebugDrawer class below that use gplay types.
 */
class BulletDebugDraw : public btIDebugDraw
{
public:

    BulletDebugDraw()
    {
        Effect* effect = Effect::createFromFile("res/core/shaders/physics.vert", "res/core/shaders/physics.frag");

        Material* material = Material::create(effect);
        GP_ASSERT(material && material->getStateBlock());
        material->getStateBlock()->setDepthTest(true);
        material->getStateBlock()->setDepthFunction(RenderState::DEPTH_LEQUAL);

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::COLOR, 4),
        };
        _meshBatch = MeshBatch::create(VertexFormat(elements, 2), Mesh::LINES, material, false, 4096, 4096);
        SAFE_RELEASE(material);
        SAFE_RELEASE(effect);
    }

    ~BulletDebugDraw()
    {
        SAFE_DELETE(_meshBatch);
    }

    void begin(const Matrix& viewProjection)
    {
        GP_ASSERT(_meshBatch);
        _meshBatch->start();
        _meshBatch->getMaterial()->getParameter("u_viewProjectionMatrix")->setValue(viewProjection);
    }

    void end()
    {
        GP_ASSERT(_meshBatch && _meshBatch->getMaterial());
        _meshBatch->finish();
        _meshBatch->draw();
        _lineCount = 0;
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override
    {
        GP_ASSERT(_meshBatch);

        static BulletDebugDraw::DebugVertex vertices[2];

        vertices[0].x = from.getX();
        vertices[0].y = from.getY();
        vertices[0].z = from.getZ();
        vertices[0].r = fromColor.getX();
        vertices[0].g = fromColor.getY();
        vertices[0].b = fromColor.getZ();
        vertices[0].a = 1.0f;

        vertices[1].x = to.getX();
        vertices[1].y = to.getY();
        vertices[1].z = to.getZ();
        vertices[1].r = toColor.getX();
        vertices[1].g = toColor.getY();
        vertices[1].b = toColor.getZ();
        vertices[1].a = 1.0f;

        _meshBatch->add(vertices, 2);

        ++_lineCount;
        if (_lineCount >= 4096)
        {
            // Flush the batch when it gets full (don't want to to grow infinitely)
            end();
            _meshBatch->start();
        }
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
    {
        drawLine(from, to, color, color);
    }

    static const btTransform toBtTransform(const Matrix& matrix)
    {
        Vector3 s;      // scale is not used in btTransfrom, shape will no be scaled.
        Quaternion r;
        Vector3 t;
        matrix.decompose(&s, &r, &t);

        /*btMatrix3x3 btMat;
        btMat.setFromOpenGLSubMatrix(&matrix.m[0]);
        //btMat.setRotation(BQ(r));
        //btMat.scaled(BV(s));
        return btTransform(btMat, BV(t));*/

        return btTransform(BQ(r), BV(t));
    }

    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override {}
    virtual void reportErrorWarning(const char* warningString) override {}
    virtual void draw3dText(const btVector3& location,const char* textString) override {}
    virtual void setDebugMode(int debugMode) override {}
    virtual int	getDebugMode() const override {}

private:

    struct DebugVertex
    {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
        float a;
    };

    MeshBatch* _meshBatch;
    int _lineCount;
};



/**
 * Wrapper to use the BulletDebugDraw with gplay types.
 */
class DebugDrawer
{
private:
    BulletDebugDraw dd;

public:
    void begin(const Matrix& viewProjection);
    void end();
    void drawLine(const Vector3& from, const Vector3& to, const Vector3& color);
    void drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor);
    void drawBox(const Vector3& bbMin, const Vector3& bbMax, const Vector3& color);
    void drawBox(const Vector3& bbMin, const Vector3& bbMax, const Matrix& matrix, const Vector3& color);
    void drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& color);
    void drawSphere(float radius, const Matrix& matrix, const Vector3& color);
    void drawSphere(const Vector3& p, float radius, const Vector3& color);
    void drawCapsule(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color);
    void drawCylinder(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color);
    void drawCone(float radius, float height, int upAxis, const Matrix& matrix, const Vector3& color);
    void drawPlane(const Vector3& planeNormal, float planeConst, const Matrix& matrix, const Vector3 &color);
    void drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radiusA, float radiusB, float minAngle, float maxAngle,
                 const Vector3& color, bool drawSect, float stepDegrees = float(10.f));
    void drawTransform(const Matrix& matrix, float orthoLen);
    void drawAabb(const Vector3& from, const Vector3& to, const Vector3& color);
};


void DebugDrawer::begin(const Matrix& viewProjection)
{
    dd.begin(viewProjection);
}

void DebugDrawer::end()
{
    dd.end();
}

void DebugDrawer::drawLine(const Vector3& from, const Vector3& to, const Vector3& color)
{
    dd.drawLine(BV(from), BV(to), BV(color), BV(color));
}

void DebugDrawer::drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor)
{
    dd.drawLine(BV(from), BV(to), BV(fromColor), BV(toColor));
}

void DebugDrawer::drawBox(const Vector3& bbMin, const Vector3& bbMax, const Vector3& color)
{
    dd.drawBox(BV(bbMin), BV(bbMax), BV(color));
}

void DebugDrawer::drawBox(const Vector3& bbMin, const Vector3& bbMax, const Matrix& matrix, const Vector3& color)
{
    dd.drawBox(BV(bbMin), BV(bbMax), BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& color)
{
    drawLine(v0,v1,color);
    drawLine(v1,v2,color);
    drawLine(v2,v0,color);
}

void DebugDrawer::drawSphere(float radius, const Matrix& matrix, const Vector3& color)
{
    dd.drawSphere(radius, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawSphere(const Vector3& p, float radius, const Vector3& color)
{
    dd.drawSphere(BV(p), radius, BV(color));
}

void DebugDrawer::drawCapsule(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCapsule(radius, halfHeight, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawCylinder(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCylinder(radius, halfHeight, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawCone(float radius, float height, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCone(radius, height, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawPlane(const Vector3& planeNormal, float planeConst, const Matrix& matrix, const Vector3& color)
{
    dd.drawPlane(BV(planeNormal), planeConst, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radiusA, float radiusB, float minAngle, float maxAngle,
             const Vector3& color, bool drawSect, float stepDegrees)
{
    dd.drawArc(BV(center), BV(normal), BV(axis), radiusA, radiusB, minAngle, maxAngle, BV(color), drawSect, stepDegrees);
}

void DebugDrawer::drawTransform(const Matrix& matrix, float orthoLen)
{
    dd.drawTransform(BulletDebugDraw::toBtTransform(matrix), orthoLen);
}

void DebugDrawer::drawAabb(const Vector3& from, const Vector3& to, const Vector3& color)
{
    dd.drawAabb(BV(from), BV(to), BV(color));
}



//-----------------------------------------------------------------------------------------------------------------------------------------------
// GPDevice
//-----------------------------------------------------------------------------------------------------------------------------------------------

DebugDrawer* _debugDraw = nullptr;


GPDevice& GPDevice::get()
{
    if(!_instance)
    {
        static GPDevice static_instance;
        _instance = &static_instance;
    }
    return *_instance;
}

GPDevice::GPDevice() :
    _platform(nullptr),
    _scene(nullptr),
    _isShowDebug(true)
{

}

GPDevice::~GPDevice()
{

}

void GPDevice::createRenderWindow(void* hwnd)
{
    _platform = gplay::Platform::create(this, hwnd);
    GP_ASSERT(_platform);

    _platform->start();

    //gplay::Renderer::getInstance().toggleDebugStats();

    View::create(0, Rectangle(200, 200), View::ClearFlags::COLOR_DEPTH, 0x556677ff, 1.0f, 0);
}

void GPDevice::frame()
{
    _platform->processEvents();
    _platform->frame();
}

void GPDevice::stop()
{
    _platform->stop();
}

void GPDevice::resizeRenderView(int width, int height)
{
    _platform->setWindowSize(width, height);
    View::getView(0)->setViewRect(Rectangle(width, height));


    if(_scene)
    {
        Camera* camera = _scene->getActiveCamera();
        if(camera)
        {
            float ratio = (float)width / (float)height;
            camera->setAspectRatio(ratio);
        }
    }

}

void GPDevice::initialize()
{
    // Create a new empty scene.
    _scene = Scene::create();

    // create camera
    Camera* camera = Camera::createPerspective(45.0f, 1.77f, 0.1f, 100.0f);
    Node* cameraNode = _scene->addNode("camera");
    cameraNode->setCamera(camera);
    cameraNode->setTranslation(Vector3(0,0,10));
    _scene->setActiveCamera(camera);

    // create debug drawer
    _debugDraw = new DebugDrawer();
}

void GPDevice::finalize()
{
    delete _debugDraw;
}

void GPDevice::update(float elapsedTime)
{
    _scene->visit(this, &GPDevice::updateEmitters, elapsedTime);
}

void GPDevice::render(float elapsedTime)
{
    bgfx::touch(0);

    View::getView(0)->bind();
    _scene->visit(this, &GPDevice::drawScene);
}

bool GPDevice::drawScene(Node* node)
{
    Drawable* drawable = node->getDrawable();
    if (drawable)
        drawable->draw();
    return true;
}

void createDebugGeomteriesFromZone(const SPK::Ref<SPK::Zone> zone)
{
    // every zone has a position
    const SPK::Vector3D pos = zone->getPosition();

    if(zone->getClassName() == "Point")
    {
        _debugDraw->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.1f, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Sphere")
    {
        const SPK::Sphere* sphere = dynamic_cast<SPK::Sphere*>(zone.get());
        GP_ASSERT(sphere);
        float radius = sphere->getRadius();
        _debugDraw->drawSphere(Vector3(pos.x, pos.y, pos.z), radius, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Plane")
    {
        const SPK::Plane* plane = dynamic_cast<SPK::Plane*>(zone.get());
        GP_ASSERT(plane);
        const SPK::Vector3D normal = plane->getNormal();
        _debugDraw->drawPlane(Vector3(normal.x, normal.y, normal.z), 0.0f, Matrix::identity(), Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Box")
    {
        const SPK::Box* box = dynamic_cast<SPK::Box*>(zone.get());
        GP_ASSERT(box);
        Vector3 scale = ToGplayVector3(box->getDimensions());
        Matrix matrix;
        // todo: fix rotation
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        BoundingBox bbox(-scale/2.0f, scale/2.0f);
        bbox *= matrix;
        _debugDraw->drawBox(bbox.min, bbox.max, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Cylinder")
    {
        const SPK::Cylinder* cylinder = dynamic_cast<SPK::Cylinder*>(zone.get());
        GP_ASSERT(cylinder);

        const SPK::Vector3D axis = cylinder->getAxis();
        float height = cylinder->getHeight();
        float radius = cylinder->getRadius();

        // todo: fix rotation
        Matrix matrix;
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        _debugDraw->drawCylinder(radius, height/2.0f, 1, matrix, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Ring")
    {
        const SPK::Ring* ring = dynamic_cast<SPK::Ring*>(zone.get());
        GP_ASSERT(ring);

        float minRadius = ring->getMinRadius();
        float maxRadius = ring->getMaxRadius();

        // todo: fix rotation

        _debugDraw->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), minRadius, minRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
        _debugDraw->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), maxRadius, maxRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
    }
}

void drawDebugShapes(SparkParticleEmitter* spkEffect, Scene* scene)
{
    _debugDraw->begin(scene->getActiveCamera()->getViewProjectionMatrix());

    SPK::Ref<SPK::System> spkSystem = spkEffect->getSparkSystem();
    for(size_t nGroup = 0; nGroup < spkSystem->getNbGroups(); nGroup++)
    {
        // show emitters zones
        for(size_t nEmitter = 0; nEmitter < spkSystem->getGroup(nGroup)->getNbEmitters(); nEmitter++)
        {
            const SPK::Ref<SPK::Zone> zone = spkSystem->getGroup(nGroup)->getEmitter(nEmitter)->getZone();
            createDebugGeomteriesFromZone(zone);
        }

        // show modifiers zones
        for(size_t nModifiers = 0; nModifiers < spkSystem->getGroup(nGroup)->getNbModifiers(); nModifiers++)
        {
            const SPK::Ref<SPK::Modifier> modifier = spkSystem->getGroup(nGroup)->getModifier(nModifiers);

            if(modifier->getClassName() == "PointMass")
            {
                const SPK::PointMass* pointMass = dynamic_cast<SPK::PointMass*>(modifier.get());
                GP_ASSERT(pointMass);
                const SPK::Vector3D pos = pointMass->getPosition();
                _debugDraw->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.25f, Vector3(0,1,0));
            }

            if(modifier->getClassName() == "Destroyer")
            {
                const SPK::Destroyer* destroyer = dynamic_cast<SPK::Destroyer*>(modifier.get());
                GP_ASSERT(destroyer);
                createDebugGeomteriesFromZone(destroyer->getZone());
            }
        }
    }

    _debugDraw->end();
}

bool GPDevice::updateEmitters(Node* node, float elapsedTime)
{
    SparkParticleEmitter* spkEffect = dynamic_cast<SparkParticleEmitter*>(node->getDrawable());
    if (spkEffect)
        spkEffect->update(elapsedTime);

    // show debug shapes for all zones in spark system
    if(spkEffect && _debugDraw && _isShowDebug)
    {
        drawDebugShapes(spkEffect, _scene);
    }

    return true;
}

void GPDevice::setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem)
{
    Node* node = _scene->findNode("sparkSystem");
    if(node)
        _scene->removeNode(node);

    // Create a node in scene and attach spark foutain effect
    SparkParticleEmitter* foutainEmitter = SparkParticleEmitter::createRef(sparkSystem, true);
    Node* particleNode = Node::create("sparkSystem");
    particleNode->setDrawable(foutainEmitter);
    particleNode->setTranslation(0.0f, 0.0f, 0.0f);

    _scene->addNode(particleNode);
}
