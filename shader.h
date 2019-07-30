#include "tgaimage.h"
#include "geometry.h"
#include <vector>
#include "model.h"

class ModelShader {
public:
    ModelShader(Model *model_);
    virtual Vec3f vertexShader(int face, int vertIndex) = 0;
    virtual TGAColor fragShader(Vec3f barCoords) = 0;

protected:
    Model *model;
};

class SimpleModelShader : public ModelShader
{
public:
    SimpleModelShader(Model *model_, int viewportWidth_, int viewportHeight_, Vec3f lightDir_ = Vec3f(0.f, -1.f, 0.f));
    virtual Vec3f vertexShader(int face, int vertIndex) override;
    virtual TGAColor fragShader(Vec3f barCoords) override;

protected:
    // Passed between shader (varying)
    Vec3f vertCoords[3];
    Vec3f intensity;
    int viewportWidth;
    int viewportHeight;

    Vec3f lightDir;
    
    Matrix perspective;
    Matrix viewport;
    Matrix view;

    void initMatrices();
};

class SimpleTextureModelShader : public SimpleModelShader
{
public:
    using SimpleModelShader::SimpleModelShader;
    virtual TGAColor fragShader(Vec3f barCoords) override;
};