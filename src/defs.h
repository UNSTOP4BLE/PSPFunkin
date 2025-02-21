#pragma once

//graphics rendererers
#ifdef PSP
#define ENABLE_PSP_RENDERER
#else
#define ENABLE_OPENGL_RENDERER
#endif

//other
#define DEBUGLOG_MAX 128 //max lines for debug logging