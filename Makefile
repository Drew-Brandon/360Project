# Compiler
CXX = gcc

# Compiler flags
CXXFLAGS = -Wall -g -Wextra

# Target executable
TARGET = ./ez_filez.out

# For deleting the target
TARGET_DEL = ./ez_filez.out

# Source filesc
SRCS = ./main.c ./journaling.c ./file_utils.c ./search.c ./star_compare.c ./vfs.c

# Object files
OBJS = $(SRCS:.c=.o)

# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .cpp files into .o files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to run the executable
run: $(TARGET)
	$(TARGET)

# Clean rule to remove generated files
clean:
	rm $(TARGET_DEL) $(OBJS)
