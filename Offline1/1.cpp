#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// #include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#define max_space 35.0
#define rotate_amount 3.0

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double space = 25.0;

struct point
{
	double x,y,z;
    point(){}
    point(double x,double y,double z)
    {
        this->x=x;
        this->y=y;
        this->z=z;
    }
};

point pos, u_unit, r_unit, l_unit;

point add_point(point a, point b)
{
    point p;
    p.x=a.x+b.x;
    p.y=a.y+b.y;
    p.z=a.z+b.z;
    return p;
}

point sub_point(point a, point b)
{
    point p;
    p.x=a.x-b.x;
    p.y=a.y-b.y;
    p.z=a.z-b.z;
    return p;
}

point cross_product(point a, point b)
{
    point p;
    p.x=a.y*b.z-a.z*b.y;
    p.y=a.z*b.x-a.x*b.z;
    p.z=a.x*b.y-a.y*b.x;
    return p;
}

point rotate_point(point a, double angle, point b)
{
    point p = cross_product(a, b);
    p.x=a.x*cos(angle)+p.x*sin(angle);
    p.y=a.y*cos(angle)+p.y*sin(angle);
    p.z=a.z*cos(angle)+p.z*sin(angle);
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
    r_unit = rotate_point(r_unit, -rotate_amount*pi/180, u_unit);
    l_unit = rotate_point(l_unit, -rotate_amount*pi/180, u_unit);
}

void rotate_right()
{
    r_unit = rotate_point(r_unit, rotate_amount*pi/180, u_unit);
    l_unit = rotate_point(l_unit, rotate_amount*pi/180, u_unit);
}

void look_up()
{
    l_unit = rotate_point(l_unit, -rotate_amount*pi/180, r_unit);
    u_unit = rotate_point(u_unit, -rotate_amount*pi/180, r_unit);
}

void look_down()
{
    l_unit = rotate_point(l_unit, rotate_amount*pi/180, r_unit);
    u_unit = rotate_point(u_unit, rotate_amount*pi/180, r_unit);
}

void tilt_clockwise()
{
    r_unit = rotate_point(r_unit, -rotate_amount*pi/180, l_unit);
    u_unit = rotate_point(u_unit, -rotate_amount*pi/180, l_unit);
}

void tilt_anticlockwise()
{
    r_unit = rotate_point(r_unit, rotate_amount*pi/180, l_unit);
    u_unit = rotate_point(u_unit, rotate_amount*pi/180, l_unit);
}


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void draw_one_eighth_sphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(pi/2.0));
			points[i][j].y=r*sin(((double)j/(double)slices)*(pi/2.0));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        // glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void draw_one_fourth_cylinder(double radius, double height, int slices, int segments)
{
    struct point points[slices+1];
    int i,j;
    double h,r;
    //generate points
    for(i=0;i<=slices;i++)
    {
        points[i].x = radius*cos(((double)i/(double)slices)*(pi/2.0));
        points[i].y = radius*sin(((double)i/(double)slices)*(pi/2.0));
    }
    glColor3f(0.0,1.0,0.0);
    //draw quads using generated points
    for(i=0;i<slices;i++)
    {
        glBegin(GL_QUADS);
        {
            glVertex3f(points[i].x,points[i].y,height);
            glVertex3f(points[i+1].x,points[i+1].y,height);
            glVertex3f(points[i+1].x,points[i+1].y,-height);
            glVertex3f(points[i].x,points[i].y,-height);
        }
        glEnd();
    }
}

void draw_eight_sphere()
{
    double radius = max_space - space;
    // double space = 10.0;
    glColor3f(1.0,0.0,0.0);
    // xyz 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(space ,space ,space);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // xyz'
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(space ,space ,-space);
        glRotatef(90, 0.0, 1.0, 0.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // xy'z 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(space ,-space ,space);
        glRotatef(90, 1.0, 0.0, 0.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // xy'z' 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(space ,-space ,-space);
        glRotatef(180, 1.0, 0.0, 0.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // x'yz 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(-space ,space ,space);
        glRotatef(90, 0.0, 0.0, 1.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // x'yz' 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(-space ,space ,-space);
        glRotatef(180, 0.0, 1.0, 0.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // x'y'z 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(-space ,-space ,space);
        glRotatef(180, 0.0, 0.0, 1.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
    // x'y'z' 
    glPushMatrix();
    {
        glColor3f(1.0,0.0,0.0);
        glTranslatef(-space ,-space ,-space);
        glRotatef(-90, 0.0, 0.0, 1.0);
        glRotatef(180, 1.0, 0.0, 0.0);
        draw_one_eighth_sphere(radius,20,20);
    }
    glPopMatrix();
}

void draw_six_square()
{
    // double radius = max_space - space;
    // double space = 10.0;
    double len = max_space;
    glColor3f(1.0, 1.0, 1.0);
    // xy above
    glPushMatrix();
    {
        glTranslatef(0, 0, len);
        drawSquare(space);
    }
    glPopMatrix();
    // xy below
    glPushMatrix();
    {
        glTranslatef(0, 0, -len);
        drawSquare(space);
    }
    glPopMatrix();
    // xz above
    glPushMatrix();
    {
        glTranslatef(0, len, 0);
        glRotatef(90, 1.0, 0.0, 0.0);
        drawSquare(space);
    }
    glPopMatrix();
    // xz below
    glPushMatrix();
    {
        glTranslatef(0 ,-len, 0);
        glRotatef(90, 1.0, 0.0, 0.0);
        drawSquare(space);
    }
    glPopMatrix();
    // yz above
    glPushMatrix();
    {
        glTranslatef(len , 0, 0);
        glRotatef(90, 0.0, 1.0, 0.0);
        drawSquare(space);
    }
    glPopMatrix();
    // yz below
    glPushMatrix();
    {
        glTranslatef(-len , 0, 0);
        glRotatef(90, 0.0, 1.0, 0.0);
        drawSquare(space);
    }
    glPopMatrix();
}

void draw_twelve_cylinders()
{
    double radius = max_space - space;
    double height = space;
    double len = space;
    int slices = 50;
    int segments = 50;
    glColor3f(1.0,0.0,0.0);
    // 1
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(len, len, 0.0);
        glRotatef(0, 1.0, 0.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 2
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(len, 0.0, len);
        glRotatef(90, 1.0, 0.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 3
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(len, -len, 0.0);
        glRotatef(180, 1.0, 0.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 4
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(len, 0.0, -len);
        glRotatef(270, 1.0, 0.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();

    // 5
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(-len, len, 0.0);
        glRotatef(0, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 0.0, 1.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 6
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(-len, 0.0, len);
        glRotatef(90, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 0.0, 1.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 7
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(-len, -len, 0.0);
        glRotatef(180, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 0.0, 1.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 8
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(-len, 0.0, -len);
        glRotatef(270, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 0.0, 1.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 9
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(0.0, len, -len);
        glRotatef(0, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 10
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(0.0, len, len);
        glRotatef(90, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 11
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(0.0, -len, len);
        glRotatef(180, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
    // 12
    glPushMatrix();
    {
        glColor3f(0.0,1.0,0.0);
        glTranslatef(0.0, -len, -len);
        glRotatef(270, 1.0, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        draw_one_fourth_cylinder(radius,height, slices, segments);
    }
    glPopMatrix();
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
            rotate_left();
			// drawgrid=1-drawgrid;
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


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
            move_backward();
			// cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
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
            if(space-1.0 < 0.0)
                break;
            else
                space -= 1.0;
			break;
		case GLUT_KEY_END:
            if(space+1.0 > max_space)
                break;
            else
                space += 1.0;
		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
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



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	// gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	// gluLookAt(0,0,200,	0,0,0,	0,1,0);
    gluLookAt(pos.x,pos.y,pos.z,    pos.x+l_unit.x,pos.y+l_unit.y,pos.z+l_unit.z,  u_unit.x, u_unit.y, u_unit.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    draw_eight_sphere();
    draw_six_square();
    draw_twelve_cylinders();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

    pos = point(100.0, 100.0, 0.0);
    u_unit = point(0.0, 0.0, 1.0);
    r_unit = point(-1/sqrt(2), 1/sqrt(2), 0.0);
    l_unit = point(-1/sqrt(2), -1/sqrt(2), 0.0);

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Task 1 & 2");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
