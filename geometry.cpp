#include "geometry.h"


Vec3f cross(Vec3f A, Vec3f B)
{
    return Vec3f(A.y*B.z-A.z*B.y, B.x*A.z-A.x*B.z, A.x*B.y-A.y*B.x);
}

Matrix::Matrix(int r, int c)
    :m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f)))
{
    rows = r;
    cols = c;
}

Matrix Matrix::v2m(Vec3f v)
{
    Matrix mat(4,1);

    mat[0][0] = v[0];
    mat[1][0] = v[1];
    mat[2][0] = v[2];
    mat[3][0] = 1.0f;

    return mat;
}

Matrix v2m(Vec3f v)
{
    Matrix mat(4,1);

    mat[0][0] = v[0];
    mat[1][0] = v[1];
    mat[2][0] = v[2];
    mat[3][0] = 1.0f;

    return mat;
}

Vec3f Matrix::toVec()
{
    if(rows != 4 || cols != 1)
        throw "Not 4x1 matrix";
    
    Vec3f v(m[0][0], m[1][0], m[2][0]);
    return v*(1.f/m[3][0]);
}

Matrix Matrix::operator*(const Matrix &mat)
{
    if (cols != mat.rows)
        throw "Invalid Matrix multiplication";

    Matrix result = Matrix(rows, mat.cols);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            result[i][j] = 0.f;
            for (int k = 0; k < cols; k++)
                result[i][j] += m[i][k] * mat.m[k][j];
        }
    }

    return result;
}

Matrix Matrix::identity(int dimensions)
{
    Matrix mat(dimensions, dimensions);
    for (int i = 0 ; i < dimensions; i++)
    {
        mat[i][i] = 1.0f;
    }
    return mat;
}

Matrix Matrix::transpose()
{
    Matrix result(cols, rows);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[j][i] = m[i][j];
    return result;
}

Matrix Matrix::inverse()
{
    if (rows != cols)
        throw "Not Square Matrix";

    // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
    Matrix result(rows, cols*2);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[i][j] = m[i][j];
    for(int i=0; i<rows; i++)
        result[i][i+cols] = 1;
    // first pass
    for (int i=0; i<rows-1; i++) {
        // normalize the first row
        for(int j=result.cols-1; j>=0; j--)
            result[i][j] /= result[i][i];
        for (int k=i+1; k<rows; k++) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // normalize the last row
    for(int j=result.cols-1; j>=rows-1; j--)
        result[rows-1][j] /= result[rows-1][rows-1];
    // second pass
    for (int i=rows-1; i>0; i--) {
        for (int k=i-1; k>=0; k--) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            truncate[i][j] = result[i][j+cols];
    return truncate;
}

Matrix Matrix::camLookAt(Vec3f up, Vec3f target, Vec3f eye)
{
    Vec3f zaxis = (eye-target).normalize();
    Vec3f xaxis = cross(up, zaxis).normalize();
    Vec3f yaxis = cross(zaxis, xaxis).normalize();

    /*  View Matrix is the Inverse of Orientation and Traslation matrices
     *  V = (T x O) ^ -1
     *  V = O^-1 x T^-1
     *  O is orthogonal so O^-1 = O^T
     *  T is negated which is equivelant to inverse of translation matrix
     * 
     */
    
    Matrix Oinv = Matrix::identity(4);
    Matrix Tr   = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        Oinv[0][i] = xaxis[i];
        Oinv[1][i] = yaxis[i];
        Oinv[2][i] = zaxis[i];
        Tr[i][3] = -eye[i];
    }
    Matrix r = (Oinv * Tr);
    return r;
}

Matrix Matrix::viewport(int width, int height, int x, int y)
{
    Matrix mat = Matrix::identity(4);

    mat[0][0] = width/2.f;
    mat[1][1] = height/2.f;
    mat[2][2] = 100/2.f;

    mat[0][3] = x + width/2.f;
    mat[1][3] = y + height/2.f;
    mat[2][3] = 255/2.f;

    return mat;
}