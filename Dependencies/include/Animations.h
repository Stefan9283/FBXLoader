#pragma once
#include "Common.h"

void freeSkellyBoi(Bone* node);
Bone* addToSkellyBoi(std::vector<Bone*>* bones, FbxNode* node, unsigned int* index);
Bone* createSkellyboi(FbxNode* node, std::vector<Bone*>* bones);

