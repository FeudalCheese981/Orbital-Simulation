#define _USE_MATH_DEFINES
#include <cmath>

#include "satellite.hpp"

double wrapTwoPi(double angleRadians)
{
	double newAngle = std::fmod(angleRadians, 2.0 * M_PI);
	if (newAngle < 0)
		newAngle += 2.0 * M_PI;
	return newAngle;
}

Satellite::Satellite
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
)
	: satelliteTransform(parentBody->getPos(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	satelliteName = name;
	satelliteDryMass = dryMass;
	satelliteFuelMass = fuelMass;
	satelliteOrbitLineColour = orbitLineColour;
	changeParentBody(parentBody);
	calculateOrbitalParameters
	(
		longitude,
		latitude,
		azimuth,
		altitude,
		velocity,
		flightPathAngle,
		time
	);
	satelliteIcon = std::make_unique<CircleIcon>(glm::vec3(orbitLineColour), name, glm::vec3(0.0f));
}

void Satellite::draw(Shader& shapeShader, Shader& textShader, Shader& orbitLineShader, Camera& camera, Text& textObj)
{
	if (satelliteOrbitMesh != nullptr)
	{
		satelliteIcon->draw(shapeShader, textShader, camera, textObj);

		orbitLineShader.activate();
		satelliteTransform.uniform(orbitLineShader);
		satelliteOrbitMesh->draw(GL_LINE_STRIP);
		if (burnPlanned)
			satelliteNewOrbitMesh->draw(GL_LINES);
	}
}

void Satellite::changeParentBody(Planet* parentBody)
{
	satelliteTransform.setPosition(parentBody->getPos());
	satelliteTransform.setRotation(parentBody->getRotation());
	satelliteParentBody = parentBody;
	gravitationalParameter = G * parentBody->getMass();
}

void Satellite::calculateOrbitalParameters
(
	double longitude,
	double latitude,
	double azimuth,
	double altitude,
	double velocity,
	double flightPathAngle,
	double time
)
{
	satelliteDistance = altitude + satelliteParentBody->getRadius();
	satelliteVelocity = velocity;
	satelliteFlightPathAngle = flightPathAngle;

	satelliteEccentricity = sqrt
	(
		pow(((satelliteDistance * pow(satelliteVelocity, 2)) / gravitationalParameter - 1), 2)
		* pow(cos(satelliteFlightPathAngle), 2)
		+ pow(sin(satelliteFlightPathAngle), 2)
	);

	satelliteSemiMajorAxis = 1.0 / ((2.0 / satelliteDistance) - (pow(satelliteVelocity, 2) / gravitationalParameter));

	satelliteApoapsis = satelliteSemiMajorAxis * (1 + satelliteEccentricity);
	satellitePeriapsis = satelliteSemiMajorAxis * (1 - satelliteEccentricity);

	satelliteTrueAnomaly = atan2
	(
		(((satelliteDistance * pow(satelliteVelocity, 2)) / gravitationalParameter) * cos(satelliteFlightPathAngle) * sin(satelliteFlightPathAngle)),
		(((satelliteDistance * pow(satelliteVelocity, 2)) / gravitationalParameter) * pow(cos(satelliteFlightPathAngle), 2) - 1)
	);
	satelliteTrueAnomaly = wrapTwoPi(satelliteTrueAnomaly);

	satelliteEccentricAnomaly = atan2
	(
		sqrt(1 - pow(satelliteEccentricity, 2)) * sin(satelliteTrueAnomaly),
		satelliteEccentricity + cos(satelliteTrueAnomaly)
	);
	satelliteEccentricAnomaly = wrapTwoPi(satelliteEccentricAnomaly);

	satelliteMeanAnomaly = satelliteEccentricAnomaly - satelliteEccentricity * sin(satelliteEccentricAnomaly);
	satelliteMeanAnomaly = wrapTwoPi(satelliteMeanAnomaly);

	satelliteOrbitalPeriod = 2.0 * M_PI * sqrt(pow(satelliteSemiMajorAxis, 3) / gravitationalParameter);
	satelliteMeanMotion = 2.0 * M_PI / satelliteOrbitalPeriod;

	satelliteEpochOfPeriapsis = time - satelliteMeanAnomaly / satelliteMeanMotion;
	
	double deltaLongitude = atan2
	(
		sin(latitude) * sin(azimuth),
		cos(azimuth)
	);
	satelliteLongitudeOfAscendingNode = longitude - deltaLongitude;

	satelliteInclination = acos(cos(latitude) * sin(azimuth));
	
	double l = atan2
	(
		tan(latitude),
		cos(azimuth)
	);
	satelliteArgumentOfPeriapsis = l - satelliteTrueAnomaly;

	satelliteOrbitMesh = std::make_unique<Mesh>
	(
		generateOrbitLine(
			1024, 
			satelliteEccentricity, 
			satelliteSemiMajorAxis, 
			satelliteArgumentOfPeriapsis, 
			satelliteInclination, 
			satelliteLongitudeOfAscendingNode, 
			satelliteOrbitLineColour
		)
	);
}

void Satellite::updatePosition(double time)
{
	satelliteTrueAnomaly = calculateAnomaly(time);

	satelliteDistance = calculateDistance(satelliteTrueAnomaly);
	satelliteVelocity = calculateVelocity(satelliteTrueAnomaly);
	satelliteFlightPathAngle = calculateFlightPathAngle(satelliteTrueAnomaly);
	
	double x = satelliteDistance * cos(satelliteTrueAnomaly);
	double y = satelliteDistance * sin(satelliteTrueAnomaly);
	double z = 0.0;

	glm::vec3 pos = glm::vec3(x, y, z);

	glm::mat4 rotation = glm::mat4(1.0f);
	rotation = glm::rotate(rotation, (float)satelliteLongitudeOfAscendingNode, glm::vec3(0.0f, 0.0f, 1.0f));
	rotation = glm::rotate(rotation, (float)satelliteInclination, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, (float)satelliteArgumentOfPeriapsis, glm::vec3(0.0f, 0.0f, 1.0f));
	pos = glm::vec3(rotation * glm::vec4(pos, 1.0f));

	satellitePos = pos;
	satelliteIcon->updatePos(glm::vec3(satelliteTransform.getTranslationMatrix() * satelliteTransform.getRotationMatrix() * satelliteTransform.getScaleMatrix() * glm::vec4(pos, 1.0f)));
}

double Satellite::calculateAnomaly(double time)
{
	const int iterations = 50;

	satelliteMeanAnomaly = satelliteMeanMotion * (time - satelliteEpochOfPeriapsis);
	satelliteMeanAnomaly = wrapTwoPi(satelliteMeanAnomaly);

	double M = satelliteMeanAnomaly;
	double e = satelliteEccentricity;
	double E = M + satelliteEccentricity * sin(M); // heuristic first guess


	// Halley's iteration method for root finding
	for (int i = 0; i < iterations; i++)
	{
		// calculate function and first and second derivatives
		double f_E = E - e * sin(e) - M; // f(E)
		double f1_E = 1 - e * cos(E); // f'(E)
		double f2_E = e * sin(E); // f"(E)

		// apply to iteration formula
		E = E - (f_E * f1_E) / (pow(f1_E, 2) - (0.5 * f_E * f2_E));
	}

	satelliteEccentricAnomaly = wrapTwoPi(E);

	double trueAnomaly = 2.0 * atan(sqrt(1 + satelliteEccentricity) / (1 - satelliteEccentricity) * tan(satelliteEccentricAnomaly / 2.0));
	trueAnomaly = wrapTwoPi(trueAnomaly);
	return trueAnomaly;
}

double Satellite::calculateDistance(double trueAnomaly)
{
	double distance = (satelliteSemiMajorAxis * (1 - pow(satelliteEccentricity, 2))) / (1 + satelliteEccentricity * cos(trueAnomaly));
	return distance;
}

double Satellite::calculateVelocity(double trueAnomaly)
{
	double velocity = sqrt(gravitationalParameter * ((2.0 / (satelliteSemiMajorAxis * (1 - pow(satelliteEccentricity, 2))) / (1 + satelliteEccentricity * cos(trueAnomaly))) - (1.0 / satelliteSemiMajorAxis)));
	return velocity;
}

double Satellite::calculateFlightPathAngle(double trueAnomaly)
{
	double flightPathAngle = atan((satelliteEccentricity * sin(trueAnomaly)) / (1 + satelliteEccentricity * cos(trueAnomaly)));
	return flightPathAngle;
}

std::string Satellite::getName()
{
	return satelliteName;
}