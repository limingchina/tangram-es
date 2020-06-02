#include "App.h"

#include <SDL2/SDL.h>
#define GLAD_GLES2_IMPLEMENTATION
#include "glad_gles2.h"

namespace Tangram {

namespace App {

Tangram::Map* map = nullptr;
SDL_Window* sdlWindow;

void create(std::unique_ptr<Platform> p, int w, int h) {

    SDL_Init(SDL_INIT_VIDEO);

    sdlWindow =
        SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GLContext sdlContext = SDL_GL_CreateContext( sdlWindow );
    LOGD("SDL context: %p", sdlContext);
    SDL_GL_MakeCurrent(sdlWindow, sdlContext);

    int version = gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
    LOGD("GL version: %d", version);

    char* nextzenApiKeyEnvVar = getenv("NEXTZEN_API_KEY");
    if (nextzenApiKeyEnvVar && strlen(nextzenApiKeyEnvVar) > 0) {
        apiKey = nextzenApiKeyEnvVar;
    } else {
        LOGW("No API key found!\n\nNextzen data sources require an API key. "
             "Sign up for a key at https://developers.nextzen.org/about.html and then set it from the command line with: "
             "\n\n\texport NEXTZEN_API_KEY=YOUR_KEY_HERE"
             "\n\nOr, if using an IDE on macOS, with: "
             "\n\n\tlaunchctl setenv NEXTZEN_API_KEY YOUR_API_KEY\n");
    }

    if (!apiKey.empty()) {
        sceneUpdates.push_back(SceneUpdate(apiKeyScenePath, apiKey));
    }

    // Setup tangram
    if (!map) {
        map = new Tangram::Map(std::move(p));
    }

    // Setup graphics
    map->setupGL();

}

void run() {

    loadSceneFile(true);

    SDL_Event e;
    while(true)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) std::terminate();
        }

        // Render
        MapState state = map->update(10);
        if (state.isAnimating()) {
            map->getPlatform().requestRender();
        }

        map->render();

        SDL_GL_SwapWindow(sdlWindow);
    }
    /*
    double lastTime = glfwGetTime();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(main_window)) {

        if(show_gui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Create ImGui interface.
            // ImGui::ShowDemoWindow();
            showSceneGUI();
            showViewportGUI();
            showMarkerGUI();
            showDebugFlagsGUI();
        }
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;

        // Render
        MapState state = map->update(delta);
        if (state.isAnimating()) {
            map->getPlatform().requestRender();
        }

        const bool wireframe = wireframe_mode;
        if(wireframe) {
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
        }
        map->render();
        if(wireframe) {
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
        }

        if (show_gui) {
            // Render ImGui interface.
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        // Swap front and back buffers
        glfwSwapBuffers(main_window);

        // Poll for and process events
        if (map->getPlatform().isContinuousRendering()) {
            glfwPollEvents();
        } else {
            glfwWaitEvents();
        }
    }
    */
}

void stop(int) {
    exit(1);
}

void destroy() {
    if (map) {
        delete map;
        map = nullptr;
    }
}

} // namespace App
} // namespace Tangram
