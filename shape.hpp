#pragma once

#include "meshData.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

// Generates vertices and indices for a sphere
MeshData generateSphere(double radius, int segments, glm::vec4 colour);
// Generates vertices and indices for an orbital trajectory
MeshData generateOrbitLine(int segments, double eccentricity, double semiMajorAxis, double argumentOfPeriapsis, double inclination, double longitudeOfAscendingNode, glm::vec4 colour);