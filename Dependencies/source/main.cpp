

#include "Common.h"


#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{

#pragma region LOADING
    // Change the following filename to a suitable filename value.
    const char* lFilename = "BOX.fbx";

    // Initialize the SDK manager. This object handles memory management.
    FbxManager* lSdkManager = FbxManager::Create();

    // Create the IO settings object.
    FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");


    // Use the first argument as the filename for the importer.
    if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
        exit(-1);
    }
    // Create a new scene so that it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    FbxGeometryConverter conv(lSdkManager);

    conv.Triangulate(lScene, true);

    // The file is imported, so get rid of the importer.
    lImporter->Destroy();

    // Print the nodes of the scene and their attributes recursively.
    // Note that we are not printing the root node because it should
    // not contain any attributes.


    std::vector<Mesh*> Meshes;

    FbxNode* lRootNode = lScene->GetRootNode();
    if (lRootNode) 
        for (int i = 0; i < lRootNode->GetChildCount(); i++)
            if (lRootNode->GetChild(i)->GetMesh())
            {
                Meshes.push_back(getMeshData(lRootNode->GetChild(i)));
            }
    // Destroy the SDK manager and all the other objects it was handling.
    lSdkManager->Destroy();
#pragma endregion 

#pragma region OPENGL
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    int w = 1000;
    int h = w;
    GLFWwindow* window = glfwCreateWindow(w, h, "cow.exe", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }
    else std::cout << "OpenGL was initialized just fine\n";


    glfwSwapInterval(1);


    int width = 500, height = 500;

    glfwGetWindowSize(window, &width, &height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

 
    Shader* shader = new Shader("Dependencies/shaders/vertex.glsl", "Dependencies/shaders/fragment.glsl");
    shader->bind();

   
    std::cout << "Num of Meshes: " << Meshes.size() << "\n";
    for (auto mesh : Meshes)
        mesh->prepare();

    
    Camera cam(window);

    glm::mat4* proj, * view;

    glm::mat4 model = glm::mat4(1);
    shader->setMat4("model", &model);

    while (!glfwWindowShouldClose(window))
    {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        cam.Move(window);
        view = cam.getviewmatrix();
        proj = cam.getprojmatrix();
        shader->setMat4("view", view);
        shader->setMat4("proj", proj);

        for (auto mesh : Meshes)
            mesh->Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
            break;
    }   

    shader->unbind();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete shader;

#pragma endregion
    return 0;
}