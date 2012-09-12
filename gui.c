/*
 *  gui.c
 *  Copyright (C) 2011 Eric Bakan
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
  * Callback functions for GLUT
  */

#include "gui.h"
#include <stdio.h>

#define NUM_SPECIAL_KEYS 21

Particle* m_particles;
unsigned int m_numParticles;
Pose m_robotPose;
Map m_fieldmap;

//keyboard smoothers
uint8_t m_keys[127]; //change if support for non-ascii chars needed
uint8_t m_specialKeys[NUM_SPECIAL_KEYS];

unsigned int m_width;
unsigned int m_height;

//parameters
static const uint8_t SHOW_NEFF=0;
static const float particle_size=2;
static const float guess_size=5;
static const float robot_size=10;
static const float guess_dir_len=.05;
static const float robot_dir_len=.10;
static const float particle_width=1;
static const float guess_width=4;
static const float robot_width=2;
static const float delta_rot=M_PI/100;
static const float delta_trans=.02;
static const float robot_map_gauss_sigma=0.1;

void resetField()
{
    //reset robot
    m_robotPose.x=0;
    m_robotPose.y=0;
    m_robotPose.theta=M_PI/2;

    //reset particles
    resetParticles(m_particles,m_numParticles);
    
    glutPostRedisplay();
}

void resetMap()
{
    clearMap(&m_fieldmap);
    {
	#include "fieldmap.h"
	byteArrayToBitArray(fieldmap_bytes,m_fieldmap.width*m_fieldmap.height,m_fieldmap.data);
    }
    glutPostRedisplay();
}

//converts a GLUT special key to an int
//so that we can store this in a smaller
//data structure
uint8_t specialKeyToInt(int key)
{
    switch(key) {
	case GLUT_KEY_F1:
	    return 0;
	case GLUT_KEY_F2:
	    return 1;
	case GLUT_KEY_F3:
	    return 2;
	case GLUT_KEY_F4:
	    return 3;
	case GLUT_KEY_F5:
	    return 4;
	case GLUT_KEY_F6:
	    return 5;
	case GLUT_KEY_F7:
	    return 6;
	case GLUT_KEY_F8:
	    return 7;
	case GLUT_KEY_F9:
	    return 8;
	case GLUT_KEY_F10:
	    return 9;
	case GLUT_KEY_F11:
	    return 10;
	case GLUT_KEY_F12:
	    return 11;
	case GLUT_KEY_LEFT:
	    return 12;
	case GLUT_KEY_UP:
	    return 13;
	case GLUT_KEY_RIGHT:
	    return 14;
	case GLUT_KEY_DOWN:
	    return 15;
	case GLUT_KEY_PAGE_UP:
	    return 16;
	case GLUT_KEY_PAGE_DOWN:
	    return 17;
	case GLUT_KEY_HOME:
	    return 18;
	case GLUT_KEY_END:
	    return 19;
	case GLUT_KEY_INSERT:
	    return 20;
	default:
	    return -1;
    }
}

void glutKeyboardCallback(unsigned char key, int x, int y)
{
    m_keys[(int)key]=1;
}

void glutKeyboardUpCallback(unsigned char key, int x, int y)
{
    m_keys[(int)key]=0;
}

void glutSpecialKeyboardCallback(int key, int x, int y)
{
    m_specialKeys[specialKeyToInt(key)]=1;
}

void glutSpecialKeyboardUpCallback(int key, int x, int y)
{
    m_specialKeys[specialKeyToInt(key)]=0;
}

void glutReshapeCallback(int w, int h)
{
    glViewport(0,0,w,h);
    m_width=w;
    m_height=h;
}

void glutMouseFuncCallback(int button, int state, int x, int y)
{
    if(button==0 && state==0) {
	float x_scaled=((float)x/m_width)*2-1;
	float y_scaled=-(((float)y/m_height)*2-1);
	if(!getVal_float(x_scaled,y_scaled,&m_fieldmap)) {
	    float deltax=x_scaled-m_robotPose.x;
	    float deltay=y_scaled-m_robotPose.y;
	    float deltalen=hypot(deltax,deltay);
	    float newtheta=atan2f(deltay,deltax);
	    Command command={newtheta-m_robotPose.theta,deltalen};
	    m_robotPose.x=x_scaled;
	    m_robotPose.y=y_scaled;
	    m_robotPose.theta=newtheta;
	    SensorData sensorData;
	    sensorData.laserrange=range_find(&m_robotPose,&m_fieldmap);
	    mcl(m_particles, m_numParticles, &command, &sensorData, &m_fieldmap);
	}
    }
	glutPostRedisplay();
}

void glutMotionFuncCallback(int x,int y)
{
    //do nothing for now
}

void glutDisplayCallback(void)
{
    Pose mcl_guess=approxPose(m_particles, m_numParticles);
    //printf("g: x: %f y: %f r: %f\n",mcl_guess.x,mcl_guess.y,mcl_guess.theta);

    //for now only display particles, add map later
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float newx,newy;

    float maxWeight=m_particles[0].weight;
    for(int i=1;i<m_numParticles;i++) {
	float weight=m_particles[i].weight;
	if(weight>maxWeight)
	    maxWeight=weight;
    }

    glPointSize(particle_size);
    glLineWidth(particle_width);

    //map
    glColor3f(1.f,0.f,0.f);
    drawGrid(&m_fieldmap);
    drawMap(&m_fieldmap);

    //random
    glBegin(GL_POINTS);
    for(int i=0;i<m_numParticles;i++) {
	Particle particle=m_particles[i];
	glColor3f(particle.weight/maxWeight,particle.weight/maxWeight,particle.weight/maxWeight);
	glVertex2f(particle.pose.x,particle.pose.y);
    }
    glEnd();

    //guess
    glPointSize(guess_size);
    glLineWidth(guess_width);
    glBegin(GL_POINTS);
    glColor3f(0.f,1.f,0.f);
    glVertex2f(mcl_guess.x,mcl_guess.y);
    glEnd();
    glBegin(GL_LINES);
    newx=mcl_guess.x+guess_dir_len*cosf(mcl_guess.theta);
    newy=mcl_guess.y+guess_dir_len*sinf(mcl_guess.theta);
    glVertex2f(mcl_guess.x,mcl_guess.y);
    glVertex2f(newx,newy);
    glEnd();
    
    //robot
    glPointSize(robot_size);
    glLineWidth(robot_width);
    glBegin(GL_POINTS);
    glColor3f(0.f,0.f,1.f);
    glVertex2f(m_robotPose.x,m_robotPose.y);
    glEnd();
    glBegin(GL_LINES);
    newx=m_robotPose.x+robot_dir_len*cosf(m_robotPose.theta);
    newy=m_robotPose.y+robot_dir_len*sinf(m_robotPose.theta);
    glVertex2f(m_robotPose.x,m_robotPose.y);
    glVertex2f(newx,newy);
    glEnd();

    //raycasting
    glColor3f(1.f,1.f,0.f);
    Pose collision=raycast(&m_robotPose,&m_fieldmap);

    glBegin(GL_LINES);
    glVertex2f(m_robotPose.x,m_robotPose.y);
    glVertex2f(collision.x,collision.y);
    glEnd();

    glutSwapBuffers();
}

void glutIdleCallback()
{
    //handle keypresses - outsource to function at some point?

    //check for a quit
    if(m_keys['q']) {
	exit(0);
    }
    if(m_keys[' ']) {
	resetField();
    }
    if(m_keys['r']) {
	resetField();
	resetMap();
    }

    uint8_t up=m_specialKeys[specialKeyToInt(GLUT_KEY_UP)];
    uint8_t down=m_specialKeys[specialKeyToInt(GLUT_KEY_DOWN)];
    uint8_t left=m_specialKeys[specialKeyToInt(GLUT_KEY_LEFT)];
    uint8_t right=m_specialKeys[specialKeyToInt(GLUT_KEY_RIGHT)];

    if(up || down || left || right) {
	//generate a deltapose from keyboard stuff
	Command command = {0,0};
	if(left) {
	    command.delta_angle+=delta_rot;
	}
	if(right) {
	    command.delta_angle-=delta_rot;
	}

	if(up)  {
	    command.delta_length+=delta_trans;
	}
	if(down) {
	    command.delta_length-=delta_trans;
	}

	float deltax=command.delta_length*cosf(m_robotPose.theta);
	float deltay=command.delta_length*sinf(m_robotPose.theta);

	if(fabsf(m_robotPose.x+deltax) > 1.0 || fabsf(m_robotPose.y+deltay) > 1.0 || getVal_float(m_robotPose.x+deltax,m_robotPose.y+deltay,&m_fieldmap) || getVal_float(m_robotPose.x+deltax,m_robotPose.y,&m_fieldmap) || getVal_float(m_robotPose.x,m_robotPose.y+deltay,&m_fieldmap)) {
	    command.delta_length=0;
	    deltax=0;
	    deltay=0;
	}

	/*
	if(m_robotPose.x+deltaPose.x>1.0)
	    deltaPose.x=1.0-m_robotPose.x;
	else if(m_robotPose.x+deltaPose.x<-1.0)
	    deltaPose.x=-1.0-m_robotPose.x;

	if(m_robotPose.y+deltaPose.y>1.0)
	    deltaPose.y=1.0-m_robotPose.y;
	else if(m_robotPose.y+deltaPose.y<-1.0)
	    deltaPose.y=-1.0-m_robotPose.y;
	*/

	m_robotPose.x+=deltax;
	m_robotPose.y+=deltay;
	m_robotPose.theta+=command.delta_angle;
	m_robotPose.theta=circlemod(m_robotPose.theta);

	SensorData sensorData;
	sensorData.laserrange=range_find(&m_robotPose,&m_fieldmap);

	mcl(m_particles, m_numParticles, &command, &sensorData, &m_fieldmap);
	if(SHOW_NEFF)
	    printf("n_eff: %f\n",n_eff(m_particles,m_numParticles));

	glutPostRedisplay();
    }
}

void initGlut(int argc, char** argv, unsigned int numParticles, char* title, unsigned int width, unsigned int height)
{
    m_numParticles=numParticles;
    m_width=width;
    m_height=height;

    //create the map
    //put in own namespace so the large fieldmap array is deallocated
    {
	#include "fieldmap.h"
	initMap(&m_fieldmap,fieldheight,fieldwidth);
	byteArrayToBitArray(fieldmap_bytes,m_fieldmap.width*m_fieldmap.height,m_fieldmap.data);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(m_width,m_height);
    glutCreateWindow(title);

    glClearColor(0.f,0.5,0.5,1.f);

    glutKeyboardFunc(glutKeyboardCallback);
    glutKeyboardUpFunc(glutKeyboardUpCallback);
    glutSpecialFunc(glutSpecialKeyboardCallback);
    glutSpecialUpFunc(glutSpecialKeyboardUpCallback);
    glutReshapeFunc(glutReshapeCallback);
    glutIdleFunc(glutIdleCallback);
    glutMouseFunc(glutMouseFuncCallback);
    glutPassiveMotionFunc(glutMotionFuncCallback);
    glutMotionFunc(glutMotionFuncCallback);
    glutDisplayFunc(glutDisplayCallback);

    m_particles=malloc(m_numParticles*sizeof(Particle));
    resetField();
    //resetMap();

    glutMainLoop();

    //menus?

}

