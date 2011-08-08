/*
 *  mcl.c
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

#include "mcl.h"
#include "rand.h"
#include <stdio.h>
#include <unistd.h>
#include <OpenGL/gl.h>

//parameters for tweaking
static const float motion_length_offset=0.0010;
static const float motion_angle_offset=0.0010;
static const float motion_length_mult=1;
static const float motion_angle_mult=1;
static const float measurement_gauss_box_sigma=0.00001;
static const float measurement_gauss_laser_sigma=0.00001;
static const float measurement_gauss_laser_error_sigma_sq=1/(2*M_PI);
static const uint8_t add_random_samples=1;
static const float random_new_sample_frac=0.05;
static const float raycast_incr=.01;
static const float kidnapped_angle_thresh=M_PI/50;
static const float kidnapped_length_thresh=.1;

float circlemod(float radians)
{
    while(radians>M_PI*2)
	radians-=M_PI*2;
    while(radians<0)
	radians+=M_PI*2;
    return radians;
}

float gauss(const float x, const float mu, const float sigma_sq)
{
    float mu_x=mu-x; //eliminating pow
    return 1/sqrtf(2*M_PI*sigma_sq)*expf(-mu_x*mu_x/(2*sigma_sq));
}

void resetParticles(Particle* particles, const unsigned int numParticles)
{
    for(int i=0;i<numParticles;i++) {
	Pose* particlePose=&(particles[i].pose);
	particlePose->x=2*randf()-1;
	particlePose->y=2*randf()-1;
	particlePose->theta=circlemod(M_PI*2*randf()+M_PI/2);
	/*
	particlePose->x=randgauss(0,.1);
	particlePose->y=randgauss(0,.1);
	particlePose->theta=randgauss(M_PI/2,.1);
	*/
	particles[i].weight=1.f/numParticles;
    }
}

void normalize(Particle* particles, const unsigned int numParticles)
{
    float total=0;
    for(int i=0;i<numParticles;i++)
	total+=particles[i].weight;
    for(int i=0;i<numParticles;i++)
	particles[i].weight/=total;
}

void motion_model(Particle* particle, const Command* command, const Map* map)
{
    Pose* particlePose=&(particle->pose);

    particlePose->theta+=randgauss(command->delta_angle,motion_angle_offset+motion_angle_mult*command->delta_angle);
    particlePose->theta=circlemod(particlePose->theta);
    float deltalength=randgauss(command->delta_length,motion_length_offset+motion_length_mult*command->delta_length);
    particlePose->x+=deltalength*cosf(particlePose->theta);
    particlePose->y+=deltalength*sinf(particlePose->theta);
}

float range_find(Pose* pose, const Map* map)
{
    Pose collision=raycast(pose,map);
    return hypot(collision.x-pose->x,collision.y-pose->y);
}

void measurement_model(Particle* particle, const SensorData* data, const Map* map)
{
    //inside vs outside the box
    float laserrange=randgauss(range_find(&(particle->pose),map),measurement_gauss_laser_sigma);
    float laserrange_error=laserrange-data->laserrange;
    //gaussian model
    float laserrange_gauss_error=gauss(laserrange_error,0,measurement_gauss_laser_error_sigma_sq);
    float laserweight=laserrange_gauss_error;

    particle->weight=laserweight;
    //kill the weight if in an invalid location
    /*
    if(getVal_float(particle->pose.x,particle->pose.y,map))
	particle->weight=0;
	*/

}

void sir(Particle* particles, const unsigned int numParticles)
{
    normalize(particles,numParticles);

    //TODO(ebakan): lower space/copying requirements

    Particle copy_particles[numParticles];
    for(int i=0;i<numParticles;i++)
	copy_particles[i]=particles[i];

    //algorithm shamelessly stolen from
    //An Improved Particle Filter for Non-linear Problems
    //James Carpenter Peter Clifford Paul Fearnhead

    //TODO(ebakan): verify this algorithm is correct,
    //replace/rewrite if necessary
    float u=1.f/numParticles;
    float q_j=copy_particles[0].weight;
    float t_i=0;
    float t_n=(numParticles+1)*u;
    int i=0,j=1;
    while(i<numParticles) {
	if(q_j*t_n>t_i) {
	    particles[i++]=copy_particles[j-1];
	    t_i+=u;
	}
	else {
	    q_j+=copy_particles[j++].weight;
	}
    }
}

void addRandomParticles(Particle* particles, const unsigned int numParticles, const unsigned int numNewParticles)
{
    const unsigned int numOldParticles=numParticles-numNewParticles;
    sir(particles,numOldParticles);

    //static const float new_particle_sigma_sq=1.f/(8*M_PI*M_PI*M_PI);

    //Pose guess=approxPose(particles,numOldParticles);
    //float weight=1.f/numParticles;
    for(int i=numOldParticles;i<numParticles;i++) {
	particles[i].pose.x=randf()*2-1;
	particles[i].pose.y=randf()*2-1;
	particles[i].pose.theta=randf()*M_PI*2;
	/*
	particles[i].pose.x=randgauss(guess.x,measurement_gauss_laser_error_sigma_sq);
	if(particles[i].pose.x>1)
	    particles[i].pose.x=1;
	else if(particles[i].pose.x<-1)
	    particles[i].pose.x=-1;
	particles[i].pose.y=randgauss(guess.y,measurement_gauss_laser_error_sigma_sq);
	if(particles[i].pose.y>1)
	    particles[i].pose.y=1;
	else if(particles[i].pose.y<-1)
	    particles[i].pose.y=-1;
	particles[i].pose.theta=circlemod(randgauss(guess.theta,new_particle_sigma_sq));
	*/
	particles[i].weight=0;
    }
}

void mcl(Particle* particles, const unsigned int numParticles, const Command* command, const SensorData* sensorData, const Map* map)
{
    //check to see if we've been kidnapped!
    if(fabs(command->delta_angle)>=kidnapped_angle_thresh || fabs(command->delta_length)>=kidnapped_length_thresh) {
	resetParticles(particles,numParticles);
    }
    for(int i=0;i<numParticles;i++) {
	motion_model(particles+i,command,map);
	measurement_model(particles+i,sensorData,map);
    }
    sir(particles,numParticles);
    if(add_random_samples)
	//addRandomParticles(particles,numParticles,numParticles-((int)n_eff(particles,numParticles)));
	addRandomParticles(particles,numParticles,numParticles*random_new_sample_frac);
}

float n_eff(Particle* particles, const unsigned int numParticles)
{
    normalize(particles,numParticles);
    float sum_variance=0;
    for(int i=0;i<numParticles;i++)
	sum_variance+=particles[i].weight*particles[i].weight;
    return 1/sum_variance;
}

Pose approxPose(Particle* particles, const unsigned int numParticles)
{
    Pose outPose = {0,0,0};
    for(unsigned int i=0;i<numParticles;i++) {
	Pose* currPose=&(particles[i].pose);
	float currWeight=particles[i].weight;
	outPose.x+=currWeight*currPose->x;
	outPose.y+=currWeight*currPose->y;
	outPose.theta+=currWeight*circlemod(currPose->theta);
    }
    outPose.theta=circlemod(outPose.theta);
    return outPose;
}

BoundingBox getBox(const Pose* pose, const Map* map)
{
    BoundingBox box;
    float x=pose->x;
    float y=pose->y;
    uint16_t xval=(x+1)/2*map->width;
    uint16_t yval=(-y+1)/2*map->height;
    box.left=((float)xval/map->width*2)-1;
    box.right=box.left+2.f/map->width;
    box.top=-(((float)yval/map->height*2)-1);
    box.bottom=box.top-2.f/map->height;
    return box;
}

Pose boxIntersection(const Pose* pose, const BoundingBox* box)
{
    //using y-y1=m(x-x1), with m=tan(particle->pose.theta)
    float x=pose->x;
    float y=pose->y;
    float theta=circlemod(pose->theta);
    float m=tanf(theta);

    //NOTE: there's a lot of copypasta because it's quicker to
    //determine which values need to be computed based on the angle
    //rather than compute everything before and then select which
    //computed values to use

    //values of the intersections on each axis
    float x_bound_side,y_bound_side;

    //choose 2 sides to compare based on the orientation
    if(theta<=M_PI/2) { //quadrant I: x_pos and y_pos
	x_bound_side=box->right;
	y_bound_side=box->top;
    }
    else if(theta<=M_PI) { //quadrant II: x_neg and y_pos
	x_bound_side=box->left; 
	y_bound_side=box->top;
    }
    else if(theta<=3*M_PI/2) { //quadrant III: x_neg and y_neg
	x_bound_side=box->left;
	y_bound_side=box->bottom;
    }
    else { //quadrant IV: x_pos and y_neg
	x_bound_side=box->right;
	y_bound_side=box->bottom;
    }

    //values of the intersection on each axis
    float x_dist_1=x_bound_side-x;
    float y_dist_1=y_bound_side-y;
    float x_axis_val=m*(x_dist_1)+y;
    float y_axis_val=(y_dist_1)/m+x;

    //terms to be squared
    float x_dist_2=x_axis_val-y;
    float y_dist_2=y_axis_val-x;

    //distances squared
    float x_sq=x_dist_1*x_dist_1+x_dist_2*x_dist_2;
    float y_sq=y_dist_1*y_dist_1+y_dist_2*y_dist_2;

    if(x_sq<y_sq) {
	x=x_bound_side;
	y=x_axis_val;
    }
    else {
	x=y_axis_val;
	y=y_bound_side;
    }

    Pose out={x,y,pose->theta};
    return out;
}

Pose raycast(const Pose* pose, const Map* map)
{
    static const float incr=0.001;

    if(getVal_float(pose->x,pose->y,map)) {
	return *pose;
    }

    BoundingBox box;
    Pose intersection=*pose;

    
    //until we hit a box
    int count=map->width+map->height;
    float cosincr=cosf(intersection.theta)*incr;
    float sinincr=sinf(intersection.theta)*incr;
    while(!getVal_float(intersection.x,intersection.y,map) && count-->0) {

	//get the current box and intersection
	box = getBox(&intersection,map);
	intersection = boxIntersection(&intersection,&box);

	//push the intersection ever so slightly
	intersection.x+=cosincr;
	intersection.y+=sinincr;
    }
    //compensate for that little addition
    intersection.x-=cosincr;
    intersection.y-=sinincr;

    /*
    glColor3f(1.f,0.f,1.f);
    glBegin(GL_LINES);
    glVertex2f(box.left,box.top);
    glVertex2f(box.right,box.top);
    glVertex2f(box.right,box.top);
    glVertex2f(box.right,box.bottom);
    glVertex2f(box.right,box.bottom);
    glVertex2f(box.left,box.bottom);
    glVertex2f(box.left,box.bottom);
    glVertex2f(box.left,box.top);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(pose->x,pose->y);
    glVertex2f(intersection.x,intersection.y);
    glEnd();
    */
    return intersection;
}

