#include "SparkNodeRender.h"

#include <Spark/SPARK.h>
#include <Urhox/Spark/SparkParticle.h>
#include <Urhox/Spark/SparkParticleEffect.h>
#include <Urhox/Spark/SPK_Urho3D_QuadRenderer.h>
#include "SpkUtils.h"


void createDebugGeomteriesFromZone(const SPK::Ref<SPK::Zone> zone)
{
   /* // every zone has a position
    const SPK::Vector3D pos = zone->getPosition();

    if(zone->getClassName() == "Point")
    {
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.1f, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Sphere")
    {
        const SPK::Sphere* sphere = dynamic_cast<SPK::Sphere*>(zone.get());
        GP_ASSERT(sphere);
        float radius = sphere->getRadius();
        DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), radius, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Plane")
    {
        const SPK::Plane* plane = dynamic_cast<SPK::Plane*>(zone.get());
        GP_ASSERT(plane);
        const SPK::Vector3D normal = plane->getNormal();
        DebugDraw::getInstance()->drawPlane(Vector3(normal.x, normal.y, normal.z), 0.0f, Matrix::identity(), Vector3(1,1,1));
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
        DebugDraw::getInstance()->drawBox(bbox.min, bbox.max, Vector3(1,1,1));
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
        DebugDraw::getInstance()->drawCylinder(radius, height/2.0f, 1, matrix, Vector3(1,1,1));
    }
    else if(zone->getClassName() == "Ring")
    {
        const SPK::Ring* ring = dynamic_cast<SPK::Ring*>(zone.get());
        GP_ASSERT(ring);

        float minRadius = ring->getMinRadius();
        float maxRadius = ring->getMaxRadius();

        // todo: fix rotation

        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), minRadius, minRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
        DebugDraw::getInstance()->drawArc(ToGplayVector3(pos), Vector3(0,1,0), Vector3(1,0,0), maxRadius, maxRadius, 0.0f, MATH_DEG_TO_RAD(360.0f), Vector3(1,1,1), false);
    }*/
}
/*
void drawDebugShapes(SparkParticleEmitter* spkEffect, Scene* scene)
{
    DebugDraw::getInstance()->begin(scene->getActiveCamera()->getViewProjectionMatrix());

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
                DebugDraw::getInstance()->drawSphere(Vector3(pos.x, pos.y, pos.z), 0.25f, Vector3(0,1,0));
            }
            else if(modifier->getClassName() == "Destroyer")
            {
                const SPK::Destroyer* destroyer = dynamic_cast<SPK::Destroyer*>(modifier.get());
                GP_ASSERT(destroyer);
                createDebugGeomteriesFromZone(destroyer->getZone());
            }
            else if(modifier->getClassName() == "Obstacle")
            {
                const SPK::Obstacle* obstacle = dynamic_cast<SPK::Obstacle*>(modifier.get());
                GP_ASSERT(obstacle);
                createDebugGeomteriesFromZone(obstacle->getZone());
            }
            else if(modifier->getClassName() == "LinearForce")
            {
                const SPK::LinearForce* linearForce = dynamic_cast<SPK::LinearForce*>(modifier.get());
                GP_ASSERT(linearForce);
                createDebugGeomteriesFromZone(linearForce->getZone());
            }
        }
    }

    DebugDraw::getInstance()->end();
}
*/





SparkNodeRender::SparkNodeRender() :
    BaseRenderer3D(),
    _isShowDebug(true)
{


   /* // create a framebuffer for scene preview in imgui window

    const int frameBufferWidth = 512;
    const int frameBufferHeight = 512;

    Texture* texColor = Texture::create("targetColor", frameBufferWidth, frameBufferHeight, Texture::Format::RGBA, Texture::Type::TEXTURE_RT);
    Texture* texDepth = Texture::create("targetDepth", frameBufferWidth, frameBufferHeight, Texture::Format::D16, Texture::Type::TEXTURE_RT);
    std::vector<Texture*> textures;
    textures.push_back(texColor);
    textures.push_back(texDepth);
    _frameBuffer = FrameBuffer::create("MyFrameBuffer", textures);

    View::create(1, Rectangle(frameBufferWidth, frameBufferHeight), View::ClearFlags::COLOR_DEPTH, 0x000000ff, 1.0f, 0);


    // show gplay in game editor
    Game::getInstance()->showEditor(_scene);*/

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

    // Look at spark node for orbit camera
    _cameraController->setTarget(sparkNode->GetPosition());


   /* Node* node = _scene->findNode("SparkNode");
    if(node)
        _scene->removeNode(node);

    // Create a node in scene and attach spark effect
    SparkParticleEmitter* sparkEffect = SparkParticleEmitter::create(sparkSystem, true);
    Node* sparkNode = Node::create("SparkNode");
    sparkNode->setDrawable(sparkEffect);
    sparkNode->setTranslation(0.0f, 0.0f, 0.0f);

    _scene->addNode(sparkNode);*/
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

