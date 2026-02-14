# 1. Use Ubuntu 22.04
FROM ubuntu:22.04

# 2. Avoid prompts
ENV DEBIAN_FRONTEND=noninteractive

# 3. Install specific dependencies matching your main.cpp
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3-pip \
    libglfw3-dev \
    libglm-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# 4. Install the GLAD generator
RUN pip3 install glad

# 5. Set working directory
WORKDIR /app

# 6. Generate GLAD files
RUN glad --generator=c --api="gl=3.3" --profile=compatibility --out-path=glad

# 7. Copy your source code
COPY . /app

# 8. Compile the code
RUN g++ -o particle_sim *.cpp glad/src/glad.c -I./glad/include -lglfw -lGL -ldl

# 9. Run
CMD ["./particle_sim"]