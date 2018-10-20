#include <spark/SPARK.h>
#include <sparkparticles/SparkParticleEmitter.h>
#include <sparkparticles/SparkQuadRenderer.h>
#include <sparkparticles/SparkParticleEmitter.h>

#include "SpkSystem.h"
#include "GPDevice.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>


//------------------------------------------------------------------------------------------------------------------------------
// group list
//------------------------------------------------------------------------------------------------------------------------------

NodeSparkGroupList::NodeSparkGroupList()
{
    IN_PORT(ENC_GROUP, "group");
    IN_PORT(ENC_GROUP, "group");
    IN_PORT(ENC_GROUP, "group");
    IN_PORT(ENC_GROUP, "group");
    IN_PORT(ENC_GROUP, "group");
    OUT_PORT(ENC_GROUP, "groups");
}

void NodeSparkGroupList::process()
{
    _groups.clear();
    for(size_t i=0; i<_inputs.size(); i++)
    {
        std::shared_ptr<NodeDataSparkGroupList> in = getInput<NodeDataSparkGroupList>(i);
        if(in.get())
        {
            for(size_t j=0; j<in->_result.size(); j++)
                _groups.push_back(in->_result[j]);
        }
    }

    dataUpdated(0);
}


//--------------------------------------------------------------------------------------------
// group
//--------------------------------------------------------------------------------------------

NodeSparkGroup::NodeSparkGroup()
{
    IN_PORT(ENC_RENDERER, "renderer");
    IN_PORT(ENC_EMITTER, "emitters");
    IN_PORT(ENC_MODIFIER, "modifiers");
    OUT_PORT(ENC_GROUP, "group");

    createBaseObjectParams("Group");
    PARAM_INT("Capacity", 1, 500000, 100);
    PARAM_FXY("Lifetime", 0.000001, eF32_MAX, 1.0f, 1.0f);
    PARAM_BOOL("Immortal", false);
    PARAM_FLOAT("GraphicalRadius", 0, eF32_MAX, 1.0f);
    PARAM_FLOAT("PhysicalRadius", 0, eF32_MAX, 0.0f);
    PARAM_BOOL("Sorted", false);
}

void NodeSparkGroup::process()
{
    const unsigned int INPUT_RENDERER_INDEX = 0;
    const unsigned int INPUT_EMITTERS_INDEX = 1;
    const unsigned int INPUT_MODIFIERS_INDEX = 2;

    // get parameters
    eInt capacity = getParameter("Capacity")->getValueAsInt();
    eFXY lifetime = getParameter("Lifetime")->getValueAsFXY();
    bool immortal = getParameter("Immortal")->getValueAsBool();
    eF32 graphicalRadius = getParameter("GraphicalRadius")->getValueAsFloat();
    eF32 physicalRadius = getParameter("PhysicalRadius")->getValueAsFloat();
    bool sorted = getParameter("Sorted")->getValueAsBool();

    // create new group
    SPK::Ref<SPK::Group> group = SPK::Group::create(capacity);
    setBaseObjectParams(group);
    group->setLifeTime(lifetime.x, lifetime.y);
    group->setImmortal(immortal);
    group->setGraphicalRadius(graphicalRadius);
    group->setPhysicalRadius(physicalRadius);
    group->enableSorting(sorted);

    // set renderer
    std::shared_ptr<NodeDataSparkRenderer> inRenderer = getInput<NodeDataSparkRenderer>(INPUT_RENDERER_INDEX);
    if(inRenderer && inRenderer->_result.get())
    {
        group->setRenderer(inRenderer->_result);
    }
    else
    {
        setValidationState(NodeValidationState::Error, "Missing renderer input");
        return;
    }

    // add emitters
    std::shared_ptr<NodeDataSparkEmitterList> inEmitters = getInput<NodeDataSparkEmitterList>(INPUT_EMITTERS_INDEX);
    if(inEmitters)
    {
        for(size_t i=0; i<inEmitters->_result.size(); i++)
        {
            if(inEmitters->_result[i].get())
                group->addEmitter(inEmitters->_result[i]);
        }
    }

    // add modifiers
    std::shared_ptr<NodeDataSparkModifierList> inModifiers = getInput<NodeDataSparkModifierList>(INPUT_MODIFIERS_INDEX);
    if(inModifiers)
    {
        for(size_t i=0; i<inModifiers->_result.size(); i++)
        {
            if(inModifiers->_result[i].get())
                group->addModifier(inModifiers->_result[i]);
        }
    }




    // Zone
    SPK::Ref<SPK::Point> point = SPK::Point::create(SPK::Vector3D(0,0,0));

    // Obstacle
    SPK::Ref<SPK::Plane> groundPlane = SPK::Plane::create();
    SPK::Ref<SPK::Obstacle> obstacle = SPK::Obstacle::create(groundPlane,0.9f,1.0f);
    obstacle->setBouncingRatio(0.6f);
    obstacle->setFriction(1.0f);

    //_group->addEmitter(particleEmitter);
    //group->addModifier(obstacle);
    //group->setRenderer(renderer);
    //group->addModifier(SPK::Gravity::create(SPK::Vector3D(0,-5,0)));
    group->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0x445588ff,0x995577ff));


    // store result
    if(_groups.size() > 1)
        _groups.back().reset();
    _groups.clear();
    _groups.push_back(group);


    setValidationState(NodeValidationState::Valid);

    dataUpdated(0);
}



//--------------------------------------------------------------------------------------------
// system
//--------------------------------------------------------------------------------------------

NodeSparkSystem::NodeSparkSystem()
{
    IN_PORT(ENC_GROUP, "groups");

    PARAM_FLOAT("Myssytem", eF32_MIN, eF32_MAX, 0.0f);
}

void NodeSparkSystem::process()
{
   // if(!_system.get())
   //     _system = SPK::System::create(true);
   // else
   //     _system->removeAllGroups();
   //

    // dynamic add input test
    //_inputs.push_back(new ePort(PortType::In, NODE_DATA_TYPE[ENC_GROUP], QString("dsqdqs")));
   // Q_EMIT(interfaceChanged());


    _system = SPK::System::create(true);
    _system->setName("mySystem");

    // get group from input 0

   /*std::shared_ptr<NodeDataSparkGroup> in0 = getInput<NodeDataSparkGroup>(0);
    if(in0)
        _system->addGroup(in0->_result);*/

    std::shared_ptr<NodeDataSparkGroupList> in0 = getInput<NodeDataSparkGroupList>(0);
    if(in0)
    {
        for(size_t i=0; i<in0->_result.size(); i++)
        {
            if(in0->_result[i].get())
                _system->addGroup(in0->_result[i]);
        }
    }


    ///Q_EMIT(systemUpdated(_system));
    ///

    GPDevice::get().setCurentParticleSystem(_system);
}


//--------------------------------------------------------------------------------------------
// quad renderer node
//--------------------------------------------------------------------------------------------

NodeSparkQuadRenderer::NodeSparkQuadRenderer()
{
    OUT_PORT(ENC_RENDERER, "renderer");

    createBaseObjectParams("Renderer");
    PARAM_FILE("Material", "Materials/Particle.xml");
    PARAM_FILE("Texture", "Textures/Flare.dds");
    PARAM_IXY("AtlasDimension", 1, 1000, 1, 1);
    PARAM_FXY("Scale", 0.0f, eF32_MAX, 1.0f, 1.0f);
    PARAM_ENUM("Orientation", "CAMERA_PLANE_ALIGNED"
                              "|CAMERA_POINT_ALIGNED"
                              "|DIRECTION_ALIGNED"
                              "|AROUND_AXIS"
                              "|TOWARDS_POINT"
                              "|FIXED_ORIENTATION", 0);
}

void NodeSparkQuadRenderer::process()
{
    // get parameters
    std::string materialFile(getParameter("Material")->getValueAsString().toStdString());
    std::string textureFile(getParameter("Texture")->getValueAsString().toStdString());
    eIXY atlasDimensions = (getParameter("AtlasDimension")->getValueAsIXY());
    eFXY scale = getParameter("Scale")->getValueAsFXY();
    SPK::OrientationPreset orientation = SPK::OrientationPreset(getParameter("Orientation")->getValueAsEnum());


    // Create a material for particles
    Material* material = Material::create("res/core/shaders/particle.vert", "res/core/shaders/particle.frag");
    Texture::Sampler* sampler2 = material->getParameter("u_diffuseTexture")->setValue("res/data/textures/flare.png", true);
    sampler2->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
    material->getStateBlock()->setCullFace(true);
    material->getStateBlock()->setDepthTest(true);
    material->getStateBlock()->setDepthWrite(false);
    material->getStateBlock()->setBlend(true);
    material->getStateBlock()->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
    material->getStateBlock()->setBlendDst(RenderState::BLEND_ONE);


    // Renderer
    SPK::Ref<SPK::GP3D::SparkQuadRenderer> renderer = SPK::GP3D::SparkQuadRenderer::create();
    renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    renderer->setBlendMode(SPK::BLEND_MODE_ADD);
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
    renderer->setScale(scale.x, scale.y);
    renderer->setMaterial(material->clone());
    renderer->setAtlasDimensions(atlasDimensions.x, atlasDimensions.y);
    renderer->setOrientation(orientation);

    _renderer.reset();
    _renderer = renderer;

    dataUpdated(0);
}



//------------------------------------------------------------------------------------------------------------------------------
// test node
//------------------------------------------------------------------------------------------------------------------------------

#include <QMessageBox>
#include "CustomWidgets.h"

NodeSparkTest::NodeSparkTest()
{
    OUT_PORT(ENC_RENDERER, "renderer");

    PARAM_TEXT("text", "Bonjour");
    PARAM_STRING("string", "Hello");
    PARAM_BOOL("bool", false);
    PARAM_INT("int", -5, 5, 2);
    PARAM_FLOAT("float", -5.0f, 5.0f, 3.14f);
    PARAM_FXY("fxy", -5.0f, 5.0f, 1.0f, 2.0f);
    PARAM_FXYZ("fxyz", -5.0f, 5.0f, 1.0f, 2.0f, 3.0f);
    PARAM_FXYZW("fxyzw", -5.0f, 5.0f, 1.0f, 2.0f, 3.0f, 4.0f);
    PARAM_IXY("ixy", -5, 5, 1, 2);
    PARAM_IXYZ("ixyz", -5, 5, 1, 2, 3);
    PARAM_IXYZW("ixyzw", -5, 5, 1, 2, 3, 4);
    PARAM_ENUM("enum", "enum1|enum2|enum3|num4|enum5", 1);
    PARAM_FILE("file", "res/data/textures");
    PARAM_FLAGS("flags", "flag1|flag2|flag3|flag4|flag5", 2)
    PARAM_RGBA("rgba", 100, 50, 50, 255);
    PARAM_BUTTON("button", "Simple Button");



    // force parameters widget creation  immediately to get acess to pushbutton ptr
    createParamWidgets();

    // connect push button to a slot
    eButton* button = (eButton*)getParameter("button")->userData(0);
    Q_ASSERT(button);
    connect(button, &QPushButton::clicked, this, &NodeSparkTest::onTestButtonClick);
}

void NodeSparkTest::process()
{

}

void NodeSparkTest::onTestButtonClick()
{
    QMessageBox msgBox;
    msgBox.setText("The button has been clicked.");
    msgBox.exec();
}
