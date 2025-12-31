#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "shape.hpp"


MeshData generateSphere(double radius, int segments, glm::vec4 colour)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int rings = segments / 2;

	for (int i = 0; i <= rings; ++i)
	{
		double phi = M_PI * (double)i / (double)rings;
		double v = - (double)i / (double)rings;

		for (int j = 0; j <= segments; ++j)
		{
			double theta = 2.0f * M_PI * (double)j / (double)segments - M_PI;
			double u = (double)j / (double)segments;
			//if (u >= 1.0) u -= 1.0;

			double x = radius * sin(phi) * cos(theta);
			double y = radius * sin(phi) * sin(theta);
			double z = radius * cos(phi);

			glm::vec3 pos = glm::vec3(x, y, z);
			glm::vec3 normal = glm::normalize(pos);
			glm::vec2 texUV = glm::vec2(u, v);

			vertices.push_back(Vertex{ pos, normal, colour, texUV });
		}
	}

	for (int i = 0; i < rings; ++i)
	{
		for (int j = 0; j < segments; ++j)
		{
			int i0 = i * (segments + 1) + j;
			int i1 = i0 + 1;
			int i2 = i0 + (segments + 1);
			int i3 = i2 + 1;

			indices.push_back(i0);
			indices.push_back(i3);
			indices.push_back(i1);
			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}

	MeshData meshData = {
		vertices,
		indices
	};
	return meshData;
}

MeshData generateOrbitLine(int segments, double eccentricity, double semiMajorAxis, double argumentOfPeriapsis, double inclination, double longitudeOfAscendingNode, glm::vec4 colour)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i <= segments; i++)
	{
		double phi = 2.0 * M_PI * (double)i / (double)segments;
		double r = semiMajorAxis * (1 - pow(eccentricity, 2)) / (1 + eccentricity * cos(phi));

		double x = r * cos(phi);
		double y = r * sin(phi);
		double z = 0.0;

		glm::vec3 pos = glm::vec3(x, y, z);
		pos = glm::rotate(pos, (float)argumentOfPeriapsis, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		pos = glm::rotate(pos, (float)inclination, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
		pos = glm::rotate(pos, (float)longitudeOfAscendingNode, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

		vertices.push_back(Vertex{ pos, glm::vec3(0.0f), colour, glm::vec2(0.0f) });
		indices.push_back(i);
	}

	MeshData meshData =
	{
		vertices,
		indices
	};

	return meshData;
}