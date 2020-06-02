#include "App.h"

namespace Tangram {

namespace App {

std::string sceneFile = "scene.yaml";
std::string sceneYaml;
std::string apiKey;
bool load_async = true;

std::vector<SceneUpdate> sceneUpdates;
const char* apiKeyScenePath = "global.sdk_api_key";

void loadSceneFile(bool setPosition, std::vector<SceneUpdate> updates) {

    for (auto& update : updates) {
        bool found = false;
        for (auto& prev : sceneUpdates) {
            if (update.path == prev.path) {
                prev = update;
                found = true;
                break;
            }
        }
        if (!found) { sceneUpdates.push_back(update); }
    }

    if (load_async) {
        if (!sceneYaml.empty()) {
            map->loadSceneYamlAsync(sceneYaml, sceneFile, setPosition, sceneUpdates);
        } else {
            map->loadSceneAsync(sceneFile, setPosition, sceneUpdates);
        }
    } else {
        if (!sceneYaml.empty()) {
            map->loadSceneYaml(sceneYaml, sceneFile, setPosition, sceneUpdates);
        } else {
            map->loadScene(sceneFile, setPosition, sceneUpdates);
        }
    }
}

void parseArgs(int argc, char* argv[]) {
    // Load file from command line, if given.
    int argi = 0;
    while (++argi < argc) {
        if (strcmp(argv[argi - 1], "-f") == 0) {
            sceneFile = std::string(argv[argi]);
            LOG("File from command line: %s\n", argv[argi]);
            break;
        }
        if (strcmp(argv[argi - 1], "-s") == 0) {

            if (argi+1 < argc) {
                sceneYaml = std::string(argv[argi]);
                sceneFile = std::string(argv[argi+1]);
                LOG("Yaml from command line: %s, resource path: %s\n",
                    sceneYaml.c_str(), sceneFile.c_str());
            } else {
                LOG("-s options requires YAML string and resource path");
                exit(1);
            }
            break;
        }
    }
}

void setScene(const std::string& _path, const std::string& _yaml) {
    sceneFile = _path;
    sceneYaml = _yaml;
}

}
}
