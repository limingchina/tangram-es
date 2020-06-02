#include "App.h"
#include "linuxPlatform.h"
#include "log.h"
#include "map.h"
#include <memory>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

using namespace Tangram;

int main(int argc, char* argv[]) {

    // Create the windowed app.
    App::create(std::make_unique<LinuxPlatform>(), 1024, 768);

    App::sceneFile = "res/scene.yaml";
    App::parseArgs(argc, argv);

    // Resolve the input path against the current directory.
    Url baseUrl("file:///");
    char pathBuffer[PATH_MAX] = {0};
    if (getcwd(pathBuffer, PATH_MAX) != nullptr) {
        baseUrl = Url(std::string(pathBuffer) + "/").resolved(baseUrl);
    }
    
    LOG("Base URL: %s", baseUrl.string().c_str());
    
    Url sceneUrl = Url(App::sceneFile).resolved(baseUrl);
    App::sceneFile = sceneUrl.string();

    // Give it a chance to shutdown cleanly on CTRL-C
    signal(SIGINT, &App::stop);

    // Loop until the user closes the window
    App::run();

    // Clean up.
    App::destroy();

}
