#pragma once
#include "Common.h"

void printFbxDouble3(FbxDouble3 v);
glm::vec3 FbxDouble3tovec3(FbxDouble3 v);

glm::mat4 extractTransformFromNode(FbxNode* node);

float interpolateValue(std::vector<std::pair<float, signed long>> keyfr, double time);