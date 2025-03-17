#pragma once

//‘§±‡“Î∫Í
#ifdef DEBUG
#define GL_CALL(func) func; GLErrorCheck();
#else
#define GL_CALL(func) func;
#endif

void GLErrorCheck();