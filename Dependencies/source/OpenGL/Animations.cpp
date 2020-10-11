#include "OpenGL/Animations.h"

// WIP
void freeSkellyBoi(Bone* node)
{
    for (auto child : node->children)
        freeSkellyBoi(child);

    delete node;
}
Bone* addToSkellyBoi(std::vector<Bone*>* bones, FbxNode* node, unsigned int *index, glm::mat4 *parentMatrix)
{

    Bone* NBone = new Bone;
    //std::cout << "\t" << node->GetName() << "\n";


    NBone->name = node->GetName();
    NBone->ID = *index;
    NBone->Transform = (*parentMatrix) * extractTransformFromNode(node);
    (*index)++;
    bones->push_back(NBone);
    for (auto i = 0; i < node->GetChildCount(); i++)
    {
        NBone->children.push_back(addToSkellyBoi(bones, node->GetChild(i), index, &NBone->Transform));
        NBone->children[NBone->children.size() - 1]->parent = NBone;
    }
    return NBone;
    //Bone* newBone = new Bone;
    //newBone->name = pNode->GetName();
}
Bone* createSkellyboi(FbxNode* node, std::vector<Bone*>* bones)
{

    //TODO check if node is a bone

    Bone* SkellyBoi = new Bone;
    SkellyBoi->name = node->GetName();
    
    //////////////////////////////////////////////////////////////////
    SkellyBoi->Transform = extractTransformFromNode(node);
    //////////////////////////////////////////////////////////////////

    unsigned int index = 0;
    SkellyBoi->ID = index;
    index++;
    bones->push_back(SkellyBoi);
    for (auto i = 0; i < node->GetChildCount(); i++)
    {
        SkellyBoi->children.push_back(addToSkellyBoi(bones, node->GetChild(i), &index, &SkellyBoi->Transform));
        SkellyBoi->children[SkellyBoi->children.size() - 1]->parent = SkellyBoi;
    }
    return SkellyBoi;
}
// WIP