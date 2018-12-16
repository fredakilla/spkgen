#include <Spark/SPARK.h>
#include <Urhox/Spark/SPK_Urho3D_QuadRenderer.h>

#include "SpkSystem.h"
#include "../../UrhoDevice.h"
#include "../common/CustomWidgets.h"

#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Texture2D.h>


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
    IN_PORT(ENC_COLORINTERPOLATOR, "colors");
    IN_PORT(ENC_PARAMINTERPOLATOR, "params");    
    OUT_PORT(ENC_GROUP, "group");

    //createBaseObjectParams("Group");
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
    const unsigned int INPUT_COLORINTERPOLATOR_INDEX = 3;
    const unsigned int INPUT_PARAMSINTERPOLATOR_INDEX = 4;

    // get parameters
    eInt capacity = getParameter("Capacity")->getValueAsInt();
    eFXY lifetime = getParameter("Lifetime")->getValueAsFXY();
    bool immortal = getParameter("Immortal")->getValueAsBool();
    eF32 graphicalRadius = getParameter("GraphicalRadius")->getValueAsFloat();
    eF32 physicalRadius = getParameter("PhysicalRadius")->getValueAsFloat();
    bool sorted = getParameter("Sorted")->getValueAsBool();

    // create new group
    SPK::Ref<SPK::Group> group = SPK::Group::create(capacity);
    //setBaseObjectParams(group);
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

    // set color interpolator
    std::shared_ptr<NodeDataSparkColorInterpolator> inColors = getInput<NodeDataSparkColorInterpolator>(INPUT_COLORINTERPOLATOR_INDEX);
    if(inColors)
    {
        group->setColorInterpolator(inColors->_result);
    }

    // set param interpolators
    std::shared_ptr<NodeDataSparkParamInterpolatorList> inParams = getInput<NodeDataSparkParamInterpolatorList>(INPUT_PARAMSINTERPOLATOR_INDEX);
    if(inParams && inParams.get())
    {
        for(size_t i=0; i<inParams->_result.size(); i++)
        {
            if(inParams->_result[i].interpolatorFloat)
                group->setParamInterpolator(inParams->_result[i].param, inParams->_result[i].interpolatorFloat);
        }
    }

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

    PARAM_STRING("Name", "System");
    PARAM_BOOL("Initialized", eTRUE);
    PARAM_FILESAVE("Export", "SparkSystem.xml");
    PARAM_BUTTON("Action", "Run export");

    // force parameters widget creation  immediately to get acess to pushbutton ptr
    createParamWidgets();

    // connect push button to a slot
    eButton* button = (eButton*)getParameter("Action")->userData(0);
    Q_ASSERT(button);
    connect(button, &QPushButton::clicked, this, &NodeSparkSystem::onExportButtonClick);
}

void NodeSparkSystem::onExportButtonClick()
{
    if (_system)
    {
        QString filename = getParameter("Export")->getValueAsString();

        QFileInfo fileInfo(filename);
        QString extension = fileInfo.suffix();
        if (extension == "spk" || extension == "xml")
        {
            SPK::IO::IOManager::get().save(filename.toStdString(), _system);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Error");
            msgBox.setText("Unsupported file type (use .spk or .xml)");
            msgBox.exec();
        }
    }
}

void NodeSparkSystem::process()
{
    QString name = getParameter("Name")->getValueAsString();
    bool initialized = getParameter("Initialized")->getValueAsBool();

    SPK::Ref<SPK::System> system = SPK::System::create(initialized);
    system->setName(name.toStdString());

    // add groups
    std::shared_ptr<NodeDataSparkGroupList> in0 = getInput<NodeDataSparkGroupList>(0);
    if(in0)
    {
        for(size_t i=0; i<in0->_result.size(); i++)
        {
            if(in0->_result[i].get())
                system->addGroup(in0->_result[i]);
        }
    }

    _system.reset();
    _system = system;

    UrhoDevice::getInstance()->setCurentParticleSystem(_system);
}

void NodeSparkSystem::onParameterChanged()
{
    // custom onParameterChanged()
    // process without emit dataUpdated
    // to avoid signals infinite loop

    process();
}


//--------------------------------------------------------------------------------------------
// quad renderer node
//--------------------------------------------------------------------------------------------

NodeSparkQuadRenderer::NodeSparkQuadRenderer()
{
    OUT_PORT(ENC_RENDERER, "renderer");

    createBaseObjectParams("Renderer");
    PARAM_FILE("Material", "Data/Materials/Particle.xml");
    PARAM_FILE("Texture", "Data/Textures/Flare.dds");
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


    Urho3D::FileSystem* fileSystem = UrhoDevice::gUrhoContext->GetSubsystem<Urho3D::FileSystem>();
    bool isMaterialFileExists = fileSystem->FileExists(materialFile.c_str());
    bool isTextureFileExists = fileSystem->FileExists(textureFile.c_str());

    if (!isMaterialFileExists)
    {
        setValidationState(NodeValidationState::Error, "Material file does not exists");
        return;
    }

    if (!isTextureFileExists)
    {
        setValidationState(NodeValidationState::Error, "Texture file does not exists");
        return;
    }

    // Load base material
    Urho3D::ResourceCache* cache = UrhoDevice::gUrhoContext->GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::Material* baseMaterial = cache->GetResource<Urho3D::Material>(materialFile.c_str());

    // Create material clones and set textures
    Urho3D::SharedPtr<Urho3D::Material> material = baseMaterial->Clone();
    material->SetTexture(Urho3D::TU_DIFFUSE, cache->GetResource<Urho3D::Texture2D>(textureFile.c_str()));

    // Renderer
    SPK::Ref<SPK::URHO::Urho3DQuadRenderer> renderer = SPK::URHO::Urho3DQuadRenderer::create(UrhoDevice::gUrhoContext);
    renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    renderer->setBlendMode(SPK::BLEND_MODE_ADD);
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
    renderer->setScale(scale.x, scale.y);
    renderer->setMaterial(material);
    renderer->setAtlasDimensions(atlasDimensions.x, atlasDimensions.y);
    renderer->setOrientation(orientation);

    _renderer.reset();
    _renderer = renderer;

    dataUpdated(0);

    setValidationState(NodeValidationState::Valid);
}



//------------------------------------------------------------------------------------------------------------------------------
// test node
//------------------------------------------------------------------------------------------------------------------------------

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
    PARAM_FILESAVE("file", "res/data/textures");
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
