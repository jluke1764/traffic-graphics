#pragma once
#include <GL/glew.h>
#include <iostream>

namespace Debug
{
// Task 2: Add file name and line number parameters
inline void glErrorCheck(const char* filename, int line_num) {
    GLenum errorNumber = glGetError();
    while (errorNumber != GL_NO_ERROR) {
        // Task 2: Edit this print statement to be more descriptive
        std::cout << "filename: " << filename << ", line number: " << line_num << ", error number: " << errorNumber << std::endl;

        errorNumber = glGetError();
    }
}
}

// TASK 3: Add a preprocessor directive to automate the writing of this function
#define identifier replacement
#define glErrorCheck() Debug::glErrorCheck(__FILE__, __LINE__)
