#pragma once

#include <memory>
#include "shape.hpp"
#include "mesh.hpp"
#include "circleIcon.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "planet.hpp"

const double G = 6.673e-11;

double wrapTwoPi(double angleRadians);

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
	);
	~Satellite() = default;

	// making class move-only for memory safety
	Satellite(const Satellite&) = delete;
	Satellite& operator=(const Satellite&) = delete;
	Satellite(Satellite&&) noexcept = default; // garuntee exception safety
	Satellite& operator=(Satellite&&) noexcept = default;


	void draw(Shader& shapeShader, Shader& textShader, Shader& lineShader, Camera& camera, Text& textObj, float uiScale);

	// TODO: void manoeuvrePlan(double burnStartTime);
	void changeParentBody(Planet* parentBody);

	void updatePosition(double time);

	void calculateOrbitalParameters
	(
		double longitude,
		double latitude,
		double azimuth,
		double altitude,
		double velocity,
		double flightPathAngle,
		double time
	);

	double calculateAnomaly(double time);
	double calculateDistance(double trueAnomaly);
	double calculateVelocity(double trueAnomaly);
	double calculateFlightPathAngle(double trueAnomaly);

	std::string getName();

private:
	std::unique_ptr<CircleIcon> satelliteIcon;

	std::unique_ptr<Mesh> satelliteOrbitMesh;
	std::unique_ptr<Mesh> satelliteNewOrbitMesh;

	Transform satelliteTransform;

	std::string satelliteName;
	double satelliteDryMass;
	double satelliteFuelMass;
	double satelliteSpecificImpulse;

	double satelliteEccentricity;
	double satelliteSemiMajorAxis;
	double satelliteArgumentOfPeriapsis;
	double satelliteInclination;
	double satelliteLongitudeOfAscendingNode;
	double satelliteEpochOfPeriapsis;

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

	glm::vec3 satellitePos;
	glm::vec4 satelliteOrbitLineColour;

	bool burnPlanned = false;
	bool selected = false;

	Planet* satelliteParentBody;
	double gravitationalParameter;
};