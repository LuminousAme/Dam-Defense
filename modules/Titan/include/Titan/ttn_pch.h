#pragma once

//data stuff
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>

//functionality
#include <memory>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <filesystem>
#include "Logging.h"

//math
#include "GLM/glm.hpp"
#include "glm/common.hpp"
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>
//allow use of experimental glm features
#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/gtx/quaternion.hpp"
#include "GLM/gtx/transform.hpp"

//API
#include <glad/glad.h>
#include <stb_image.h>
#include "entt.hpp"