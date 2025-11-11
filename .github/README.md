# GitHub Configuration Directory

This directory contains GitHub-specific configuration files for the alib-c project.

## Contents

### 📁 Workflows (`workflows/`)
Contains GitHub Actions workflow definitions for CI/CD automation.

- **`ci.yml`**: Main CI/CD pipeline
  - Builds with CMake (multiple configurations)
  - Builds with Make (legacy support)
  - Compiles and validates examples
  - Runs static analysis
  - Enforces code quality standards

### 📄 Configuration Files

- **`dependabot.yml`**: Automated dependency updates
  - Keeps GitHub Actions versions up to date
  - Runs weekly checks every Monday
  - Creates PRs for outdated dependencies

### 📋 Templates

- **`pull_request_template.md`**: Standard PR template
  - Guides contributors through the PR process
  - Ensures consistent PR descriptions
  - Includes testing checklist

### 📚 Documentation

- **`CI_CD_PIPELINE.md`**: Comprehensive CI/CD documentation
  - Pipeline architecture and job descriptions
  - Local development workflow
  - Troubleshooting guide
  - Performance optimization tips

- **`BRANCH_PROTECTION.md`**: Branch protection setup guide
  - Step-by-step configuration instructions
  - Required status checks
  - Security best practices

## Quick Links

| Document | Purpose |
|----------|---------|
| [CI/CD Pipeline](CI_CD_PIPELINE.md) | Understanding and working with the CI pipeline |
| [Branch Protection](BRANCH_PROTECTION.md) | Configuring branch protection rules |
| [PR Template](pull_request_template.md) | Template used for all pull requests |
| [Main Workflow](workflows/ci.yml) | CI/CD workflow definition |

## For Contributors

Before submitting a pull request:

1. ✅ Read the [CI/CD Pipeline documentation](CI_CD_PIPELINE.md)
2. ✅ Ensure your changes pass all CI checks locally
3. ✅ Follow the project's coding conventions (see main README.md)
4. ✅ Fill out the PR template completely

## For Maintainers

Setting up the repository:

1. 🔧 Configure branch protection rules using [this guide](BRANCH_PROTECTION.md)
2. 🔧 Enable GitHub Actions in repository settings
3. 🔧 Set up required status checks
4. 🔧 Configure team permissions

## Pipeline Status

The pipeline ensures:
- ✓ Code builds on multiple platforms
- ✓ No compiler warnings or errors
- ✓ Examples compile successfully
- ✓ Static analysis passes
- ✓ Code quality standards are met

## Getting Help

- **CI Issues**: Check [CI_CD_PIPELINE.md](CI_CD_PIPELINE.md) troubleshooting section
- **Branch Protection**: See [BRANCH_PROTECTION.md](BRANCH_PROTECTION.md)
- **General Issues**: Open an issue in the main repository

## Maintenance

This directory is maintained alongside the main codebase. When making changes:
- Update documentation if workflow changes
- Test workflow changes in a PR before merging
- Keep dependabot enabled for security updates
