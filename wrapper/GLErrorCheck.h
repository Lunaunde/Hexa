#pragma once

//预编译宏
#ifdef DEBUG
#define GL_CALL(func) func; GLErrorCheck();
#else
#define GL_CALL(func) func;
#endif

void GLErrorCheck();