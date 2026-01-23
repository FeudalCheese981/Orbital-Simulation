#pragma once

#include <memory>
#include "shape.hpp"
#include "mesh.hpp"
#include "circleIcon.hpp"
#include "triangleIcon.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "planet.hpp"

const double G = 6.673e-11; // Gravitational Constant

double wrapTwoPi(double angleRadians); // Function to wrap an angle to 0 to 2 Pi

// Satellite Class - stores information about a satellite in orbit and its trajectory
class Satellite
{
public:
	Satellite
	(
		std::string name,
		double dryMass,
		double fuelMass,
		glm::vec4 orbitLineColour,
		Planet* parentBody,
		double longitude,
		double latitude,
		double azimuth,
		double altitude,
		double velocity,
		double flightPathAngle,
		double time
	); // Initialise the Satellite from Launch Parameters
	~Satellite() = default;

	// Making class move-only for memory safety
	Satellite(const Satellite&) = delete;
	Satellite& operator=(const Satellite&) = delete;
	Satellite(Satellite&&) noexcept = default; // Guarantee exception safety
	Satellite& operator=(Satellite&&) noexcept = default;

	void draw(Shader& shapeShader, Shader& textShader, Shader& lineShader, Camera& camera, Text& textObj, float uiScale); // Draws satellite Icon and Trajectory

	void changeParentBody(Planet* parentBody); // Set The parent body to given Planet

	void updatePosition(double time); // Set new satellite position at new simulation time

	void calculateOrbitalParameters
	(
		double longitude,
		double latitude,
		double azimuth,
		double altitude,
		double velocity,
		double flightPathAngle,
		double time
	); // Calculates orbital elements from launch parameters

	// Helper Functions to compute respective attributes
	double calculateAnomaly(double time);
	double calculateDistance(double trueAnomaly);
	double calculateVelocity(double trueAnomaly);
	double calculateFlightPathAngle(double trueAnomaly);
	glm::vec3 trueAnomalyToCartesian(double trueAnomaly);

	// Getters for attributes
	std::string getName();	
	double getAltitude();
	double getVelocity();
	double getFlightPathAngle();
	double getApoapsis();
	double getPeriapsis();
	double getEccentricity();
	double getSemiMajorAxis();
	double getArgumentOfPeriapsis();
	double getInclination();
	double getLongitudeOfAscendingNode();
	double getOrbitalPeriod();

	bool selected = true;
	bool hidden = false;

private:
	// Icons
	std::unique_ptr<CircleIcon> satelliteIcon;
	std::unique_ptr<TriangleIcon> apoapsisIcon;
	std::unique_ptr<TriangleIcon> periapsisIcon;

	// Trajectory Mesh
	std::unique_ptr<Mesh> satelliteOrbitMesh;

	// Transform
	Transform satelliteTransform;

	// Satellite basic attributes
	std::string satelliteName;
	double satelliteDryMass;
	double satelliteFuelMass;

	// Orbital elements
	double satelliteEccentricity;
	double satelliteSemiMajorAxis;
	double satelliteArgumentOfPeriapsis;
	double satelliteInclination;
	double satelliteLongitudeOfAscendingNode;
	double satelliteEpochOfPeriapsis;

	// Other Orbital Information
	double satelliteApoapsis;
	double satellitePeriapsis;

	double satelliteOrbitalPeriod;
	double satelliteMeanMotion;

	double satelliteMeanAnomaly;
	double satelliteEccentricAnomaly;
	double satelliteTrueAnomaly;

	double satelliteDistance;
	double satelliteVelocity;
	double satelliteFlightPathAngle;

	// 3D space position and colour
	glm::vec3 satellitePos;
	glm::vec4 satelliteOrbitLineColour;

	// Pointer to the satellites parent body
	Planet* satelliteParentBody;

	double gravitationalParameter;
};