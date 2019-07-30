#include "shader.h"

ModelShader::ModelShader(Model *model_)
    : model(model_)
{
}

void SimpleModelShader::initMatrices()
{
    perspective = Matrix::identity();
    perspective[3][2] = -1.f/1.f;
    
    viewport = Matrix::viewport(viewportWidth, viewportHeight,0,0);

    Vec3f eye(.5f, 0.2f, 1.f);
    view = Matrix::camLookAt(Vec3f(0.f,1.f,0.f), Vec3f(0.f,0.f,0.f), eye);
}

SimpleModelShader::SimpleModelShader(Model *model_ , int viewportWidth_, int viewportHeight_, Vec3f lightDir_)
    :ModelShader(model_), viewportWidth(viewportWidth_), 
    viewportHeight(viewportHeight_),  lightDir(lightDir_)
{   
    lightDir.normalize();
    initMatrices();
}

Vec3f SimpleModelShader::vertexShader(int face, int vertIndex)
{
    vertCoords[vertIndex]= model->vert(model->face(face)[vertIndex]);
    Vec3f v = (viewport * perspective * view * Matrix::v2m(vertCoords[vertIndex])).toVec();
    Vec3f n = model->normal(face, vertIndex);
    intensity[vertIndex] = -std::min(0.f, lightDir * n);
    return Vec3f(int(v.x), int(v.y), int(v.z));
}

TGAColor SimpleModelShader::fragShader(Vec3f barCoords)
{   
    return white * (intensity * barCoords);
    return white;
}

TGAColor SimpleTextureModelShader::fragShader(Vec3f barCoords)
{   
    return (white*intensity[0]) * barCoords[0] +
    (white*intensity[1]) * barCoords[1] +
    (white*intensity[2]) * barCoords[2];
    
    return white;
}
