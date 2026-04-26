## Project Overview & Goals
- We set out to create a virtual file management system for easy access and use by more casual developers that still wanted experience using a terminal. 

## Themes Covered
File System/Management 
- Made a virtual file system managed by the user to easily interact and navigate directories
I/O Systems 
Journaling
- Created a journaling system to log methods called and to catch errors
Control Flow

## Design Decisions & Tradeoffs
- Needed to decide how the search function would work and what it would return. Use cases including knowing where a file was and also accessing the file. Ended up creating two functions for both cases where one will return a list of paths from the current directory to all matching directories and files. The other function returns a list of virtual file nodes that contain all matching directories and files for easy access.

## Challenges & Lessons
- Had challenges figuring out history system. Wanted to implement arrow navigation but couldn't with standard C input. Ended up switching to a terminal system where `hs` displays the 10 most recent commands and `hs x` with `x` being an integer 1-10 reruns the command chosen. 

- Learned more about the C programming language and how the underlying systems of file navigation and mangement work
- Learned more about collaborating with others on software projects
- Learned about macros and virtual file systems
