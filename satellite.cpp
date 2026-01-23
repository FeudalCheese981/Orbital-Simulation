#define _USE_MATH_DEFINES
#include <cmath>

#include "satellite.hpp"

double wrapTwoPi(double angleRadians)
{
	double newAngle = std::fmod(angleRadians, 2.0 * M_PI); // Angle MOD 2 Pi
	if (newAngle < 0) // results less than 0 add 2 Pi
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
	: satelliteTransform(parentBody->getPos(), parentBody->getRotation(), glm::vec3(1.0f)) // Initialise Transform
{
	// Set Satellite attributes
	satelliteName = name;
	satelliteDryMass = dryMass;
	satelliteFuelMass = fuelMass;
	satelliteOrbitLineColour = orbitLineColour;
	// Set Parent Body
	changeParentBody(parentBody);
	// Calculate The orbital Elements
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
	// Initialise the Icons
	satelliteIcon = std::make_unique<CircleIcon>(glm::vec3(orbitLineColour), name, glm::vec3(0.0));

	// Calculate 3D x, y, z position of the point of Apoapsis and Periapsis
	glm::mat4 matrix = satelliteTransform.getTranslationMatrix() * satelliteTransform.getRotationMatrix() * satelliteTransform.getScaleMatrix();
	glm::vec3 apoapsisPos = glm::vec3(matrix * glm::vec4(trueAnomalyToCartesian(M_PI), 1.0f));
	glm::vec3 periapsisPos = glm::vec3(matrix* glm::vec4(trueAnomalyToCartesian(0), 1.0f));

	apoapsisIcon = std::make_unique<TriangleIcon>(glm::vec3(orbitLineColour) - glm::vec3(0.1f), "Apoapsis", apoapsisPos);
	periapsisIcon = std::make_unique<TriangleIcon>(glm::vec3(orbitLineColour) - glm::vec3(0.1f), "Periapsis", periapsisPos);
}

void Satellite::draw(Shader& shapeShader, Shader& textShader, Shader& orbitLineShader, Camera& camera, Text& textObj, float uiScale)
{
	// Don't draw if satellite is hidden
	if (hidden)
		return;
	// Dont draw if mesh not initialised
	if (satelliteOrbitMesh == nullptr)
		return;
	// Draw thicker line if selected
	if (selected) 
		glLineWidth(3.0f * uiScale);

	// Draw Icons
	satelliteIcon->draw(shapeShader, textShader, camera, textObj, uiScale);
	apoapsisIcon->draw(shapeShader, textShader, camera, textObj, uiScale);
	periapsisIcon->draw(shapeShader, textShader, camera, textObj, uiScale);

	// Activate Shader
	orbitLineShader.activate();
	// Send transformation matrix to shader
	satelliteTransform.uniform(orbitLineShader);
	// Draw trajectory mesh
	satelliteOrbitMesh->draw(GL_LINE_STRIP);

	// Reset line width
	glLineWidth(1.0f * uiScale);
}

void Satellite::changeParentBody(Planet* parentBody)
{
	// Set position and rotation to that of parent body
	// This aligns orbit with the parent bodies, equitorial reference frame
	satelliteTransform.setPosition(parentBody->getPos());
	satelliteTransform.setRotation(parentBody->getRotation());
	satelliteParentBody = parentBody;
	// Calculate the new gravitationalParameter
	gravitationalParameter = G * (parentBody->getMass() + satelliteDryMass + satelliteFuelMass);
}

void Satellite::updatePosition(double time)
{
	// Set new transform positon if parent body has moved in simulation
	satelliteTransform.setPosition(satelliteParentBody->getPos());

	// Calculate the true anomaly
	satelliteTrueAnomaly = calculateAnomaly(time);
	// Use new true anomaly to calculate position and velocity
	satelliteDistance = calculateDistance(satelliteTrueAnomaly);
	satelliteVelocity = calculateVelocity(satelliteTrueAnomaly);
	satelliteFlightPathAngle = calculateFlightPathAngle(satelliteTrueAnomaly);
	// Compute the 3D x, y, z coordinates of the satellite
	glm::vec3 pos = trueAnomalyToCartesian(satelliteTrueAnomaly);
	// Update the position
	satellitePos = pos;
	satelliteIcon->updatePos(glm::vec3(satelliteTransform.getTranslationMatrix() * satelliteTransform.getRotationMatrix() * satelliteTransform.getScaleMatrix() * glm::vec4(pos, 1.0f)));
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
	// Set distance velocity and flightPathAngle
	satelliteDistance = altitude + satelliteParentBody->getRadius();
	satelliteVelocity = velocity;
	satelliteFlightPathAngle = flightPathAngle;

	// Compute the Eccentricity
	satelliteEccentricity = sqrt
	(
		pow(((satelliteDistance * pow(satelliteVelocity, 2)) / gravitationalParameter - 1), 2)
		* pow(cos(satelliteFlightPathAngle), 2)
		+ pow(sin(satelliteFlightPathAngle), 2)
	);
	// Compute the Semi-major Axis
	satelliteSemiMajorAxis = 1.0 / ((2.0 / satelliteDistance) - (pow(satelliteVelocity, 2) / gravitationalParameter));

	// Find Apoapsis and Periapsis
	satelliteApoapsis = satelliteSemiMajorAxis * (1 + satelliteEccentricity);
	satellitePeriapsis = satelliteSemiMajorAxis * (1 - satelliteEccentricity);

	// Compute the initial True Anomaly
	satelliteTrueAnomaly = atan2
	(
		(((satelliteDistance * pow(satelliteVelocity, 2)) / gravitationalParameter) * cos(satelliteFlightPathAngle) * sin(satelliteFlightPathAngle)),
		(((satelliteDistance * pow(satelliteVelocity, 2)) / gravitationalParameter) * pow(cos(satelliteFlightPathAngle), 2) - 1)
	);
	satelliteTrueAnomaly = wrapTwoPi(satelliteTrueAnomaly);

	// Compute the initial Eccentric Anomaly
	satelliteEccentricAnomaly = atan2
	(
		sqrt(1 - pow(satelliteEccentricity, 2)) * sin(satelliteTrueAnomaly),
		satelliteEccentricity + cos(satelliteTrueAnomaly)
	);
	satelliteEccentricAnomaly = wrapTwoPi(satelliteEccentricAnomaly);

	// Compute the initial Mean Anomaly
	satelliteMeanAnomaly = satelliteEccentricAnomaly - satelliteEccentricity * sin(satelliteEccentricAnomaly);
	satelliteMeanAnomaly = wrapTwoPi(satelliteMeanAnomaly);

	// Compute the satellite's Orbital Period and Mean Motion
	satelliteOrbitalPeriod = 2.0 * M_PI * sqrt(pow(satelliteSemiMajorAxis, 3) / gravitationalParameter);
	satelliteMeanMotion = 2.0 * M_PI / satelliteOrbitalPeriod;

	// Compute the Epoch of periapsis in simulation time
	satelliteEpochOfPeriapsis = time - satelliteMeanAnomaly / satelliteMeanMotion;
	
	// Compute the Longitude of Ascending Node
	double deltaLongitude = atan2
	(
		sin(latitude) * sin(azimuth),
		cos(azimuth)
	);
	satelliteLongitudeOfAscendingNode = longitude - deltaLongitude;
	satelliteLongitudeOfAscendingNode = wrapTwoPi(satelliteLongitudeOfAscendingNode);

	// Compute the Inclination
	satelliteInclination = acos(cos(latitude) * sin(azimuth));
	satelliteInclination = wrapTwoPi(satelliteInclination);

	// Compute the Argument of Periapsis
	double l = atan2
	(
		tan(latitude),
		cos(azimuth)
	);
	satelliteArgumentOfPeriapsis = l - satelliteTrueAnomaly;
	satelliteArgumentOfPeriapsis = wrapTwoPi(satelliteArgumentOfPeriapsis);

	// Initialise the Trajectory Mesh
	satelliteOrbitMesh = std::make_unique<Mesh>
	(
		// Function Defined in shape.cpp
		generateOrbitLine(
			1024, 
			satelliteEccentricity, 
			satelliteSemiMajorAxis, 
			satelliteArgumentOfPeriapsis, 
			satelliteInclination, 
			satelliteLongitudeOfAscendingNode, 
			satelliteOrbitLineColour - glm::vec4(0.1f, 0.1f, 0.1f, 0.0f)
		)
	);
}

double Satellite::calculateAnomaly(double time)
{
	// 64 iterations is more than enough to give an accurate approximation of the Eccentric Anomaly
	const int iterations = 64;

	// Compute the Mean Anomaly for the current simulation time
	satelliteMeanAnomaly = satelliteMeanMotion * (time - satelliteEpochOfPeriapsis);
	satelliteMeanAnomaly = wrapTwoPi(satelliteMeanAnomaly);
	
	// Compute the Eccentric Anomaly from the Mean Anomaly
	double M = satelliteMeanAnomaly;
	double e = satelliteEccentricity;
	double E = M + satelliteEccentricity * sin(M); // heuristic first guess

	// Halley's iteration method for root finding
	for (int i = 0; i < iterations; i++)
	{
		// calculate function and first and second derivatives
		double f_E = E - e * sin(E) - M; // f(E)
		double f1_E = 1 - e * cos(E); // f'(E)
		double f2_E = e * sin(E); // f"(E)

		// apply to iteration formula
		E = E - (f_E * f1_E) / (pow(f1_E, 2) - (0.5 * f_E * f2_E));
	}

	satelliteEccentricAnomaly = wrapTwoPi(E);
	
	// Compute The True Anomaly from the Eccentric Anomaly
	double trueAnomaly = 2.0 * atan(sqrt(1 + satelliteEccentricity) / (1 - satelliteEccentricity) * tan(satelliteEccentricAnomaly / 2.0));
	trueAnomaly = wrapTwoPi(trueAnomaly);
	return trueAnomaly;
}

glm::vec3 Satellite::trueAnomalyToCartesian(double trueAnomaly)
{
	// Find the distance for the given True Anomaly
	double distance = calculateDistance(trueAnomaly);

	// Convert to 2D Cartesian
	double x = distance * cos(trueAnomaly);
	double y = distance * sin(trueAnomaly);
	double z = 0.0;

	glm::vec3 pos = glm::vec3(x, y, z);

	// Apply Euler Angle Transformation to get 3D Cartesian
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation = glm::rotate(rotation, (float)satelliteLongitudeOfAscendingNode, glm::vec3(0.0f, 0.0f, 1.0f));
	rotation = glm::rotate(rotation, (float)satelliteInclination, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, (float)satelliteArgumentOfPeriapsis, glm::vec3(0.0f, 0.0f, 1.0f));
	pos = glm::vec3(rotation * glm::vec4(pos, 1.0f));

	return pos;
}

double Satellite::calculateDistance(double trueAnomaly)
{
	double distance = (satelliteSemiMajorAxis * (1 - pow(satelliteEccentricity, 2))) / (1 + satelliteEccentricity * cos(trueAnomaly));
	return distance;
}

double Satellite::calculateVelocity(double trueAnomaly)
{
	double velocity = sqrt(gravitationalParameter * ((2.0 / calculateDistance(trueAnomaly)) - (1.0 / satelliteSemiMajorAxis)));
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

double Satellite::getAltitude()
{
	return satelliteDistance - satelliteParentBody->getRadius();
}

double Satellite::getVelocity()
{
	return satelliteVelocity;
}

double Satellite::getFlightPathAngle()
{
	return satelliteFlightPathAngle;
}

double Satellite::getApoapsis()
{
	return satelliteApoapsis - satelliteParentBody->getRadius();
}

double Satellite::getPeriapsis()
{
	return satellitePeriapsis - satelliteParentBody->getRadius();
}

double Satellite::getEccentricity()
{
	return satelliteEccentricity;
}

double Satellite::getSemiMajorAxis()
{
	return satelliteSemiMajorAxis;
}

double Satellite::getArgumentOfPeriapsis()
{
	return satelliteArgumentOfPeriapsis;
}

double Satellite::getInclination()
{
	return satelliteInclination;
}

double Satellite::getLongitudeOfAscendingNode()
{
	return satelliteLongitudeOfAscendingNode;
}

double Satellite::getOrbitalPeriod()
{
	return satelliteOrbitalPeriod;
}