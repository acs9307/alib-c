# Dockerfile for alib-c development and testing
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libjson-c-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace/alib-c

# Copy project files
COPY . .

# Build the library and tests
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make

# Set the default command to run tests
CMD ["sh", "-c", "cd build && ctest --output-on-failure --verbose"]

# Alternative: For interactive development
# CMD ["/bin/bash"]
