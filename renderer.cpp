#include "renderer.h"
#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor blue  = TGAColor(0  , 0, 255,   255);
const TGAColor green = TGAColor(0  , 255, 0,   255);

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    
    if (abs(y1-y0) > abs(x1-x0))    
    {
        steep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    
    if (x0>x1)
    {
        std::swap(x1,x0);
        std::swap(y1,y0);
    }

    int dx = x1-x0;
    int dy = y1-y0;

    int yinc = (y1>y0) ? 1 : -1;
    dy *= yinc;

    int err = 2*dy - dx;

    int y = y0;
    for (int x = x0; x <= x1;x++)
    {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        err += 2*dy;
        if (err > dx)
        {
            y += yinc;
            err -= 2*dx;
        }

    }
}


void drawLine(Vec2i v0, Vec2i v1, TGAImage &image, TGAColor color)
{
    drawLine(v0.x, v0.y, v1.x, v1.y, image, color);
}


Vec3f cross(Vec3f A, Vec3f B)
{
    return Vec3f(A.y*B.z-A.z*B.y, B.x*A.z-A.x*B.z, A.x*B.y-A.y*B.x);
}

Vec3f barycentric(Vec2i *pts, Vec2i P)
{
    Vec3f u = cross(Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x-P.x), 
                    Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.y)<1) return Vec3f(-1,1,1);
    
    return Vec3f(1.f - (u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    // Calculate the two vectors;
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void drawTriangle(Vec2i* pts, TGAImage &image, TGAColor color)
{
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            (j == 0 ? bboxmin.x :bboxmin.y) = std::max(0,        std::min(bboxmin[j], pts[i][j]));
            (j == 0 ? bboxmax.x :bboxmax.y) = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    Vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f b = barycentric(pts, P);
            if (b.x < 0 || b.y < 0 || b.z < 0)
                continue;
            image.set(P.x, P.y, color);
        }
    }
}

Renderer::Renderer(TGAImage &image_)
    :image(image_)
{
    int w = image.get_width();
    int h = image.get_height();
    zBuf.resize(h);
    for (int i = 0;i < h; i++)
    {
        zBuf[i].resize(w);
        for (int j = 0 ; j < w; j++)
        {
            zBuf[i][j] = -std::numeric_limits<float>::max();
        }
    }
}

Renderer::Renderer(TGAImage &image_, Model* model_)
    :image(image_), model(model_)
{
    int w = image.get_width();
    int h = image.get_height();
    zBuf.resize(h);
    for (int i = 0;i < h; i++)
    {
        zBuf[i].resize(w);
        for (int j = 0 ; j < w; j++)
        {
            zBuf[i][j] = -std::numeric_limits<float>::max();
        }
    }
}

void Renderer::drawTriangle(Vec2i* pts, TGAColor color)
{
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            (j == 0 ? bboxmin.x :bboxmin.y) = std::max(0,        std::min(bboxmin[j], pts[i][j]));
            (j == 0 ? bboxmax.x :bboxmax.y) = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    Vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f b = barycentric(pts, P);
            if (b.x < 0 || b.y < 0 || b.z < 0)
                continue;
            image.set(P.x, P.y, color);
        }
    }
}

void Renderer::drawTriangle(Vec3f* pts, TGAColor color)
{
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            (j == 0 ? bboxmin.x :bboxmin.y) = std::max(0.f,        std::min(bboxmin[j], pts[i][j]));
            (j == 0 ? bboxmax.x :bboxmax.y) = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
            
            
            if (zBuf[int(P.x)][int(P.y)] < P.z) {
                zBuf[int(P.x)][int(P.y)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

void Renderer::drawTriangle(Vec3f* pts, Vec2f* uvs)
{
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            (j == 0 ? bboxmin.x :bboxmin.y) = std::max(0.f,        std::min(bboxmin[j], pts[i][j]));
            (j == 0 ? bboxmax.x :bboxmax.y) = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    Vec3f P;
    Vec2f dtp;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen[0] <0 || bc_screen[1] <0 || bc_screen[2]<0) continue;
            P.z = 0;
            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
            
            dtp = uvs[0] * bc_screen[0] + uvs[1] * bc_screen[1] + uvs[2] * bc_screen[2];

            if (zBuf[int(P.x)][int(P.y)] < P.z) {
                zBuf[int(P.x)][int(P.y)] = P.z;
                image.set(P.x, P.y, model->diffuse(dtp));
            }
        }
    }
}

void Renderer::drawModel()
{
    const int width = image.get_width();
    const int height = image.get_height();

    for (int i=0; i<model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        Vec2f uvs[3];


        for (int j=0; j<3; j++) {
            // Spatial
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
            world_coords[j] = v;

            // Diffuse
            uvs[j] = model->uv(i, j);
        }

        Vec3f normal = cross(world_coords[2]-world_coords[0], world_coords[1]-world_coords[0]);
        normal.normalize();
            drawTriangle(screen_coords, uvs);
    }
}