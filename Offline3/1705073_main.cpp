#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "1705073_classes.hpp"
#include <bits/stdc++.h>
#include "bitmap_image.hpp"
#include <GL/glut.h>

#define pi (2 * acos(0.0))
#define rotate_amount 3.0
double fovY = 80.0;
double aspect = 1.0;
double zNear = 1.0;
double zFar = 1000.0;
Vector3D pos(100.0, 100.0, 0.0), u_unit(0.0, 0.0, 1.0), r_unit(-1 / sqrt(2), 1 / sqrt(2), 0.0), l_unit(-1 / sqrt(2), -1 / sqrt(2), 0.0);
#define INF 1000000000

int windowWidth = 600;
int windowHeight = 600;
string filename = "pluginfile.txt";
int recursion_level;
int pixelCount;
vector<Object *> objects;
vector<Light *> lights;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

Vector3D add_point(Vector3D a, Vector3D b)
{
    Vector3D p;
    p.x = a.x + b.x;
    p.y = a.y + b.y;
    p.z = a.z + b.z;
    return p;
}

Vector3D sub_point(Vector3D a, Vector3D b)
{
    Vector3D p;
    p.x = a.x - b.x;
    p.y = a.y - b.y;
    p.z = a.z - b.z;
    return p;
}

Vector3D cross_product(Vector3D a, Vector3D b)
{
    Vector3D p;
    p.x = a.y * b.z - a.z * b.y;
    p.y = a.z * b.x - a.x * b.z;
    p.z = a.x * b.y - a.y * b.x;
    return p;
}

Vector3D rotate_point(Vector3D a, double angle, Vector3D b)
{
    Vector3D p = cross_product(a, b);
    p.x = a.x * cos(angle) + p.x * sin(angle);
    p.y = a.y * cos(angle) + p.y * sin(angle);
    p.z = a.z * cos(angle) + p.z * sin(angle);
    return p;
}

// operations

void move_forward()
{
    pos = add_point(pos, l_unit);
}

void move_backward()
{
    pos = sub_point(pos, l_unit);
}

void move_left()
{
    pos = sub_point(pos, r_unit);
}

void move_right()
{
    pos = add_point(pos, r_unit);
}

void move_up()
{
    pos = add_point(pos, u_unit);
}

void move_down()
{
    pos = sub_point(pos, u_unit);
}

void rotate_left()
{
    r_unit = rotate_point(r_unit, -rotate_amount * pi / 180, u_unit);
    l_unit = rotate_point(l_unit, -rotate_amount * pi / 180, u_unit);
}

void rotate_right()
{
    r_unit = rotate_point(r_unit, rotate_amount * pi / 180, u_unit);
    l_unit = rotate_point(l_unit, rotate_amount * pi / 180, u_unit);
}

void look_up()
{
    l_unit = rotate_point(l_unit, -rotate_amount * pi / 180, r_unit);
    u_unit = rotate_point(u_unit, -rotate_amount * pi / 180, r_unit);
}

void look_down()
{
    l_unit = rotate_point(l_unit, rotate_amount * pi / 180, r_unit);
    u_unit = rotate_point(u_unit, rotate_amount * pi / 180, r_unit);
}

void tilt_clockwise()
{
    r_unit = rotate_point(r_unit, -rotate_amount * pi / 180, l_unit);
    u_unit = rotate_point(u_unit, -rotate_amount * pi / 180, l_unit);
}

void tilt_anticlockwise()
{
    r_unit = rotate_point(r_unit, rotate_amount * pi / 180, l_unit);
    u_unit = rotate_point(u_unit, rotate_amount * pi / 180, l_unit);
}

void drawAxes()
{
    if (drawaxes == 1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f(100, 0, 0);
            glVertex3f(-100, 0, 0);

            glVertex3f(0, -100, 0);
            glVertex3f(0, 100, 0);

            glVertex3f(0, 0, 100);
            glVertex3f(0, 0, -100);
        }
        glEnd();
    }
}

void drawGrid()
{
    int i;
    if (drawgrid == 1)
    {
        glColor3f(0.6, 0.6, 0.6); // grey
        glBegin(GL_LINES);
        {
            for (i = -8; i <= 8; i++)
            {
                if (i == 0)
                    continue; // SKIP the MAIN axes

                // lines parallel to Y-axis
                glVertex3f(i * 10, -90, 0);
                glVertex3f(i * 10, 90, 0);

                // lines parallel to X-axis
                glVertex3f(-90, i * 10, 0);
                glVertex3f(90, i * 10, 0);
            }
        }
        glEnd();
    }
}

void drawInputs()
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
    for (int i = 0; i < lights.size(); i++)
    {
        lights[i]->draw();
    }
}


void loadData()
{
    ifstream input;
    ofstream output;
    input.open(filename);
    if (!input.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }
    int objectCount;
    string objectName;
    Object *object;
    input >> recursion_level >> pixelCount >> objectCount;
    for (int i = 0; i < objectCount; i++)
    {
        input >> objectName;
        if (objectName == "sphere")
        {
            Vector3D center;
            double radius;
            input >> center.x >> center.y >> center.z >> radius;
            object = new Sphere(center, radius);
            Color color;
            input >> color.r >> color.g >> color.b;
            object->setColor(color);
            input >> object->ambient >> object->diffuse >> object->specular >> object->reflection;
            input >> object->shine;
            objects.push_back(object);
        }
        if (objectName == "triangle")
        {
            Vector3D a, b, c;
            input >> a.x >> a.y >> a.z;
            input >> b.x >> b.y >> b.z;
            input >> c.x >> c.y >> c.z;
            object = new Triangle(a, b, c);
            Color color;
            input >> color.r >> color.g >> color.b;
            object->setColor(color);
            input >> object->ambient >> object->diffuse >> object->specular >> object->reflection;
            input >> object->shine;
            objects.push_back(object);
        }
        else if (objectName == "general")
        {
            double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
            input >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m >> n >> o >> p;
            object = new QuadraticShape(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
            Color color;
            input >> color.r >> color.g >> color.b;
            object->setColor(color);
            input >> object->ambient >> object->diffuse >> object->specular >> object->reflection;
            input >> object->shine;
            objects.push_back(object);
        }
    }
    object = new Floor(1000.0, 20.0);
    objects.push_back(object);

    int lightCount;
    input >> lightCount;
    Light *light;
    for (int i = 0; i < lightCount; i++)
    {
        light = new Light();
        input >> light->light_pos.x >> light->light_pos.y >> light->light_pos.z;
        input >> light->color.r >> light->color.g >> light->color.b;
        lights.push_back(light);
    }
}

void capture()
{
    cout << "capture started" << endl;
    bitmap_image image(pixelCount, pixelCount);

    for (int i = 0; i < pixelCount; i++)
    {
        for (int j = 0; j < pixelCount; j++)
        {
            image.set_pixel(i, j, 0.0, 0.0, 0.0);
        }
    }

    double planeDistance = (windowHeight / (2.0)) * tan(fovY * (pi / 360.0));
    Vector3D topLeft = pos + (l_unit * planeDistance) - r_unit * (windowWidth / 2.0) + u_unit * (windowHeight / 2.0);

    double du = ((double)windowWidth / pixelCount);
    double dv = ((double)windowHeight / pixelCount);

    topLeft = topLeft + r_unit * (du / 2.0) - u_unit * (dv / 2.0);

    for (int column = 0; column < pixelCount; column++)
    {
        for (int row = 0; row < pixelCount; row++)
        {
            Vector3D curPixel = topLeft + r_unit * (column * du) - u_unit * (row * dv);
            Ray ray(pos, curPixel - pos);

            int intersectingPoint = -1;
            double t;
            double tMin = INT_MAX;
            Color color;

            for (int i = 0; i < objects.size(); i++)
            {
                t = objects[i]->intersectWithPhong(ray, color, 0);
                if (t > 0 && t < tMin)
                {
                    tMin = t;
                    intersectingPoint = i;
                }
            }

            if (intersectingPoint != -1)
            {
                tMin = objects[intersectingPoint]->intersectWithPhong(ray, color, recursion_level);
            }

            color.clip();
            image.set_pixel(column, row, (int)round(color.r * 256.0), (int)round(color.g * 256.0), (int)round(color.b * 256.0));
        }
    }

    image.save_image("output.bmp");

    cout << "capture finished" << endl;
    image.clear();
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '0':
        capture();
        break;
    case '1':
        rotate_left();
        drawgrid=1-drawgrid;
        break;
    case '2':
        rotate_right();
        break;
    case '3':
        look_up();
        break;
    case '4':
        look_down();
        break;
    case '5':
        tilt_clockwise();
        break;
    case '6':
        tilt_anticlockwise();
        break;
    default:
        break;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN: // down arrow key
        move_backward();
        // cameraHeight -= 3.0;
        break;
    case GLUT_KEY_UP: // up arrow key
        move_forward();
        // cameraHeight += 3.0;
        break;

    case GLUT_KEY_RIGHT:
        move_right();
        // cameraAngle += 0.03;
        break;
    case GLUT_KEY_LEFT:
        move_left();
        // cameraAngle -= 0.03;
        break;

    case GLUT_KEY_PAGE_UP:
        move_up();
        break;
    case GLUT_KEY_PAGE_DOWN:
        move_down();
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;
    default:
        break;
    }
}

void mouseListener(int button, int state, int x, int y)
{ // x, y is the x-y of the screen (2D)
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        { // 2 times?? in ONE click? -- solution is checking DOWN or UP
            drawaxes = 1 - drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}

void display()
{

    // clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0); // color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    // load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    // initialize the matrix
    glLoadIdentity();

    // now give three info
    // 1. where is the camera (viewer)?
    // 2. where is the camera looking?
    // 3. Which direction is the camera's UP direction?

    // gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //  gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    //  gluLookAt(0,0,200,	0,0,0,	0,1,0);
    gluLookAt(pos.x, pos.y, pos.z, pos.x + l_unit.x, pos.y + l_unit.y, pos.z + l_unit.z, u_unit.x, u_unit.y, u_unit.z);

    // again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);

    /****************************
    / Add your objects from here
    ****************************/
    // add objects

    drawAxes();
    drawGrid();
    drawInputs();

    // ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}

void animate()
{
    angle += 0.05;
    // codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    // codes for initialization
    drawgrid = 0;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;

    // clear the screen
    glClearColor(0, 0, 0, 0);

    /************************
    / set-up projection here
    ************************/
    // load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    // initialize the matrix
    glLoadIdentity();

    // give PERSPECTIVE parameters
    gluPerspective(fovY, aspect, zNear, zFar);
    // field of view in the Y (vertically)
    // aspect ratio that determines the field of view in the X direction (horizontally)
    // near distance
    // far distance
}

int main(int argc, char **argv)
{
    loadData();
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color

    glutCreateWindow("Task 3");

    init();

    glEnable(GL_DEPTH_TEST); // enable Depth Testing

    glutDisplayFunc(display); // display callback function
    glutIdleFunc(animate);    // what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop(); // The main loop of OpenGL

    objects.clear();
    lights.clear();

    return 0;
}
