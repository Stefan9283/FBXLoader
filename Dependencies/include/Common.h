#pragma once

#if defined(WIN32)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define OPENGL 1

#include <stdlib.h>

#include <iostream>
#include <vector>

#include <iostream>
#include <vector>
#include <future>   // std::async, std::future
#include <thread>
#include <chrono>
#include <string>
#include <string.h>
#include <stdio.h>
#include <limits>  // std::numeric_limits


#include <unordered_map>
#include <map>
#include <deque>
#include <stack>
#include <queue>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/string_cast.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/quaternion.hpp"


#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "fbxsdk.h"
#include "fbxsdk/utils/fbxgeometryconverter.h"

#include "stb_image.h"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "OpenGL/Structs.h"
#include "OpenGL/Debug.h"

#define SHOWLOADEDTEXFEEDBACK false
#define SHOWTEXTUREFEEDBACK false
#define SHOWPREPAREFEEDBACK false

#include "OpenGL/Window.h"
#include "Utils.h"
