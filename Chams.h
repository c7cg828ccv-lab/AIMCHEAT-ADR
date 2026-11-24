/*
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
*/

#ifndef HENZRY_CHAMS
#define HENZRY_CHAMS

#include <GLES2/gl2.h>
#include <dlfcn.h>

static void *handle;

bool chams = false;
static const char* shaderName;

void setShader(const char* s) {
    shaderName = s;
}
const char* getShader() {
    return shaderName;
}

bool getWallhackEnabled(){
    return chams;
}

int (*old_glGetUniformLocation)(GLuint, const GLchar *);
GLint new_glGetUniformLocation(GLuint program, const GLchar *name) {
    return old_glGetUniformLocation(program, name);
}

void (*old_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);
void new_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
    old_glDrawElements(mode, count, type, indices);
    if (mode != GL_TRIANGLES || count < 1000) return;
    {
        GLint currProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

        GLint id = old_glGetUniformLocation(currProgram, getShader());
        if (id == -1) return;

        if (getWallhackEnabled()) {
            // Waller arpar dekhar jonno depth range
            glDepthRangef(1.0f, 0.5f);
            // Full Ujjwal Pink Color (Bright Pink: R=255, G=20, B=147, A=255)
            glBlendColor(1.0f, 0.078f, 0.576f, 1.0f);
            glColorMask(1, 1, 1, 1);
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_COLOR);
        }
                old_glDrawElements(mode, count, type, indices);

        glDepthRangef(0.5, 1);

        glColorMask(1, 1, 1, 1);
        glDisable(GL_BLEND);
    }
}

bool mlovinit(){
    handle = NULL;
    handle = dlopen("libGLESv2.so", RTLD_LAZY);
    if (!handle) {
        return false;
    }
    return true;
}

void LogShaders(){
    auto p_glGetUniformLocation = (const void*(*)(...))dlsym(handle, "glGetUniformLocation");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        return;
    } else {
        #if defined(__aarch64__)
            A64HookFunction(reinterpret_cast<void*>(p_glGetUniformLocation), reinterpret_cast<void*>(new_glGetUniformLocation), reinterpret_cast<void**>(&old_glGetUniformLocation));
        #else
            MSHookFunction(reinterpret_cast<void*>(p_glGetUniformLocation), reinterpret_cast<void*>(new_glGetUniformLocation), reinterpret_cast<void**>(&old_glGetUniformLocation));
        #endif
    }
}

void Wallhack() {
    auto p_glDrawElements = (const void*(*)(...))dlsym(handle, "glDrawElements");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        return;
    } else {
        #if defined(__aarch64__)
            A64HookFunction(reinterpret_cast<void*>(p_glDrawElements), reinterpret_cast<void*>(new_glDrawElements), reinterpret_cast<void**>(&old_glDrawElements));
        #else
            MSHookFunction(reinterpret_cast<void*>(p_glDrawElements), reinterpret_cast<void*>(new_glDrawElements), reinterpret_cast<void**>(&old_glDrawElements));
        #endif
    }
}

#endif

/*
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
===============================[ 𝗠𝗔𝗗𝗘 𝗕𝗬 @𝗕𝗗_𝗛𝗔𝗖𝗞𝗘𝗥_𝗢𝗙𝗙𝗜𝗖𝗜𝗔𝗟_𝗣𝗥𝗜𝗩𝗔𝗧𝗘_𝗦𝗥𝗖 ]============================
*/
