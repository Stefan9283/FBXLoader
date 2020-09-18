#include <iostream>

#include <fbxsdk.h>


int main()
{
    // Change the following filename to a suitable filename value.
    const char* lFilename = "file.fbx";

    // Initialize the SDK manager. This object handles memory management.
    FbxManager* lSdkManager = FbxManager::Create();

    return 0;
}