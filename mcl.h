/**
  * mcl.h
  * Written by Eric Bakan
  * A bunch of useful functions
  * for Monte-Carlo localization
  */

#ifndef MCL_C
#define MCL_C

#include "structs.h"
#include "map.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//given an angle in the set (-inf,inf),
//returns an equivalent angle in the set
//[0,2PI)
float circlemod(float radians);

//returns the y value of a given gaussian distribution
float gauss(const float x, const float mu, const float sigma);

//randomizes all the particles' poses, resets their weights
void resetParticles(Particle* particles, const unsigned int numParticles);

//normalizes the weights of the particles
void normalize(Particle* particles, const unsigned int numParticles);

//in-place motion model with Gaussian error, modifying particle's pose member and truncating points outside the map
//NOTE: deltaPose is given with respect to the global frame of reference, not the robot's
//i.e. these values are calculated from the gyro and encoder values before being passed in
void motion_model(Particle* particle, const Command* command, const Map* map);

//gives the range between the pose and the nearest object
//only works for the perimeter of the screen for now,
//extend later
float range_find(Pose* pose, const Map* map);

//in-place motion model with Gaussian error, modifying the particle's weight member
void measurement_model(Particle* particle, const SensorData* data, const Map* map);

//perform in-place SIR functions given the list of particles
void sir(Particle* particles, const unsigned int numParticles);

//perform in-place Monte Carlo localization
void mcl(Particle* particles, const unsigned int numParticles, const Command* command, const SensorData* data, const Map* map);

//calculate the number of effective particles of the system
float n_eff(Particle* particles, const unsigned int numParticles);

//approximate the pose of the robot given a set of particles
Pose approxPose(Particle* particles, const unsigned int numParticles);

//given a map and a pose, return the values of the four lines which enclose the pose
BoundingBox getBox(const Pose* pose, const Map* map);

//given a pose and four bounding lines, where does that the ray extending from the pose intersect its boundary?
Pose boxIntersection(const Pose* pose, const BoundingBox* box);

//really basic, really inefficient raycasting method
Pose raycast(const Pose* pose, const Map* map);

#endif//MCL_C

