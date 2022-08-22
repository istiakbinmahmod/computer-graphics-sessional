#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <GL/glut.h>

using namespace std;

#define pi (2 * acos(0.0))
#define inf 1e9
#define eps .000001

class Object;
class Floor;
class Light;
extern vector<Object *> objects;
extern vector<Floor *> floors;
extern vector<Light *> lights;
extern int recursion_level;

class Vector3D
{
public:
    double x, y, z;
    Vector3D() {}
    Vector3D(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vector3D operator+(const Vector3D &v)
    {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }
    Vector3D operator-(const Vector3D &v)
    {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }
    Vector3D operator*(double s)
    {
        return Vector3D(x * s, y * s, z * s);
    }
    Vector3D operator/(double s)
    {
        return Vector3D(x / s, y / s, z / s);
    }
    double operator*(const Vector3D &v)
    {
        return x * v.x + y * v.y + z * v.z;
    }
    Vector3D operator^(const Vector3D &v)
    {
        return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }
    void normalize()
    {
        double l = length();
        x /= l;
        y /= l;
        z /= l;
    }
    Vector3D _normalize()
    {
        return *this / length();
    }
    void print()
    {
        printf("%lf %lf %lf", x, y, z);
    }
};

class Ray
{
public:
    Vector3D start;
    Vector3D dir;
    Ray() {}
    Ray(Vector3D start, Vector3D dir)
    {
        this->start = start;
        this->dir = dir;
        this->dir.normalize();
    }
};

class Color
{
public:
    double r, g, b;
    Color()
    {
        r = g = b = 0.0;
    }
    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Color operator+(const Color &c)
    {
        return Color(r + c.r, g + c.g, b + c.b);
    }
    Color operator*(double s)
    {
        return Color(r * s, g * s, b * s);
    }
    Color operator*(const Color &c)
    {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator/(double s)
    {
        return Color(r / s, g / s, b / s);
    }
    Color operator=(const Color c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        return *this;
    }
    void clip()
    {
        if (r < 0)
            r = 0;
        if (r > 1)
            r = 1;
        if (g < 0)
            g = 0;
        if (g > 1)
            g = 1;
        if (b < 0)
            b = 0;
        if (b > 1)
            b = 1;
    }
    void print()
    {
        printf("%lf %lf %lf", r, g, b);
    }
};

class Light
{
public:
    Vector3D light_pos;
    Color color;
    Light() {}
    Light(Vector3D light_pos, Color color)
    {
        this->light_pos = light_pos;
        this->color = color;
    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(light_pos.x, light_pos.y, light_pos.z);
        glColor3f(color.r, color.g, color.b);
        glutSolidSphere(0.1, 10, 10);
        glPopMatrix();
    }
};

class Object
{
public:
    Vector3D reference_point;
    double height, width, length;
    Color color;
    double ambient, diffuse, specular, reflection;
    int shine;
    Object() {}
    virtual void draw() {}
    void setColor(Color c)
    {
        color = c;
    }
    void setShine(int s)
    {
        shine = s;
    }
    void setCoEfficients(double a, double d, double s, double r)
    {
        ambient = a;
        diffuse = d;
        specular = s;
        reflection = r;
    }
    virtual Color getColorAt(Vector3D &point)
    {
        return color;
    }
    virtual Vector3D getNormalAt(const Vector3D &point)
    {
        return Vector3D(0, 0, 0);
    }
    virtual double intersect(Ray &ray, Color &color, int level) = 0;
    double intersectWithPhong(Ray &ray, Color &color, int level)
    {
        double tmin = this->intersect(ray, color, level);
        if (level == 0)
            return tmin;
        Vector3D intersectionPoint = ray.start + ray.dir * tmin;
        Color intersectingPointColor = getColorAt(intersectionPoint);
        color = intersectingPointColor * ambient;
        color.clip();
        Vector3D normal = getNormalAt(intersectionPoint);
        normal.normalize();
        for (int i = 0; i < lights.size(); i++)
        {
            Vector3D lightDirection = lights[i]->light_pos - intersectionPoint;
            lightDirection.normalize();
            double t_new = inf;
            Ray shadowRay(intersectionPoint - lightDirection * eps, lightDirection);
            bool shadow = false;
            for (int j = 0; j < objects.size(); j++)
            {
                Color c;
                double t = objects[j]->intersectWithPhong(shadowRay, c, 0);
                if (t > 0 && t < t_new)
                {
                    t_new = t;
                }
            }
            if (tmin > t_new)
                shadow = true;
            if (!shadow)
            {
                double dotProduct = normal * lightDirection;
                if (dotProduct > 0)
                {
                    color.r += intersectingPointColor.r * lights[i]->color.r * diffuse * dotProduct;
                    color.g += intersectingPointColor.g * lights[i]->color.g * diffuse * dotProduct;
                    color.b += intersectingPointColor.b * lights[i]->color.b * diffuse * dotProduct;
                    color.clip();
                }
                Vector3D reflectionDirection = ((normal * (normal * lightDirection) * 2.0)) - lightDirection;
                reflectionDirection.normalize();
                dotProduct = reflectionDirection * ray.dir;
                if (dotProduct > 0)
                {
                    color.r += intersectingPointColor.r * specular * pow(dotProduct, shine);
                    color.g += intersectingPointColor.g * specular * pow(dotProduct, shine);
                    color.b += intersectingPointColor.b * specular * pow(dotProduct, shine);
                    color.clip();
                }
            }
        }
        if (level >= recursion_level)
            return tmin;
        return tmin;
    }
};

class Sphere : public Object
{
public:
    Sphere() {}
    Sphere(Vector3D center, double radius)
    {
        reference_point = center;
        length = radius;
    }
    void draw() override
    {
        int stacks = 20;
        int slices = 20;
        Vector3D points[100][100];
        int i, j;
        double h, r;
        glPushMatrix();
        {
            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            // generate points
            for (i = 0; i <= stacks; i++)
            {
                h = length * sin(((double)i / (double)stacks) * (pi / 2));
                r = length * cos(((double)i / (double)stacks) * (pi / 2));
                for (j = 0; j <= slices; j++)
                {
                    points[i][j].x = r * cos(((double)j / (double)slices) * (pi * 2.0));
                    points[i][j].y = r * sin(((double)j / (double)slices) * (pi * 2.0));
                    points[i][j].z = h;
                }
            }
            // draw quads using generated points
            for (i = 0; i < stacks; i++)
            {
                // glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
                for (j = 0; j < slices; j++)
                {
                    glColor3f(color.r, color.g, color.b);
                    glBegin(GL_QUADS);
                    {
                        glColor3f(color.r, color.g, color.b);
                        // upper hemisphere
                        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);

                        // lower hemisphere
                        glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                    }
                    glEnd();
                }
            }
            glTranslatef(-reference_point.x, -reference_point.y, -reference_point.z);
        }
        glPopMatrix();
    }
    double intersect(Ray &ray, Color &color, int level) override
    {
        double a, b, c;
        Vector3D Center, Origin, Direction;
        double Radius;
        Center = reference_point;
        Origin = ray.start;
        Direction = ray.dir;
        Direction.normalize();
        Radius = length;
        a = 1.0;
        b = ((ray.dir) * (ray.start - reference_point));
        b = b * 2.0;
        c = ((Origin - Center) * (Origin - Center));
        c = c - Radius * Radius;
        double delta = b * b - 4 * a * c;
        if (delta < 0)
            return -1;
        double t1 = (-b - sqrt(delta)) / (2 * a);
        double t2 = (-b + sqrt(delta)) / (2 * a);
        if (t1 < 0 && t2 < 0)
            return -1;
        if (t1 > 0)
            return t1;
        if (t2 > 0)
            return t2;
        return -1;
    }
    Vector3D getNormalAt(Vector3D &point)
    {
        return (point - reference_point)._normalize();
    }
};

class Triangle : public Object
{
public:
    Vector3D a, b, c;
    Triangle() {}
    Triangle(Vector3D a, Vector3D b, Vector3D c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }
    void draw() override
    {
        glPushMatrix();
        {
            glColor3f(color.r, color.g, color.b);
            glBegin(GL_TRIANGLES);
            {
                glVertex3f(a.x, a.y, a.z);
                glVertex3f(b.x, b.y, b.z);
                glVertex3f(c.x, c.y, c.z);
            }
            glEnd();
        }
        glPopMatrix();
    }
    double intersect(Ray &ray, Color &color, int level) override
    {
        Vector3D p0 = a;
        Vector3D p1 = b;
        Vector3D p2 = c;
        Vector3D origin = ray.start;
        Vector3D dir = ray.dir;
        Vector3D e1 = p1 - p0;
        Vector3D e2 = p2 - p0;
        Vector3D tmp = e1 ^ dir;
        double det = tmp * e2;
        if (abs(det) < eps)
            return -1;
        double inv_det = 1 / det;
        Vector3D s = origin - p0;
        double u = (s ^ e2) * dir * (-inv_det);
        if (u < 0 || u > 1)
            return -1;
        double v = (e1 ^ dir) * s * inv_det;
        if (v < 0 || u + v > 1)
            return -1;
        double t = (s ^ e2) * e1 * (-inv_det);
        if (t < eps)
            return -1;
        return t;
    }
    Vector3D getNormalAt(Vector3D &point)
    {
        return ((b - a) ^ (c - a))._normalize();
    }
};

class QuadraticShape : public Object
{
public:
    double a, b, c, d, e, f, g, h, i, j;
    QuadraticShape() {}
    QuadraticShape(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, double k, double l, double m, double n, double o, double p)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
        this->g = g;
        this->h = h;
        this->i = i;
        this->j = j;
        this->length = n;
        this->width = o;
        this->height = p;
        this->reference_point = Vector3D(k, l, m);
    }
    bool withinSurface(Vector3D point)
    {
        if ((point.x < reference_point.x || point.x > reference_point.x + length) && this->length != 0)
            return false;
        if ((point.y < reference_point.y || point.y > reference_point.y + width) && this->width != 0)
            return false;
        if ((point.z < reference_point.z || point.z > reference_point.z + height) && this->height != 0)
            return false;
        return true;
    }
    double intersect(Ray &ray, Color &color, int level) override
    {
        double Aq = this->a * ray.dir.x * ray.dir.x + this->b * ray.dir.y * ray.dir.y + this->c * ray.dir.z * ray.dir.z + this->d * ray.dir.x * ray.dir.y + this->e * ray.dir.x * ray.dir.z + this->f * ray.dir.y * ray.dir.z;
        double Bq = 2 * this->a * ray.start.x * ray.dir.x + 2 * this->b * ray.start.y * ray.dir.y + 2 * this->c * ray.start.z * ray.dir.z + this->d * (ray.start.x * ray.dir.y + ray.start.y * ray.dir.x) + this->e * (ray.start.x * ray.dir.z + ray.start.z * ray.dir.x) + this->f * (ray.start.y * ray.dir.z + ray.start.z * ray.dir.y) + this->g * ray.dir.x + this->h * ray.dir.y + this->i * ray.dir.z;
        double Cq = this->a * ray.start.x * ray.start.x + this->b * ray.start.y * ray.start.y + this->c * ray.start.z * ray.start.z + this->d * ray.start.x * ray.start.y + this->e * ray.start.x * ray.start.z + this->f * ray.start.y * ray.start.z + this->g * ray.start.x + this->h * ray.start.y + this->i * ray.start.z + this->j;
        double delta = Bq * Bq - 4 * Aq * Cq;
        if (delta < 0)
            return -1;
        if (Aq == 0.0)
        {
            double t = -Cq / Bq;
            if (t < 0)
                return -1;
            Vector3D point = ray.start + ray.dir * t;
            if (withinSurface(point))
                return t;
            else
                return -1;
        }
        double t1 = (-Bq - sqrt(delta)) / (2 * Aq);
        double t2 = (-Bq + sqrt(delta)) / (2 * Aq);
        if (t1 < 0 && t2 < 0)
            return -1;
        Vector3D point1 = ray.start + ray.dir * t1;
        Vector3D point2 = ray.start + ray.dir * t2;
        if (withinSurface(point1) && withinSurface(point2))
            return min(t1, t2);
        else if (withinSurface(point1))
            return t1;
        else if (withinSurface(point2))
            return t2;
        else
            return -1;
    }
    Vector3D getNormalAt(Vector3D &point)
    {
        Vector3D normal = Vector3D(2 * a * point.x + d * point.y + e * point.z + g, 2 * b * point.y + d * point.x + f * point.z + h, 2 * c * point.z + e * point.x + f * point.y + i);
        return normal._normalize();
    }
};

class Floor : public Object
{
public:
    Floor() {}
    Floor(double floorWidth, double tileWidth)
    {
        this->color = Color(0.5, 0.5, 0.5);
        this->reference_point = Vector3D(-floorWidth / 2, -floorWidth / 2, 0);
        this->length = tileWidth;
        this->width = tileWidth;
        this->height = 0;   
        this->ambient = 0.8;
        this->diffuse = 0.6;
        this->specular = 0.6;
        this->reflection = 0.6;
        this->shine = 0.;
    }
    void draw()
    {
        double floorWidth = -(reference_point.x * 2.0);
        double tileWidth = length;
        int numTiles = (floorWidth / tileWidth);
        Vector3D ref_point;
        glPushMatrix();
        {
            for (int row = -numTiles; row < numTiles; row++)
            {
                for (int col = -numTiles; col < numTiles; col++)
                {
                    ref_point.x = -(floorWidth / 2.0) + (row * tileWidth);
                    ref_point.y = -(floorWidth / 2.0) + (col * tileWidth);
                    ref_point.z = 0;
                    glBegin(GL_QUADS);
                    {
                        if ((col + row) % 2 == 0)
                            glColor3f(0.0, 0.0, 0.0);
                        else
                            glColor3f(1.0, 1.0, 1.0);
                        glVertex3f(ref_point.x, ref_point.y, ref_point.z);
                        glVertex3f(ref_point.x + tileWidth, ref_point.y, ref_point.z);
                        glVertex3f(ref_point.x + tileWidth, ref_point.y + tileWidth, ref_point.z);
                        glVertex3f(ref_point.x, ref_point.y + tileWidth, ref_point.z);
                    }
                    glEnd();
                }
            }
        }
        glPopMatrix();
    }
    bool withinSurface(Vector3D point)
    {
        double halfFloorWidth = -(reference_point.x);
        if (point.x < -halfFloorWidth || point.x > halfFloorWidth)
            return false;
        if (point.y < -halfFloorWidth || point.y > halfFloorWidth)
            return false;
        return true;
    }

    double intersect(Ray &ray, Color &color, int level) override
    {
        double t = -(ray.start.z / ray.dir.z);
        if (t < 0)
            return -1;
        Vector3D point = ray.start + ray.dir * t;
        if (withinSurface(point))
            return t;
        else
            return -1;
    }

    Color getColorAt(Vector3D &intersectionPoint) override
    {
        if (!withinSurface(intersectionPoint))
            return Color(0, 0, 0);

        int i = (int)((reference_point.x + intersectionPoint.x) / length);
        int j = (int)((reference_point.y + intersectionPoint.y) / length);

        if ((i + j) % 2 == 0)
            return Color(1, 1, 1);
        return Color(0, 0, 0);
    }

    Vector3D getNormalAt(Vector3D &point)
    {
        return Vector3D(0, 0, 1);
    }
};