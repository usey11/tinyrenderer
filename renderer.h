#include "tgaimage.h"
#include "geometry.h"
#include <vector>
#include "model.h"

class Renderer
{
    public:
    Renderer(TGAImage &image_);
    Renderer(TGAImage &image_, Model* model_);
    
    void drawTriangle(Vec2i* pts, TGAColor color);
    void drawTriangle(Vec3f* pts, TGAColor color);
    void drawTriangle(Vec3f* pts, Vec2f* uvs);

    void drawModel();
    
    private:
    std::vector<std::vector<float>> zBuf;
    TGAImage &image;
    Model* model;

    //float *zbuffer;
};
void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void drawLine(Vec2i v0, Vec2i v1, TGAImage &image, TGAColor color);

void drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage &image, TGAColor color);
void drawTriangle(Vec2i* pts, TGAImage &image, TGAColor color);

Vec3f barycentric(Vec2i *pts, Vec2i P);
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P); 
Vec3f cross(Vec3f A, Vec3f B);

