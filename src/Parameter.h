#ifndef PARAMETER_H
#define PARAMETER_H

#include "Types.h"
#include "core/Variant.h"
#include <nodes/NodeData>
#include <QObject>
#include "Color.h"

using QtNodes::NodeDataType;

class BaseNode;

enum ParamType
{
    EPT_NONE        = 0,
    EPT_STRING      = 1,
    EPT_BOOL        = 2,
    EPT_FLAGS       = 3,
    EPT_ENUM        = 4,
    EPT_FLOAT       = 5,
    EPT_FXY         = 6,
    EPT_FXYZ        = 7,
    EPT_FXYZW       = 8,
    EPT_INT         = 9,
    EPT_IXY         = 10,
    EPT_IXYZ        = 11,
    EPT_IXYZW       = 12,
    EPT_FILE        = 13,
    EPT_TEXT        = 14,
    EPT_RGB         = 15,
    EPT_RGBA        = 16,
    EPT_BUTTON      = 17
};
/*
static const NodeDataType PARAM_TYPE[] =
{
    // data types
    { "none"    , "none"        },  // 0
    { "string"  , "string"      },  // 1
    { "bool"    , "bool"        },  // 2
    { "flags"   , "flags"       },  // 3
    { "enum"    , "enum"        },  // 4
    { "float"   , "float"       },  // 5
    { "fxy"     , "fxy"         },  // 6
    { "fxyz"    , "fxyz"        },  // 7
    { "fxyzw"   , "fxyzw"       },  // 8
    { "int"     , "int"         },  // 9
    { "ixy"     , "ixy"         },  // 10
    { "ixyz"    , "ixyz"        },  // 11
    { "ixyzw"   , "ixyzw"       },  // 12
    { "file"    , "file"        },  // 13
    { "text"    , "text"        },  // 14
    { "rgb"     , "rgb"         },  // 15
    { "rgba"    , "rgba"        },  // 16
};
*/


// possible variant type for parameters
typedef nonstd::variant
<   eF32,
    eFXY,
    eFXYZ,
    eFXYZW,
    eInt,
    eIXY,
    eIXYZ,
    eIXYZW,
    QString,
    bool,
    unsigned char,
    eColor
> eParamValue;


class Parameter : public QObject
{
    Q_OBJECT
public:
    ParamType type;
    QString name;
    float min;
    float max;
    BaseNode* node;
    bool isConnected;
    eParamValue baseValue;
    const eParamValue defVal;
    eString m_descr; // used for enum and flags texts

    Parameter();
    Parameter(ParamType type, QString name, float min, float max, BaseNode* node);

    unsigned int getComponentCount();
    void setDescription(const QString &descr);
    const eString & getDescription() const;
    ParamType getType();
    eF32 getMin() const;
    eF32 getMax() const;
    BaseNode * getOwnerOp() const;
    void setChanged(eBool reconnect = eFALSE);
    const eParamValue & getDefaultValue() const;
    eParamValue & getBaseValue();
    const eParamValue & getBaseValue() const;

    bool getValueAsBool() const;
    int getValueAsEnum() const;
    unsigned char getValueAsFlags() const;
    QString getValueAsString() const;

    eF32 getValueAsFloat() const;
    eFXY getValueAsFXY() const;
    eFXYZ getValueAsFXYZ() const;
    eFXYZW getValueAsFXYZW() const;

    eInt getValueAsInt() const;
    eIXY getValueAsIXY() const;
    eIXYZ getValueAsIXYZ() const;
    eIXYZW getValueAsIXYZW() const;

    eColor getValueAsColor() const;

Q_SIGNALS:
    void parameterUpdated();
};

#endif // PARAMETER_H
