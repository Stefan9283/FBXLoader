#include "Utils.h"


// CONVERT FUNC
void printFbxDouble3(FbxDouble3 v)
{
    std::cout << v[0] << " " << v[1] << " " << v[2] << "\n";
}
glm::vec3 FbxDouble3tovec3(FbxDouble3 v)
{
    return glm::vec3(v[0], v[1], v[2]);
}

glm::mat4 extractTransformFromNode(FbxNode* node)
{

    FbxAMatrix geometricMatrix;
    geometricMatrix.SetIdentity();

    FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 scaling = node->GetGeometricScaling(FbxNode::eSourcePivot);
    geometricMatrix.SetT(translation);
    geometricMatrix.SetR(rotation);
    geometricMatrix.SetS(scaling);

    FbxAMatrix localMatrix = node->EvaluateLocalTransform();

    FbxNode* pParentNode = node->GetParent();
    FbxAMatrix parentMatrix;

    FbxAMatrix matrix;

    if (pParentNode)
    {
        parentMatrix = pParentNode->EvaluateLocalTransform();
        while ((pParentNode = pParentNode->GetParent()) != NULL)
        {
            parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
        }

    }
    else parentMatrix.SetIdentity();


    matrix = parentMatrix * localMatrix * geometricMatrix;

    translation = matrix.GetT();
    scaling = matrix.GetS();
    rotation = matrix.GetR();

    glm::mat4 T, R1, R2, R3, S;

    T = glm::translate(glm::mat4(1), glm::vec3(translation[0], translation[1], translation[2]));

    R1 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    R2 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    R3 = glm::rotate(glm::mat4(1), (float)glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    S = glm::scale(glm::mat4(1), glm::vec3(scaling[0], scaling[1], scaling[2]));

    glm::mat4 transform_matrix = T * R2 * R3 * R1 * S;

    return transform_matrix;
    //std::cout << glm::to_string(transform_matrix) << "\n";

}


double findModulo(double a, double b)
{
    double mod;
    if (a < 0)
        mod = -a;
    else
        mod = a;
    if (b < 0)
        b = -b;


    while (mod >= b)
        mod = mod - b;

    if (a < 0)
        return -mod;

    return mod;
}
float interpolateValue(std::vector<std::pair<float, signed long>> keyfr, double time)
{
    switch (keyfr.size())
    {
    case 0:
        return 0;
    case 1:
        return keyfr[0].first;
    default:
        int index = 0;

        std::cout << "debug " << keyfr.size() - 1 << "\n\n";

        time = findModulo(time, keyfr[keyfr.size() - 1].second);
        std::cout << "debug " << "\n\n";

        for (auto i = 0; i < keyfr.size(); i++)
        {
            if (time >= keyfr[i].second)
            {
                index = i - 1;
            }
            else break;

        }

        if (index < 0)
            index = 0;

        std::cout << "debug " << "\n\n";

        assert(index < keyfr.size());

        std::cout << index << "\n\n";

        float n = (time - keyfr[index].second) / (keyfr[index + 1].second - keyfr[index].second);

        assert(n >= 0);

        return (1 - n) * keyfr[index].first + keyfr[index + 1].first * n;
    }

    
}