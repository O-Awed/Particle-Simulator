# 1. Use Ubuntu as the starting point
FROM ubuntu:22.04

# 2. Avoid prompts from apt during installation
ENV DEBIAN_FRONTEND=noninteractive

# 3. Install C++ compilers, CMake, and common OpenGL libraries
# (I included the most common ones: GLFW, GLEW, GLUT, GLM. 
#  Docker will just ignore the ones you don't use.)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    libglfw3-dev \
    libglew-dev \
    libglm-dev \
    && rm -rf /var/lib/apt/lists/*

# 4. Set the working directory inside the container
WORKDIR /app

# 5. Copy your source code from your computer into the container
COPY . /app

# 6. Compile the code 
# Note: You might need to change 'main.cpp' to your actual filename.
# This is a generic compilation command for OpenGL.
RUN g++ -o particle_sim *.cpp -lGL -lGLU -lglut -lGLEW -lglfw

# 7. The command to run when the container starts
CMD ["./particle_sim"]