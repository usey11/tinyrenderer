#include "shader.h"
#include <vector>
template<class T>
T interpolate(T v[], Vec3f barCoords) {
    return v[0] * barCoords[0] + v[1] * barCoords[1] + v[2] * barCoords[2];
}


ModelShader::ModelShader(Model *model_)
    : model(model_)
{
}

void SimpleModelShader::initMatrices()
{
    // Creat the perspective Matrix which is
    // [1 0 0 0]
    // [0 1 0 0]
    // [0 0 1 0]
    // [0 0 D 1]
    // D = -1/d
    // d = distance from origin
    perspective = Matrix::identity();
    perspective[3][2] = -1.f/1.f;
    
    viewport = Matrix::viewport(800, 800, 0, 0);

    eye = Vec3f(.5f, 0.2f, 1.f);
    view = Matrix::camLookAt(Vec3f(0.f,1.f,0.f), Vec3f(0.f,0.f,0.f), eye);

    M = (perspective * view);
    //M.print();
    MIT = (perspective * view).inverse().transpose();
    //MI.print();

}

SimpleModelShader::SimpleModelShader(Model *model_ , Vec3f lightDir_)
    :ModelShader(model_), lightDir(lightDir_)
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
    normals[vertIndex] = n;
    
    intensity[vertIndex] = -std::min(0.f, lightDir * n);
    uvs[vertIndex] = model->uv(face, vertIndex);
    
    return Vec3f(v.x, v.y, v.z);
}

TGAColor SimpleModelShader::fragShader(Vec3f barCoords)
{   
    return white * (intensity * barCoords);
}

TGAColor SimpleTextureModelShader::fragShader(Vec3f barCoords)
{   
    return model->diffuse(uvs[0] * barCoords[0] + uvs[1] * barCoords[1] + uvs[2] * barCoords[2]);
}

TextureModelShader::TextureModelShader(Model *model_, Vec3f lightDir_)
    :SimpleModelShader(model_, lightDir_)
{   
    lightDir.normalize();
    initMatrices();
}

Vec3f TextureModelShader::vertexShader(int face, int vertIndex)
{
    auto out = SimpleModelShader::vertexShader(face, vertIndex);
    viewDir[vertIndex] = (eye - out).normalize();
    worldCoords[vertIndex] = out;

    return out;
}

TGAColor TextureModelShader::fragShader(Vec3f barCoords)
{   
    const float difConstant = 1.0f;
    Vec2f interpolatedUv = interpolate(uvs, barCoords);

    // Even though direction vector and not position don't need to do N[3] = 0 as were normalizing it
    Vec3f n = (MIT * Matrix::v2m(model->normal(interpolatedUv))).toVec().normalize();

    // Calculate Darbboux basis
    Matrix matA  = Matrix::identity(3);
    auto p1p0 = (worldCoords[1] - worldCoords[0]).raw;
    auto p2p0 = (worldCoords[2] - worldCoords[0]).raw;
    matA[0] = std::vector<float>(p1p0, p1p0 + 3);
    matA[1] = std::vector<float>(p2p0, p2p0 + 3);
    matA[3] = std::vector<float>(n.raw, n.raw + 3);
    Matrix matAI = matA.inverse();

    Vec3f i = matAI * Vec3f(uvs[0][1] - uvs[0][0], uvs[0][2] - uvs[0][0], 0);
    // lightDir is the direction the light is coming from so invert to get the opposite vector 
    // TODO ^^^^ change this, maybe?
    Vec3f reflectDir =   n * -2 * (lightDir * n) + lightDir;
    
    // Get the diffuse colour from the texture map
    TGAColor col = model->diffuse(interpolatedUv);
    
    // Direction of the cam and calculate diffuse + specular coefficiants
    Vec3f V = interpolate(viewDir, barCoords);
    float specular =  std::pow(std::max(0.f, reflectDir * V), model->specular(interpolatedUv));
    float diffuse = intensity * barCoords * difConstant;

    for(int i = 0; i < 3; i ++)
        col[i] = std::min<int>(255, col[i] * (diffuse + specular));

    return col;
}
