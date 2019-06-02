#include <chrono>
#include <stdio.h>

#include "config.h"
#include "gameplay.h"
#include "log.h"
#include "visualisation.h"

using std::string;
using namespace std::chrono;

int main(int argc, char **argv)
{
    Log log("main");
    log.Info() << "3D tetris";

    Config::inst().Load(argc, argv);

    auto config_path = Config::inst().GetOption<std::string>("config");
    if (config_path != "")
    {
        Config::inst().Load(config_path);
    }

    LoggingSingleton::inst().SetConsoleVerbosity(
        Config::inst().GetOption<bool>("verbose"));
    LoggingSingleton::inst().AddLogFile(
        Config::inst().GetOption<std::string>("log_file"));

    Config::inst().DumpSettings();

    //====================

    auto block_start_time = high_resolution_clock::now();

    Visualisation vis;
    Gameplay gameplay(vis);

    bool exit_requested = false;
    while (!exit_requested)
    {
        auto time = std::chrono::high_resolution_clock::now();
        float running_time =
            float(duration_cast<std::chrono::milliseconds>(time - block_start_time)
                      .count()) /
            1000.0f;

        vis.Render(running_time);

        while (auto action = vis.DequeueAction())
        {
            switch (*action)
            {
            case Visualisation::Exit:
                exit_requested = true;
                break;
            default:
                gameplay.HandleAction(*action, running_time);
            }
        }

        gameplay.Update(running_time);
    }

    log.Info() << "Exit requested. Bye, bye.";
}