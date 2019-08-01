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

    eye = Vec3f(.5f, 0.2f, 1.f);
    view = Matrix::camLookAt(Vec3f(0.f,1.f,0.f), Vec3f(0.f,0.f,0.f), eye);

    M = (viewport * perspective * view);
    //M.print();
    MIT = (perspective * view).inverse().transpose();
    //MI.print();

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
    Vec3f v = (M * Matrix::v2m(vertCoords[vertIndex])).toVec();
    
    // Calculate transformed normal
    auto N = Matrix::v2m(model->normal(face, vertIndex));
    N[3][0] = 0.f;
    Vec3f n = (MIT * N).toVec().normalize();
    normal[vertIndex] = n;
    
    intensity[vertIndex] = -std::min(0.f, lightDir * n);
    uv[vertIndex] = model->uv(face, vertIndex);
    
    return Vec3f(int(v.x), int(v.y), int(v.z));
}

TGAColor SimpleModelShader::fragShader(Vec3f barCoords)
{   
    return white * (intensity * barCoords);
}

TGAColor SimpleTextureModelShader::fragShader(Vec3f barCoords)
{   
    return model->diffuse(uv[0] * barCoords[0] + uv[1] * barCoords[1] + uv[2] * barCoords[2]);
}

TextureModelShader::TextureModelShader(Model *model_ , int viewportWidth_, int viewportHeight_,
    Vec3f lightDir_)
    :SimpleModelShader(model_, viewportWidth_, viewportHeight_, lightDir_)
{   
    lightDir.normalize();
    initMatrices();
}

Vec3f TextureModelShader::vertexShader(int face, int vertIndex)
{
    auto out = SimpleModelShader::vertexShader(face, vertIndex);
    viewDir[vertIndex] = (eye - out).normalize();
    return out;
}

TGAColor TextureModelShader::fragShader(Vec3f barCoords)
{      
    const float difConstant = 1.0f;
    Vec2f interpolatedUv = uv[0] * barCoords[0] + uv[1] * barCoords[1] + uv[2] * barCoords[2];

    Vec3f N = normal[0] * barCoords[0] + normal[1] * barCoords[1] + normal[2] * barCoords[2]; 
    //Vec3f N = model->normal(interpolatedUv);
    // lightDir is the direction the light is coming from so invert to get the opposite vector 
    Vec3f reflectDir =   N * -2 * (lightDir * N) + lightDir;
    TGAColor col = model->diffuse(interpolatedUv);
    
    Vec3f V = viewDir[0] * barCoords[0] + viewDir[1] * barCoords[1] + viewDir[2] * barCoords[2]; 
    float specular = std::pow(std::max(0.f, reflectDir * V), model->specular(interpolatedUv));
    //std::cout << specular  << " "; 
    float diffuse = intensity * barCoords * difConstant;
    for(int i = 0; i < 3; i ++)
        col[i] = std::min<int>(255, col[i] * (diffuse + specular));

    return col;
}
