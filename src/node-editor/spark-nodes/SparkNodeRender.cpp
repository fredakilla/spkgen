#include "SparkNodeRender.h"

#include <Spark/SPARK.h>
#include <Urhox/Spark/SparkParticle.h>
#include <Urhox/Spark/SparkParticleEffect.h>
#include <Urhox/Spark/SPK_Urho3D_QuadRenderer.h>
#include "SpkUtils.h"
#include "../../UrhoDevice.h"

#include <Urho3D/Urho3DAll.h>


static Vector3 PointOnSphere(const Sphere& sphere, unsigned theta, unsigned phi)
{
    return Vector3(
        sphere.center_.x_ + sphere.radius_ * Sin((float)theta) * Sin((float)phi),
        sphere.center_.y_ + sphere.radius_ * Cos((float)phi),
        sphere.center_.z_ + sphere.radius_ * Cos((float)theta) * Sin((float)phi)
    );
}

void DrawDebugSphere(const Sphere& sphere, const Matrix3x4& transform, const Color& color, bool depthTest, DebugRenderer* debugDraw)
{
    unsigned uintColor = color.ToUInt();

    unsigned step = 28;

    for (unsigned j = 0; j < 180; j += step)
    {
        for (unsigned i = 0; i < 360; i += step)
        {
            Vector3 p1 = transform * PointOnSphere(sphere, i, j);
            Vector3 p2 = transform * PointOnSphere(sphere, i + step, j);
            Vector3 p3 = transform * PointOnSphere(sphere, i, j + step);
            Vector3 p4 = transform * PointOnSphere(sphere, i + step, j + step);

            debugDraw->AddLine(p1, p2, uintColor, depthTest);
            debugDraw->AddLine(p3, p4, uintColor, depthTest);
            debugDraw->AddLine(p1, p3, uintColor, depthTest);
            debugDraw->AddLine(p2, p4, uintColor, depthTest);
        }
    }
}

void DrawDebugBox(const BoundingBox& box, const Matrix3x4& transform, const Color& color, bool depthTest, DebugRenderer* debugDraw)
{
    const Vector3& min = box.min_;
    const Vector3& max = box.max_;

    Vector3 v0(transform * min);
    Vector3 v1(transform * Vector3(max.x_, min.y_, min.z_));
    Vector3 v2(transform * Vector3(max.x_, max.y_, min.z_));
    Vector3 v3(transform * Vector3(min.x_, max.y_, min.z_));
    Vector3 v4(transform * Vector3(min.x_, min.y_, max.z_));
    Vector3 v5(transform * Vector3(max.x_, min.y_, max.z_));
    Vector3 v6(transform * Vector3(min.x_, max.y_, max.z_));
    Vector3 v7(transform * max);

    unsigned uintColor = color.ToUInt();

    debugDraw->AddLine(v0, v1, uintColor, depthTest);
    debugDraw->AddLine(v1, v2, uintColor, depthTest);
    debugDraw->AddLine(v2, v3, uintColor, depthTest);
    debugDraw->AddLine(v3, v0, uintColor, depthTest);
    debugDraw->AddLine(v4, v5, uintColor, depthTest);
    debugDraw->AddLine(v5, v7, uintColor, depthTest);
    debugDraw->AddLine(v7, v6, uintColor, depthTest);
    debugDraw->AddLine(v6, v4, uintColor, depthTest);
    debugDraw->AddLine(v0, v4, uintColor, depthTest);
    debugDraw->AddLine(v1, v5, uintColor, depthTest);
    debugDraw->AddLine(v2, v7, uintColor, depthTest);
    debugDraw->AddLine(v3, v6, uintColor, depthTest);
}

void DrawDebugCylinder(const Vector3& position, float radius, float halfHeight, const Quaternion& rotation, const Color& color, bool depthTest, DebugRenderer* debugDraw)
{
    Sphere sphere(Vector3::ZERO, radius);
    Vector3 halfHeightVec = rotation * Vector3(0, halfHeight, 0);
    Vector3 offsetXVec = rotation * Vector3(radius, 0, 0);
    Vector3 offsetZVec = rotation * Vector3(0, 0, radius);
    for (unsigned i = 0; i < 360; i += 45)
    {
        Vector3 p1 = rotation * PointOnSphere(sphere, i, 90) + position;
        Vector3 p2 = rotation * PointOnSphere(sphere, i + 45, 90) + position;
        debugDraw->AddLine(p1 - halfHeightVec, p2 - halfHeightVec, color, depthTest);
        debugDraw->AddLine(p1 + halfHeightVec, p2 + halfHeightVec, color, depthTest);
    }
    debugDraw->AddLine(position - halfHeightVec + offsetXVec, position + halfHeightVec + offsetXVec, color, depthTest);
    debugDraw->AddLine(position - halfHeightVec - offsetXVec, position + halfHeightVec - offsetXVec, color, depthTest);
    debugDraw->AddLine(position - halfHeightVec + offsetZVec, position + halfHeightVec + offsetZVec, color, depthTest);
    debugDraw->AddLine(position - halfHeightVec - offsetZVec, position + halfHeightVec - offsetZVec, color, depthTest);
}


void createDebugGeomteriesFromZone(const SPK::Ref<SPK::Zone> zone, Urho3D::SharedPtr<Urho3D::DebugRenderer> debugDraw)
{
    // every zone has a position
    const SPK::Vector3D pos = zone->getPosition();

    if(zone->getClassName() == "Point")
    {
        Matrix3x4 mat;
        mat.SetTranslation(Vector3(pos.x, pos.y, pos.z));
        Sphere sphere;
        sphere.radius_ = 0.1f;
        DrawDebugSphere(sphere, mat, Color::YELLOW, true, debugDraw);
        //DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.1f, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Sphere")
    {
        /*const SPK::Sphere* sphere = dynamic_cast<SPK::Sphere*>(zone.get());
        GP_ASSERT(sphere);
        float radius = sphere->getRadius();
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), radius, Vector3(1,1,1));*/

        const SPK::Sphere* spkSphere = dynamic_cast<SPK::Sphere*>(zone.get());
        assert(spkSphere);

        Matrix3x4 mat;
        mat.SetTranslation(Vector3(pos.x, pos.y, pos.z));
        Sphere sphere;
        sphere.radius_ = spkSphere->getRadius();
        DrawDebugSphere(sphere, mat, Color::WHITE, true, debugDraw);
    }
    else if(zone->getClassName() == "Plane")
    {
        /*const SPK::Plane* plane = dynamic_cast<SPK::Plane*>(zone.get());
        GP_ASSERT(plane);
        const SPK::Vector3D normal = plane->getNormal();
        DebugDraw::getInstance()->drawPlane(Vector3(normal.x, normal.y, normal.z), 0.0f, Matrix::identity(), Vector3(1,1,1));*/
    }
    else if(zone->getClassName() == "Box")
    {
        const SPK::Box* spkBox = dynamic_cast<SPK::Box*>(zone.get());
        assert(spkBox);

        Vector3 scale = ToUrhoVector3(spkBox->getDimensions());
        Vector3 pos = ToUrhoVector3(spkBox->getPosition());

        // todo fix rotation
        Vector3 direction = ToUrhoVector3(spkBox->getTransform().getLocalSide());
        Vector3 up = ToUrhoVector3(spkBox->getTransform().getLocalUp());
        Quaternion rot;
        rot.FromLookRotation(direction,up);

        Matrix3x4 matrix;
        matrix.SetTranslation(pos);
        matrix.SetRotation(rot.RotationMatrix());

        BoundingBox box(-scale/2, scale/2);
        DrawDebugBox(box, matrix, Color::WHITE, true, debugDraw);


        /*const SPK::Box* box = dynamic_cast<SPK::Box*>(zone.get());
        GP_ASSERT(box);
        Vector3 scale = ToGplayVector3(box->getDimensions());
        Matrix matrix;
        // todo: fix rotation
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        BoundingBox bbox(-scale/2.0f, scale/2.0f);
        bbox *= matrix;
        DebugDraw::getInstance()->drawBox(bbox.min, bbox.max, Vector3(1,1,1));*/
    }
    else if(zone->getClassName() == "Cylinder")
    {
        /*const SPK::Cylinder* cylinder = dynamic_cast<SPK::Cylinder*>(zone.get());
        GP_ASSERT(cylinder);

        const SPK::Vector3D axis = cylinder->getAxis();
        float height = cylinder->getHeight();
        float radius = cylinder->getRadius();

        // todo: fix rotation
        Matrix matrix;
        Matrix::createTranslation(ToGplayVector3(pos), &matrix);
        DebugDraw::getInstance()->drawCylinder(radius, height/2.0f, 1, matrix, Vector3(1,1,1));*/
    }
    else if(zone->getClassName() == "Ring")
    {
        /*const SPK::Ring* ring = dynamic_cast<SPK::Ring*>(zone.get());
        GP_ASSERT(ring);

        float minRadius = ring->getMinRadius();
        float maxRadius = ring->getMaxRadius();

        // todo: fix rotation

        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), minRadius, minRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), maxRadius, maxRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
    */
    }
}



void drawDebugShapes(Urho3D::SparkParticle* spark, Urho3D::SharedPtr<Urho3D::DebugRenderer> debugDraw)
{
    SPK::Ref<SPK::System> spkSystem = spark->GetSystem();
    for(size_t nGroup = 0; nGroup < spkSystem->getNbGroups(); nGroup++)
    {
        // show emitters zones
        for(size_t nEmitter = 0; nEmitter < spkSystem->getGroup(nGroup)->getNbEmitters(); nEmitter++)
        {
            const SPK::Ref<SPK::Zone> zone = spkSystem->getGroup(nGroup)->getEmitter(nEmitter)->getZone();
            createDebugGeomteriesFromZone(zone, debugDraw);
        }

        // show modifiers zones
        for(size_t nModifiers = 0; nModifiers < spkSystem->getGroup(nGroup)->getNbModifiers(); nModifiers++)
        {
            const SPK::Ref<SPK::Modifier> modifier = spkSystem->getGroup(nGroup)->getModifier(nModifiers);

            if(modifier->getClassName() == "PointMass")
            {
                /*const SPK::PointMass* pointMass = dynamic_cast<SPK::PointMass*>(modifier.get());
                GP_ASSERT(pointMass);
                const SPK::Vector3D pos = pointMass->getPosition();
                DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.25f, Vector3(0,1,0));*/
            }
            else if(modifier->getClassName() == "Destroyer")
            {
                /*const SPK::Destroyer* destroyer = dynamic_cast<SPK::Destroyer*>(modifier.get());
                GP_ASSERT(destroyer);
                createDebugGeomteriesFromZone(destroyer->getZone());*/
            }
            else if(modifier->getClassName() == "Obstacle")
            {
                /*const SPK::Obstacle* obstacle = dynamic_cast<SPK::Obstacle*>(modifier.get());
                GP_ASSERT(obstacle);
                createDebugGeomteriesFromZone(obstacle->getZone());*/
            }
            else if(modifier->getClassName() == "LinearForce")
            {
                /*const SPK::LinearForce* linearForce = dynamic_cast<SPK::LinearForce*>(modifier.get());
                GP_ASSERT(linearForce);
                createDebugGeomteriesFromZone(linearForce->getZone());*/
            }
        }
    }
}






SparkNodeRender::SparkNodeRender() :
    BaseRenderer3D(),
    _isShowDebug(true),
    _sparkNode(nullptr)
{
    if(_isShowDebug)
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SparkNodeRender, handleUpdate));
}

void SparkNodeRender::setCurentParticleSystem(SPK::Ref<SPK::System> sparkSystem)
{
    Urho3D::Node* node = _scene->GetChild("SparkNode");
    if(node)
        _scene->RemoveChild(node);

    // Create a node in scene and attach spark effect
    Urho3D::Node* sparkNode = _scene->CreateChild("SparkNode");
    sparkNode->SetPosition(Urho3D::Vector3(0.0f, 0.0f, 0.0f));
    Urho3D::SparkParticle* spk1 = sparkNode->CreateComponent<Urho3D::SparkParticle>();
    spk1->SetSystem(sparkSystem);

    _sparkNode = sparkNode;

    // Look at spark node for orbit camera
    _cameraController->setTarget(sparkNode->GetPosition()); 
}

void SparkNodeRender::handleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (_sparkNode)
    {
        Urho3D::SparkParticle* spk = _sparkNode->GetComponent<Urho3D::SparkParticle>();
        if (spk)
        {
            // show debug shapes for all zones in this spark system
            drawDebugShapes(spk, _debugRenderer);
        }
    }
}

void SparkNodeRender::update(float elapsedTime)
{
   /* // call super class method
    GPRenderer3D::update(elapsedTime);

    // show framebuffer inside an imgui window
    ImGui::Begin("Game rendering");
    {
        bgfx::TextureHandle th = _frameBuffer->getRenderTarget(0)->getTexture()->getHandle()->getHandle();
        ImGui::Image(th, ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, -1) );
    }
    ImGui::End();

    // update emitters
    _scene->visit(this, &SparkNodeRender::updateEmitters, elapsedTime);*/
}


void SparkNodeRender::render(float elapsedTime)
{
   /* View::getView(1)->bind();
    _frameBuffer->bind();
    _scene->visit(this, &SparkNodeRender::drawScene);

    View::getView(0)->bind();
   _scene->visit(this, &SparkNodeRender::drawScene);*/
}

bool SparkNodeRender::drawScene(Urho3D::Node* node)
{
   /* Drawable* drawable = node->getDrawable();
    if (drawable)
        drawable->draw();
    return true;*/
}

bool SparkNodeRender::updateEmitters(Urho3D::Node* node, float elapsedTime)
{
    /*SparkParticleEmitter* spkEffect = dynamic_cast<SparkParticleEmitter*>(node->getDrawable());
    if (spkEffect)
        spkEffect->update(elapsedTime);

    // show debug shapes for all zones in spark system
    if(spkEffect && _isShowDebug)
    {
        drawDebugShapes(spkEffect, _scene);
    }

    return true;*/
}

