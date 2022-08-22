#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include "bitmap_image.hpp"

#define POS_INF_DBL std::numeric_limits<double>::infinity();
#define NEG_INF_DBL -std::numeric_limits<double>::infinity();

using namespace std;
using std::string;

class Point
{
public:
    double x, y, z, w;
    Point()
    {
        x = y = z = 0.0;
        w = 1.0;
    }
    Point(double x, double y, double z, double w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    void scale()
    {
        x /= w;
        y /= w;
        z /= w;
        w = 1.0;
    }
    void normalize()
    {
        double length = sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;
    }
};

class Color
{
public:
    int rv;
    int gv;
    int bv;
    Color()
    {
        rv = 0;
        gv = 0;
        bv = 0;
    }
    Color(int rv, int gv, int bv)
    {
        this->rv = rv;
        this->gv = gv;
        this->bv = bv;
    }
};

class Triangle
{
public:
    Point points[3];
    Color color;
    Triangle()
    {
        points[0] = Point(0.0, 0.0, 0.0, 1.0);
        points[1] = Point(0.0, 0.0, 0.0, 1.0);
        points[2] = Point(0.0, 0.0, 0.0, 1.0);
        color = Color();
    }
    Triangle(Point p1, Point p2, Point p3, Color c)
    {
        points[0] = p1;
        points[1] = p2;
        points[2] = p3;
        color = c;
    }
};

Point subPoint(Point a, Point b)
{
    Point c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

Point addPoint(Point a, Point b)
{
    Point c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

Point gradientProduct(Point a, double b)
{
    Point c;
    c.x = a.x * b;
    c.y = a.y * b;
    c.z = a.z * b;
    return c;
}

Point crossProduct(Point a, Point b)
{
    Point c;
    c.x = a.y * b.z - a.z * b.y;
    c.y = a.z * b.x - a.x * b.z;
    c.z = a.x * b.y - a.y * b.x;
    return c;
}

double dotProduct(Point a, Point b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

class Transformation
{
public:
    double matrix[4][4];
    Transformation()
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                matrix[i][j] = 0.0;
        matrix[0][0] = 1.0;
        matrix[1][1] = 1.0;
        matrix[2][2] = 1.0;
        matrix[3][3] = 1.0;
    }
};

Transformation translationMatrix(double tx, double ty, double tz)
{
    Transformation t;
    t.matrix[0][3] = tx;
    t.matrix[1][3] = ty;
    t.matrix[2][3] = tz;
    return t;
}

Transformation scalingMatrix(double sx, double sy, double sz)
{
    Transformation t;
    t.matrix[0][0] = sx;
    t.matrix[1][1] = sy;
    t.matrix[2][2] = sz;
    return t;
}

Point rodriguesFormula(Point x, double angle, Point a)
{
    Point rx(0.0, 0.0, 0.0, 0.0);
    rx = addPoint(rx, gradientProduct(x, cos(angle)));
    rx = addPoint(rx, gradientProduct(gradientProduct(a, dotProduct(a, x)), (1.0 - cos(angle))));
    rx = addPoint(rx, gradientProduct(crossProduct(a, x), sin(angle)));
    return rx;
}

Transformation rotationMatrix(double angle, double ax, double ay, double az)
{
    angle = angle * M_PI / 180.0;
    Transformation t;
    Point a(ax, ay, az, 1.0);
    a.normalize();
    Point c1(1.0, 0.0, 0.0, 1.0);
    Point c2(0.0, 1.0, 0.0, 1.0);
    Point c3(0.0, 0.0, 1.0, 1.0);
    c1 = rodriguesFormula(c1, angle, a);
    c2 = rodriguesFormula(c2, angle, a);
    c3 = rodriguesFormula(c3, angle, a);
    t.matrix[0][0] = c1.x;
    t.matrix[0][1] = c2.x;
    t.matrix[0][2] = c3.x;
    t.matrix[0][3] = 0.0;
    t.matrix[1][0] = c1.y;
    t.matrix[1][1] = c2.y;
    t.matrix[1][2] = c3.y;
    t.matrix[1][3] = 0.0;
    t.matrix[2][0] = c1.z;
    t.matrix[2][1] = c2.z;
    t.matrix[2][2] = c3.z;
    t.matrix[2][3] = 0.0;
    t.matrix[3][0] = 0.0;
    t.matrix[3][1] = 0.0;
    t.matrix[3][2] = 0.0;
    t.matrix[3][3] = 1.0;
    return t;
}

Transformation multiplyTransformation(Transformation a, Transformation b)
{
    Transformation c;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            c.matrix[i][j] = 0.0;
            for (int k = 0; k < 4; k++)
                c.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
        }
    }
    return c;
}

Point multiplyTransformationWithPoint(Transformation a, Point b)
{
    Point c(0.0, 0.0, 0.0, 0.0);
    c.x = a.matrix[0][0] * b.x + a.matrix[0][1] * b.y + a.matrix[0][2] * b.z + a.matrix[0][3] * b.w;
    c.y = a.matrix[1][0] * b.x + a.matrix[1][1] * b.y + a.matrix[1][2] * b.z + a.matrix[1][3] * b.w;
    c.z = a.matrix[2][0] * b.x + a.matrix[2][1] * b.y + a.matrix[2][2] * b.z + a.matrix[2][3] * b.w;
    c.w = a.matrix[3][0] * b.x + a.matrix[3][1] * b.y + a.matrix[3][2] * b.z + a.matrix[3][3] * b.w;
    return c;
}

Transformation viewMatrix(Point eye, Point look, Point up)
{
    Transformation ans, tAns;
    Point l = subPoint(look, eye);
    l.normalize();
    Point r = crossProduct(l, up);
    r.normalize();
    Point u = crossProduct(r, l);
    u.normalize();
    tAns = translationMatrix(-eye.x, -eye.y, -eye.z);
    ans.matrix[0][0] = r.x;
    ans.matrix[0][1] = r.y;
    ans.matrix[0][2] = r.z;
    ans.matrix[0][3] = 0.0;
    ans.matrix[1][0] = u.x;
    ans.matrix[1][1] = u.y;
    ans.matrix[1][2] = u.z;
    ans.matrix[1][3] = 0.0;
    ans.matrix[2][0] = -l.x;
    ans.matrix[2][1] = -l.y;
    ans.matrix[2][2] = -l.z;
    ans.matrix[2][3] = 0.0;
    ans.matrix[3][0] = 0.0;
    ans.matrix[3][1] = 0.0;
    ans.matrix[3][2] = 0.0;
    ans.matrix[3][3] = 1.0;
    ans = multiplyTransformation(ans, tAns);
    return ans;
}

Transformation projectionMatrix(double fovY, double aspectRatio, double near, double far)
{
    Transformation ans;
    double fovX = fovY * aspectRatio;
    double t = near * tan(M_PI * fovY / 360.0);
    double r = near * tan(M_PI * fovX / 360.0);
    ans.matrix[0][0] = near / r;
    ans.matrix[0][1] = 0.0;
    ans.matrix[0][2] = 0.0;
    ans.matrix[0][3] = 0.0;
    ans.matrix[1][0] = 0.0;
    ans.matrix[1][1] = near / t;
    ans.matrix[1][2] = 0.0;
    ans.matrix[1][3] = 0.0;
    ans.matrix[2][0] = 0.0;
    ans.matrix[2][1] = 0.0;
    ans.matrix[2][2] = -(far + near) / (far - near);
    ans.matrix[2][3] = -2.0 * far * near / (far - near);
    ans.matrix[3][0] = 0.0;
    ans.matrix[3][1] = 0.0;
    ans.matrix[3][2] = -1.0;
    ans.matrix[3][3] = 0.0;
    return ans;
}

void printMatrix(Transformation t)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            cout << t.matrix[i][j] << " ";
        cout << endl;
    }
    cout << endl;
}

int main(int argc, char **argv)
{
    ifstream input;
    ofstream output;
    string inputFile;
    if (argc == 0)
        inputFile = "./test_cases/4/";
    else
        inputFile = "./test_cases/" + std::string(argv[1]) + "/";

    string inp, outp;

    // stage 1
    inp = inputFile + "scene.txt";
    outp = inputFile + "stage1.txt";
    input.open(inp);
    output.open(outp);
    double eyeX, eyeY, eyeZ,
        lookX, lookY, lookZ,
        upX, upY, upZ,
        fovY, aspectRatio, near, far;
    input >> eyeX >> eyeY >> eyeZ >>
        lookX >> lookY >> lookZ >>
        upX >> upY >> upZ >>
        fovY >> aspectRatio >> near >> far;
    stack<Transformation> transformationStack;
    transformationStack.push(Transformation());
    string command;
    int triangleCount = 0;
    while (true)
    {
        input >> command;
        if (command == "triangle")
        {
            triangleCount++;
            Point a, b, c;
            input >> a.x >> a.y >> a.z >>
                b.x >> b.y >> b.z >>
                c.x >> c.y >> c.z;
            a = multiplyTransformationWithPoint(transformationStack.top(), a);
            a.scale();
            b = multiplyTransformationWithPoint(transformationStack.top(), b);
            b.scale();
            c = multiplyTransformationWithPoint(transformationStack.top(), c);
            c.scale();
            output << std::fixed << std::setprecision(7);
            output << a.x << " " << a.y << " " << a.z << endl
                   << b.x << " " << b.y << " " << b.z << endl
                   << c.x << " " << c.y << " " << c.z << endl
                   << endl;
        }
        else if (command == "translate")
        {
            double tx, ty, tz;
            input >> tx >> ty >> tz;
            Transformation t = multiplyTransformation(transformationStack.top(), translationMatrix(tx, ty, tz));
            transformationStack.pop();
            transformationStack.push(t);
        }
        else if (command == "scale")
        {
            double sx, sy, sz;
            input >> sx >> sy >> sz;
            Transformation t = multiplyTransformation(transformationStack.top(), scalingMatrix(sx, sy, sz));
            transformationStack.pop();
            transformationStack.push(t);
        }
        else if (command == "rotate")
        {
            double angle, x, y, z;
            input >> angle >> x >> y >> z;
            Transformation t = multiplyTransformation(transformationStack.top(), rotationMatrix(angle, x, y, z));
            transformationStack.pop();
            transformationStack.push(t);
        }
        else if (command == "push")
        {
            transformationStack.push(transformationStack.top());
        }
        else if (command == "pop")
        {
            transformationStack.pop();
        }
        else if (command == "end")
        {
            break;
        }
    }
    input.close();
    output.close();

    // stage 2
    inp = inputFile + "stage1.txt";
    outp = inputFile + "stage2.txt";
    input.open(inp);
    output.open(outp);
    for (int i = 0; i < triangleCount; i++)
    {
        Point p1, p2, p3;
        input >> p1.x >> p1.y >> p1.z >>
            p2.x >> p2.y >> p2.z >>
            p3.x >> p3.y >> p3.z;
        Transformation t = transformationStack.top();
        t = multiplyTransformation(t, viewMatrix(Point(eyeX, eyeY, eyeZ, 1.0), Point(lookX, lookY, lookZ, 1.0), Point(upX, upY, upZ, 1.0)));
        p1 = multiplyTransformationWithPoint(t, p1);
        p2 = multiplyTransformationWithPoint(t, p2);
        p3 = multiplyTransformationWithPoint(t, p3);
        p1.scale();
        p2.scale();
        p3.scale();
        output << std::fixed << std::setprecision(7);
        output << p1.x << " " << p1.y << " " << p1.z << endl
               << p2.x << " " << p2.y << " " << p2.z << endl
               << p3.x << " " << p3.y << " " << p3.z << endl
               << endl;
    }

    input.close();
    output.close();

    // stage 3
    inp = inputFile + "stage2.txt";
    outp = inputFile + "stage3.txt";
    input.open(inp);
    output.open(outp);
    for (int i = 0; i < triangleCount; i++)
    {
        Point p1, p2, p3;
        input >> p1.x >> p1.y >> p1.z >>
            p2.x >> p2.y >> p2.z >>
            p3.x >> p3.y >> p3.z;
        Transformation t = transformationStack.top();
        t = multiplyTransformation(t, projectionMatrix(fovY, aspectRatio, near, far));
        p1 = multiplyTransformationWithPoint(t, p1);
        p2 = multiplyTransformationWithPoint(t, p2);
        p3 = multiplyTransformationWithPoint(t, p3);
        p1.scale();
        p2.scale();
        p3.scale();
        output << std::fixed << std::setprecision(7);
        output << p1.x << " " << p1.y << " " << p1.z << endl
               << p2.x << " " << p2.y << " " << p2.z << endl
               << p3.x << " " << p3.y << " " << p3.z << endl
               << endl;
    }
    input.close();
    output.close();

    // stage 4
    inp = inputFile + "config.txt";
    outp = inputFile + "stage4.txt";
    input.open(inp);
    output.open(outp);

    int Screen_Width, Screen_Height;
    double Left_Limit_X, Right_Limit_X;
    double Bottom_Limit_Y, Top_Limit_Y;
    double Front_Limit_Z, Rear_Limit_Z;

    input >> Screen_Width >> Screen_Height;
    input >> Left_Limit_X >> Bottom_Limit_Y;
    input >> Front_Limit_Z >> Rear_Limit_Z;

    Right_Limit_X = -Left_Limit_X;
    Top_Limit_Y = -Bottom_Limit_Y;

    double dx = (Right_Limit_X - Left_Limit_X) / Screen_Width;
    double dy = (Top_Limit_Y - Bottom_Limit_Y) / Screen_Height;
    double Top_Y = Top_Limit_Y - dy / 2.0;
    double Left_X = Left_Limit_X + dx / 2.0;
    double Bottom_Y = Bottom_Limit_Y + dy / 2.0;
    double Right_X = Right_Limit_X - dx / 2.0;

    input.close();

    inp = inputFile + "stage3.txt";
    input.open(inp);

    Triangle triangles[triangleCount];
    srand(time(0));

    for (int i = 0; i < triangleCount; i++)
    {
        Point p1, p2, p3;
        input >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> p3.x >> p3.y >> p3.z;
        triangles[i].points[0].x = p1.x;
        triangles[i].points[0].y = p1.y;
        triangles[i].points[0].z = p1.z;
        triangles[i].points[0].scale();
        triangles[i].points[1].x = p2.x;
        triangles[i].points[1].y = p2.y;
        triangles[i].points[1].z = p2.z;
        triangles[i].points[1].scale();
        triangles[i].points[2].x = p3.x;
        triangles[i].points[2].y = p3.y;
        triangles[i].points[2].z = p3.z;
        triangles[i].points[2].scale();
        triangles[i].color.rv = rand() % 255;
        triangles[i].color.gv = rand() % 255;
        triangles[i].color.bv = rand() % 255;
    }
    input.close();

    double **Z_Buffer = new double *[Screen_Width];
    Color **intensity_Buffer = new Color *[Screen_Width];
    for (int i = 0; i < Screen_Width; i++)
    {
        Z_Buffer[i] = new double[Screen_Height];
        intensity_Buffer[i] = new Color[Screen_Height];
    }

    for (int i = 0; i < Screen_Width; i++)
    {
        for (int j = 0; j < Screen_Height; j++)
        {
            Z_Buffer[i][j] = Rear_Limit_Z;
            intensity_Buffer[i][j] = Color(0.0, 0.0, 0.0);
        }
    }

    for (Triangle tri : triangles)
    {
        int Top_Scanline, Bottom_Scanline;
        double maxY_From_Points = max(tri.points[0].y, max(tri.points[1].y, tri.points[2].y));
        double minY_From_Points = min(tri.points[0].y, min(tri.points[1].y, tri.points[2].y));

        if (maxY_From_Points >= Top_Y)
        {
            Top_Scanline = 0;
        }
        else
        {
            Top_Scanline = (int)round((Top_Y - maxY_From_Points) / dy);
        }

        if (minY_From_Points <= Bottom_Y)
        {
            Bottom_Scanline = Screen_Height - 1;
        }
        else
        {
            Bottom_Scanline = Screen_Height - 1 - (int)round((minY_From_Points - Bottom_Y) / dy);
        }

        double maxX_From_Points = max(tri.points[0].x, max(tri.points[1].x, tri.points[2].x));
        double minX_From_Points = min(tri.points[0].x, min(tri.points[1].x, tri.points[2].x));

        for (int row_no = Top_Scanline; row_no <= Bottom_Scanline; row_no++)
        {
            double ys = Top_Y - row_no * dy;

            double x_1_2 = tri.points[0].x + (tri.points[1].x - tri.points[0].x) * (ys - tri.points[0].y) / (tri.points[1].y - tri.points[0].y);
            double z_1_2 = tri.points[0].z + (tri.points[1].z - tri.points[0].z) * (ys - tri.points[0].y) / (tri.points[1].y - tri.points[0].y);
            double x_2_3 = tri.points[1].x + (tri.points[2].x - tri.points[1].x) * (ys - tri.points[1].y) / (tri.points[2].y - tri.points[1].y);
            double z_2_3 = tri.points[1].z + (tri.points[2].z - tri.points[1].z) * (ys - tri.points[1].y) / (tri.points[2].y - tri.points[1].y);
            double x_3_1 = tri.points[2].x + (tri.points[0].x - tri.points[2].x) * (ys - tri.points[2].y) / (tri.points[0].y - tri.points[2].y);
            double z_3_1 = tri.points[2].z + (tri.points[0].z - tri.points[2].z) * (ys - tri.points[2].y) / (tri.points[0].y - tri.points[2].y);

            double xa, xb, za, zb;
            int validPointCount = 0;
            bool valid_1_2 = false;
            bool valid_2_3 = false;
            bool valid_3_1 = false;
            if (!isinf(x_1_2) && x_1_2 >= minX_From_Points && x_1_2 <= maxX_From_Points)
            {
                validPointCount++;
                valid_1_2 = true;
            }
            if (!isinf(x_2_3) && x_2_3 >= minX_From_Points && x_2_3 <= maxX_From_Points)
            {
                    validPointCount++;
                    valid_2_3 = true;
            }
            if (!isinf(x_3_1) && x_3_1 >= minX_From_Points && x_3_1 <= maxX_From_Points)
            {
                    validPointCount++;
                    valid_3_1 = true;
            }
            if (validPointCount == 0)
                continue;
            else
            {
                if (valid_1_2)
                {
                    xa = x_1_2;
                    za = z_1_2;
                    xb = x_1_2;
                    zb = z_1_2;
                }
                if (valid_2_3)
                {
                    if (!valid_1_2)
                    {
                        xa = x_2_3;
                        za = z_2_3;
                        xb = x_2_3;
                        zb = z_2_3;
                    }
                    else
                    {
                        if (x_2_3 < xa)
                        {
                            xa = x_2_3;
                            za = z_2_3;
                        }
                        if (x_2_3 > xb)
                        {
                            xb = x_2_3;
                            zb = z_2_3;
                        }
                    }
                }
                if (valid_3_1)
                {
                    if (!valid_1_2 && !valid_2_3)
                    {
                        xa = x_3_1;
                        za = z_3_1;
                        xb = x_3_1;
                        zb = z_3_1;
                    }
                    else
                    {
                        if (x_3_1 < xa)
                        {
                            xa = x_3_1;
                            za = z_3_1;
                        }
                        if (x_3_1 > xb)
                        {
                            xb = x_3_1;
                            zb = z_3_1;
                        }
                    }
                }
            }

            double Left_Intersecting_Column, Right_Intersecting_Column;
            if (xa <= Left_X)
            {
                Left_Intersecting_Column = 0;
            }
            else
            {
                Left_Intersecting_Column = (int)round((xa - Left_X) / dx);
            }
            if (xb >= Right_X)
            {
                Right_Intersecting_Column = Screen_Width - 1;
            }
            else
            {
                Right_Intersecting_Column = Screen_Width - 1 - (int)round((Right_X - xb) / dx);
            }

            double Constant_Term = 0.0;
            if (xb != xa)
            {
                Constant_Term = (dx * (zb - za)) / (xb - xa);
            }

            double zp = za;

            for (int col_no = Left_Intersecting_Column; col_no <= Right_Intersecting_Column; col_no++)
            {
                double xp = Left_X + col_no * dx;
                if (zp >= Front_Limit_Z && zp < Z_Buffer[col_no][row_no] && zp <= Rear_Limit_Z)
                {
                    Z_Buffer[col_no][row_no] = zp;
                    intensity_Buffer[col_no][row_no].rv = tri.color.rv;
                    intensity_Buffer[col_no][row_no].gv = tri.color.gv;
                    intensity_Buffer[col_no][row_no].bv = tri.color.bv;
                }
                zp += Constant_Term;
            }
        }
    }

    for (int i = 0; i < Screen_Width; i++)
    {
        for (int j = 0; j < Screen_Height; j++)
        {
            if (Z_Buffer[i][j] < Rear_Limit_Z)
                output << Z_Buffer[i][j] << "\t";
        }
        output << endl;
    }
    output << endl;

    bitmap_image image_bitmap(Screen_Width, Screen_Height);

    for (int i = 0; i < Screen_Width; i++)
    {
        for (int j = 0; j < Screen_Height; j++)
        {
            image_bitmap.set_pixel(i, j, intensity_Buffer[i][j].rv, intensity_Buffer[i][j].gv, intensity_Buffer[i][j].bv);
        }
    }
    outp = inputFile + "output.bmp";
    image_bitmap.save_image(outp);

    for (int i = 0; i < Screen_Width; i++)
    {
        delete[] Z_Buffer[i];
        delete[] intensity_Buffer[i];
    }
    delete[] Z_Buffer;
    delete[] intensity_Buffer;
    return 0;
}