#include "Debug.h"

// DEBUG FUNC
void addTabs(int count)
{
    for (auto i = 0; i < count; i++)
    {
        std::cout << "\t";
    }
}
void printNode(FbxNode* node, int level)
{
    addTabs(level);            std::cout << node->GetName();

    std::cout << " " << node->GetNodeAttributeCount() << ": " << "\n";

    for (auto i = 0; i < node->GetNodeAttributeCount(); i++)
    {
        if (node->GetNodeAttributeByIndex(i)->GetAttributeType() == 4)
        {

            FbxMesh* nodeAttrib = (FbxMesh*)node->GetNodeAttributeByIndex(i);

            FbxLayerElementArrayTemplate<int>* arr;
            nodeAttrib->GetMaterialIndices(&arr);

            FbxSurfacePhong* phong_material = (FbxSurfacePhong*)nodeAttrib->GetNode()->GetSrcObject<FbxSurfacePhong>(arr->GetAt(0));
            FbxDouble3 diff = phong_material->Diffuse.Get();

        }
    }







    std::cout << "\n";
    /*
    for (auto j = 0; j < node->GetNodeAttributeCount(); j++)
    {
        std::cout << " " << node->GetNodeAttributeByIndex(j)->GetAttributeType() << " ";
        if (node->GetNodeAttributeByIndex(j)->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            std::cout << "Was ist das? " << node->GetNodeAttributeByIndex(j)->GetNodeCount() << " ";
            std::cout << "Was ist das? " << node->GetNodeAttribute()->GetNodeCount() << " ";
            //std::cout << " triangles " << node->GetMesh()->GetPolygonCount() << " triangles ";

            //std::cout << node->GetName();
        }
    }
    std::cout << "\n";
    */

    for (auto i = 0; i < node->GetChildCount(); i++)
    {
        printNode(node->GetChild(i), level + 1);
    }
}

