# Docker Setup for alib-c

This document describes how to use Docker for building and testing alib-c in a containerized environment.

## Prerequisites

- Docker installed on your system
- Docker Compose (optional, but recommended)

## Quick Start

### Using Docker Compose (Recommended)

**Run tests:**
```bash
docker-compose up alib-c-test
```

**Interactive development shell:**
```bash
docker-compose run --rm alib-c-dev
```

### Using Docker Directly

**Build the Docker image:**
```bash
docker build -t alib-c:latest .
```

**Run tests:**
```bash
docker run --rm alib-c:latest
```

**Interactive shell:**
```bash
docker run --rm -it -v $(pwd):/workspace/alib-c alib-c:latest /bin/bash
```

## Docker Image Contents

The Docker image includes:
- Ubuntu 22.04 base
- Build tools (gcc, make, cmake)
- All dependencies:
  - libjson-c-dev (for ErrorLogger)
  - zlib1g-dev (for compression support)
- Pre-built alib-c library and tests

## Usage Examples

### Building the Library

```bash
# Using Docker Compose
docker-compose run --rm alib-c-dev sh -c "mkdir -p build && cd build && cmake .. && make"

# Using Docker
docker run --rm -v $(pwd):/workspace/alib-c alib-c:latest sh -c "cd /workspace/alib-c && mkdir -p build && cd build && cmake .. && make"
```

### Running All Tests

```bash
# Using Docker Compose
docker-compose up alib-c-test

# Using Docker
docker run --rm alib-c:latest
```

### Running a Specific Test

```bash
docker-compose run --rm alib-c-dev sh -c "cd build && ./tests/test_ArrayList"
```

### Development Workflow

1. Start an interactive container:
```bash
docker-compose run --rm alib-c-dev
```

2. Inside the container:
```bash
# Build
mkdir -p build && cd build
cmake ..
make

# Run tests
ctest --output-on-failure

# Run specific test
./tests/test_String

# Rebuild after changes
make
```

3. Exit when done:
```bash
exit
```

## Continuous Integration

The Dockerfile can be used in CI/CD pipelines:

### GitHub Actions Example

```yaml
name: CI

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2

      - name: Build Docker image
        run: docker build -t alib-c:test .

      - name: Run tests
        run: docker run --rm alib-c:test
```

### GitLab CI Example

```yaml
test:
  image: docker:latest
  services:
    - docker:dind
  script:
    - docker build -t alib-c:test .
    - docker run --rm alib-c:test
```

## Customization

### Building Without Tests

Edit `Dockerfile` and change the build command:
```dockerfile
RUN mkdir -p build && \
    cd build && \
    cmake -DBUILD_TESTS=OFF .. && \
    make
```

### Adding Additional Dependencies

Edit `Dockerfile` and add to the apt-get install command:
```dockerfile
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libjson-c-dev \
    zlib1g-dev \
    your-package-here \
    && rm -rf /var/lib/apt/lists/*
```

## Troubleshooting

### Permission Issues

If you encounter permission issues with volumes:
```bash
# Run with your user ID
docker run --rm -u $(id -u):$(id -g) -v $(pwd):/workspace/alib-c alib-c:latest
```

### Build Cache Issues

Clear the Docker build cache:
```bash
docker builder prune -a
```

Rebuild without cache:
```bash
docker build --no-cache -t alib-c:latest .
```

### Volume Issues

Remove and recreate volumes:
```bash
docker-compose down -v
docker-compose up alib-c-test
```

## Performance Tips

1. **Use BuildKit for faster builds:**
   ```bash
   DOCKER_BUILDKIT=1 docker build -t alib-c:latest .
   ```

2. **Use multi-stage builds** to reduce final image size (advanced)

3. **Mount build directory as volume** to preserve build artifacts between runs:
   ```bash
   docker run --rm -v $(pwd):/workspace/alib-c -v alib-c-build:/workspace/alib-c/build alib-c:latest
   ```

## Cleaning Up

Remove all containers and images:
```bash
docker-compose down
docker rmi alib-c:latest
```

Remove volumes:
```bash
docker-compose down -v
```

## Notes

- The `.dockerignore` file prevents unnecessary files from being copied into the image
- Build artifacts are stored in the `build/` directory
- The container runs as root by default; adjust as needed for your security requirements
- All dependencies are pinned to specific versions in the Dockerfile for reproducibility
