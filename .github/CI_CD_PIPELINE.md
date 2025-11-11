# CI/CD Pipeline Documentation

This document describes the Continuous Integration and Continuous Deployment (CI/CD) pipeline for alib-c.

## Overview

The alib-c project uses GitHub Actions for automated building, testing, and quality assurance. The pipeline ensures that all code changes meet quality standards before being merged into the main branch.

## Pipeline Workflow

The CI pipeline is defined in `.github/workflows/ci.yml` and runs automatically on:
- Pushes to `main`, `master`, or `develop` branches
- Pull requests targeting these branches

## Pipeline Jobs

### 1. Build with CMake (`build-cmake`)

**Purpose:** Verify that the project builds correctly with CMake on multiple configurations.

**Runs on:**
- Ubuntu 20.04 and Ubuntu 22.04
- Debug and Release build types
- Total: 4 build matrix combinations

**Steps:**
1. Install build dependencies (cmake, build-essential, doxygen)
2. Install library dependencies (json-c, zlib via `scripts/install_depends.sh`)
3. Configure CMake with strict compiler flags (`-Wall -Wextra -Werror`)
4. Build the library
5. Install the library to system directories
6. Verify installation success

**Compiler Flags:**
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable extra warnings
- `-Werror`: Treat warnings as errors (ensures warning-free code)

### 2. Build with Make (`build-make`)

**Purpose:** Ensure backward compatibility with the legacy Makefile build system.

**Runs on:** Ubuntu 22.04

**Steps:**
1. Install build dependencies
2. Install library dependencies
3. Build with Make
4. Verify `libalibc.a` is created
5. Test clean target

### 3. Build Examples (`build-examples`)

**Purpose:** Verify that all example programs compile and link correctly against the library.

**Runs on:** Ubuntu 22.04

**Steps:**
1. Build and install the main library
2. Compile all `.c` files in the `examples/` directory
3. Link against installed library with required dependencies

**Dependencies linked:**
- `-lalibc`: The main library
- `-lpthread`: Threading support
- `-lm`: Math library
- `-ljson-c`: JSON support
- `-lz`: Compression support

### 4. Static Analysis (`static-analysis`)

**Purpose:** Perform static code analysis to catch potential bugs and style issues.

**Runs on:** Ubuntu 22.04

**Tools used:**
- **cppcheck**: Comprehensive C/C++ static analyzer
- **Custom checks**: Formatting and header guard validation

**Checks performed:**
- Warning detection (style, performance, portability)
- Trailing whitespace detection
- Header guard verification (informational)

**Note:** Currently set to report issues but not fail the build. This can be adjusted to enforce stricter standards.

### 5. Pipeline Success (`pipeline-success`)

**Purpose:** Final gate that depends on all other jobs passing.

**Runs on:** Ubuntu 22.04

**Requirements:**
- All build-cmake matrix jobs must pass
- build-make must pass
- build-examples must pass
- static-analysis must pass

This job serves as the single status check that branch protection rules can require.

## Status Badges

You can add the following badge to your README.md to show pipeline status:

```markdown
![CI Status](https://github.com/YOUR_USERNAME/alib-c/workflows/CI%20Build%20and%20Test/badge.svg)
```

Replace `YOUR_USERNAME` with the actual GitHub username or organization name.

## Local Development Workflow

### Before Pushing

To ensure your changes will pass CI, run these commands locally:

```bash
# 1. Build with CMake (Debug)
mkdir -p build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-Wall -Wextra -Werror"
cmake --build .
cd ..

# 2. Build with CMake (Release)
mkdir -p build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-Wall -Wextra -Werror"
cmake --build .
cd ..

# 3. Build with Make
make clean
make

# 4. Static analysis (if cppcheck is installed)
cppcheck --enable=warning,style,performance,portability \
  --suppress=missingIncludeSystem \
  source/ includes/
```

### Understanding Build Failures

If CI fails, check the workflow logs:
1. Go to the "Actions" tab in GitHub
2. Click on the failed workflow run
3. Expand the failed job
4. Review the error output

Common issues:
- **Compiler warnings/errors**: Fix code issues highlighted in build logs
- **Example compilation failure**: API changes may have broken examples
- **Static analysis issues**: Address cppcheck warnings

## Continuous Improvement

### Adding New Checks

To add new checks to the pipeline:

1. Edit `.github/workflows/ci.yml`
2. Add a new job or modify existing ones
3. Test locally if possible
4. Create a PR to verify the changes work
5. Update branch protection rules to require new status checks

### Adjusting Build Matrix

The build matrix can be expanded to test:
- More Ubuntu versions
- Different compiler versions (GCC, Clang)
- Additional build configurations
- Cross-platform builds (if supported)

Example:
```yaml
strategy:
  matrix:
    os: [ubuntu-20.04, ubuntu-22.04, ubuntu-24.04]
    compiler: [gcc, clang]
    build_type: [Debug, Release]
```

### Enforcing Stricter Standards

To make static analysis failures block merging:

1. In `.github/workflows/ci.yml`, change:
   ```yaml
   --error-exitcode=0
   ```
   to:
   ```yaml
   --error-exitcode=1
   ```

2. This will cause the job to fail if cppcheck finds issues

## Performance Considerations

Current pipeline runtime: ~10-15 minutes (estimated)

**Optimization opportunities:**
- Cache dependencies between runs
- Run jobs in parallel (already implemented)
- Use faster runners (GitHub-hosted are standard)

### Caching Example

To speed up dependency installation, add caching:

```yaml
- name: Cache dependencies
  uses: actions/cache@v3
  with:
    path: |
      /usr/local/lib
      /usr/local/include
    key: ${{ runner.os }}-deps-${{ hashFiles('scripts/install_depends.sh') }}
```

## Security Considerations

- Dependencies are built from source via `install_depends.sh`
- No external binaries are downloaded
- All actions use pinned versions (e.g., `@v4`)
- Dependabot keeps action versions up to date

## Troubleshooting

### Workflow Not Triggering

**Problem:** Push or PR doesn't trigger CI

**Solutions:**
- Verify branch name matches workflow triggers
- Check `.github/workflows/ci.yml` exists on the branch
- Ensure GitHub Actions is enabled in repository settings

### Permission Errors

**Problem:** `sudo: command not found` or permission denied

**Solutions:**
- GitHub Actions runners have sudo available
- If using self-hosted runners, ensure proper permissions

### Dependency Installation Failures

**Problem:** `install_depends.sh` fails

**Solutions:**
- Check if external repositories (json-c, zlib) are accessible
- Verify apt-get can reach package repositories
- Review install script for errors

## Integration with Development Tools

### Pre-commit Hooks

Consider adding local pre-commit hooks to catch issues before pushing:

```bash
#!/bin/bash
# .git/hooks/pre-commit

# Run static analysis
cppcheck --quiet --error-exitcode=1 \
  --enable=warning \
  --suppress=missingIncludeSystem \
  source/ includes/

# Run local build
make clean && make
```

### Editor Integration

Configure your editor to show warnings:
- **VS Code**: Install C/C++ extension, configure with `-Wall -Wextra`
- **Vim**: Use ALE or Syntastic with cppcheck
- **CLion**: Configure CMake with strict flags

## Future Enhancements

Potential additions to the pipeline:

- [ ] Unit testing framework integration (check, cmocka, etc.)
- [ ] Code coverage reporting (gcov, lcov)
- [ ] Memory leak detection (Valgrind)
- [ ] Automated documentation generation (Doxygen)
- [ ] Performance benchmarking
- [ ] Release automation
- [ ] Docker container builds

## Getting Help

If you encounter issues with the CI/CD pipeline:
1. Check this documentation
2. Review workflow logs in GitHub Actions
3. Open an issue with the `ci` label
4. Contact repository maintainers

## Related Documentation

- [Branch Protection Configuration](.github/BRANCH_PROTECTION.md)
- [Pull Request Template](.github/pull_request_template.md)
- [Main README](../README.md)
