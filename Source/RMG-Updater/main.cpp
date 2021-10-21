/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <filesystem>
#include <unistd.h>

void usage(char** argv)
{
    std::cout << "Usage: \n" <<
                    argv[0] << 
                    " [extracted update data directory]" <<
                    " [target directory]" <<
                    " [target executable]" <<
                    std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        usage(argv);
        return 1;
	}

    auto source = argv[1];
    auto target = argv[2];
    auto exe    = argv[3];

    for (auto& str : {source, target, exe})
    {
        if (!std::filesystem::exists(str))
        {
            std::cout << str << " is not a file or directory!" << std::endl;
            usage(argv);
            return 1;
        }
    }

    // copy source directory to target
    auto options = std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive;
    std::filesystem::copy(source, target, options);

    // launch executable and exit
    pid_t pid = fork();
    if (pid == 0)
    { // only execute exe in child
        system(exe);
    }

    return 0;
}
