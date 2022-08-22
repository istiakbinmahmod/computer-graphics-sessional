#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// #include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#define wheel_radius 35.0
#define rotation_amount 2.0

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

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

point origin(0.0, 0.0, 0.0);
double y_angle = 0.0;
double z_angle = 0.0;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f( 100,0,0);
			glColor3f(1.0, 1.0, 0.0);
			glVertex3f(-100,0,0);

			glColor3f(0.0, 1.0, 1.0);
			glVertex3f(0,-100,0);
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0, 100,0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0,0, 100);
			glColor3f(0.0, 1.0, 1.0);
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

				// if(i==0)
				// 	continue;	//SKIP the MAIN axes

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

void drawCylinder(double radius, double height)
{
    int slices = 100;
    struct point points[slices+1];
    int i,j;
    double h,r;
    //generate points
    for(i=0;i<=slices;i++)
    {
        points[i].x = radius*cos(((double)i/(double)slices)*2*pi);
        points[i].y = radius*sin(((double)i/(double)slices)*2*pi);
    }
    glColor3f(0.0,1.0,0.0);
    //draw quads using generated points
    for(i=0;i<slices;i++)
    {
        glColor3f((double)i/(double)slices, (double)i/(double)slices, (double)i/(double)slices);
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

void drawWheel(double radius)
{
    int height = 10;
    glPushMatrix();
    {
        glRotatef(90,1,0,0);
        drawCylinder(radius,height);
    }
    glPopMatrix();
    glBegin(GL_QUADS);
    {
        glColor3f(0.7, 0.7, 0.7);
        glVertex3f(radius, height, 0);
        glVertex3f(radius, -height, 0);
        glVertex3f(-radius, -height, 0);
        glVertex3f(-radius, height, 0);
    }
    glEnd();
    glBegin(GL_QUADS);
    {
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(0, height, radius);
        glVertex3f(0, -height, radius);
        glVertex3f(0, -height, -radius);
        glVertex3f(0, height, -radius);
    }
    glEnd();
}

void drawSS()
{
	// glRotatef(180, 0, 0, 1);
    glTranslatef(origin.x, origin.y, origin.z);
    glRotatef(z_angle, 0.0, 0.0, 1.0);
    glPushMatrix();
    {
        glTranslatef(0, 0, wheel_radius);
        glRotatef(y_angle, 0.0, 1.0, 0.0);
        drawWheel(wheel_radius);
    }
    glPopMatrix();
}

void move_forward()
{
    origin.x += (rotation_amount/360.0) * 2 * pi * wheel_radius * cos(z_angle * pi / 180.0);
    origin.y += (rotation_amount/360.0) * 2 * pi * wheel_radius * sin(z_angle * pi / 180.0);
    y_angle += rotation_amount;
}

void move_backward()
{
    origin.x -= (rotation_amount/360.0) * 2 * pi * wheel_radius * cos(z_angle * pi / 180.0);
    origin.y -= (rotation_amount/360.0) * 2 * pi * wheel_radius * sin(z_angle * pi / 180.0);
    y_angle -= rotation_amount;
}

void rotate_left()
{
    z_angle -= rotation_amount;
}

void rotate_right()
{
    z_angle += rotation_amount;
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'w':
			move_forward();
			break;
        case 's':
            move_backward();
            break;
        case 'a':
            rotate_left();
            break;
        case 'd':
            rotate_right();
            break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
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
	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	// gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




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
	drawgrid=1;
	// drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

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
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Task 3");

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
