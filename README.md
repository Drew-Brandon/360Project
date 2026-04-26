## Project Overview & Goals
- The main goal was to make a basic, easy to use, easy to analyze file system. The latter meaning creating a file system that could be saved to disc with a realtively simplistic format similar to that of JSON.
- A secondary goal was to make a basic yet solid exception and journaling system. This system is not used extensively in the end, but we feel that its better used for file formatting errors than those of command syntax.

## Themes Covered
File System/Management 
- Made a virtual file system managed by the user to easily interact and navigate directories.
- Added support for the * expression to allow for more flexible specifications for commands.
- Added support for moving and copying files in a system.
I/O Systems 
- Made a system to scan in and print out files to and from the file system
- Made a system to save and load the file system to and from a file of a specific format similar to that of JSON.
Journaling
- Created a journaling system to log methods called and to catch errors.
Control Flow
- Created an exception system that is able to jump the code safely back to a specific place using the setjmp library in C.

## Design Decisions & Tradeoffs
- Needed to decide how the search function would work and what it would return. Use cases including knowing where a file was and also accessing the file. Ended up creating two functions for both cases where one will return a list of paths from the current directory to all matching directories and files. The other function returns a list of virtual file nodes that contain all matching directories and files for easy access.
- Made a basic format that was both easy to read by humans and C's built in scanning functions.
- Made an array list library that can be templated to work for any specified type.

## Challenges & Lessons
- Had challenges figuring out history system. Wanted to implement arrow navigation but couldn't with standard C input. Ended up switching to a terminal system where `hs` displays the 10 most recent commands and `hs x` with `x` being an integer 1-10 reruns the command chosen. 
- Had issues with team management, and being able to get everyone together on things. Biggest challenge at first was on what the project was going to be, and the scope that we should go for.
- Learned more about the C programming language and how the underlying systems of file navigation and mangement work.
- Learned about macros and virtual file systems.

## How to use
- Compile with make command, and run with ./ez_filez.out.
- Note that one can specific a file to load a system from, by stating something like ./ez_filez.out <insert_path_to_file_here>. Keep in mind this file must have the correct formatting to be properly loaded in. Example: ./ez_filez.out systems/dog_data.ez
- The * symbol can also be used like how it is in regular expressions.
- ls <to_list>: Lists out the objects in the given directory whose names match the given pattern at the end. Example: ls my_dir/*.txt
- cd <path>: Changes the current directory to the given path. Example: cd my_dir/foo/
- cat <to_cat>: Displays the contents of the files in the given directory whose names match the given pattern at the end. Example cat my_dir/*.txt
- pwd: Displays the current working directory.
- save <save_path>: Saves the system to the specified path. Example save vfs/dogs.ez
- offload <offload_path>: Offloads the files in the given directory whose names match the given pattern at the end to the local system. Example: offload foo.txt
- mkdir <new_dir>: Creates the new directory at the specified path. Example: mkdir my_dir/new_dir
- rm <to_remove>: Removes the files in the given directory whose names match the given pattern at the end. Example: rm *.txt
- rmdir <to_remove>: Removes the directories in the given directory whose names match the given pattern at the end. Note that if a directory has children, confirmation will be needed from the user to remove said directory. Example: rmdir my_dir
- mv <to_move>: Moves the given file to the specified location. Example: mv foo.txt bar.txt
- mvdir <to_move>: Moves the given directory to the specified location. Example: mvdir my_dir trash/my_dir
- cp <to_copy>: Copies the given file to the specified position. Example: cp foo.txt backups/foo_back.txt
- hs (or history): Lists the current history of commands, from the earliest to the latest.
- hs <command_index>: Uses the command at the given index in history. Example: hs 3
