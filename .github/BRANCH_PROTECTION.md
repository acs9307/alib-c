# Branch Protection Configuration

This document describes how to configure branch protection rules for alib-c to ensure that nothing can be merged unless it passes the CI pipeline.

## Required Branch Protection Rules

To enforce CI checks before merging, configure the following settings for your main branch (typically `main` or `master`).

### Step-by-Step Configuration

1. **Navigate to Repository Settings**
   - Go to your GitHub repository
   - Click on "Settings" tab
   - Select "Branches" from the left sidebar

2. **Add Branch Protection Rule**
   - Click "Add rule" or "Add branch protection rule"
   - In the "Branch name pattern" field, enter: `main` (or your default branch name)

3. **Configure Protection Settings**

   #### Require Pull Request Reviews
   - ☑ **Require a pull request before merging**
     - Required number of approvals before merging: `1` (adjust as needed)
     - ☑ Dismiss stale pull request approvals when new commits are pushed

   #### Status Checks
   - ☑ **Require status checks to pass before merging**
     - ☑ Require branches to be up to date before merging
     - **Required status checks** (select all of the following):
       - `Build with CMake (ubuntu-20.04, Debug)`
       - `Build with CMake (ubuntu-20.04, Release)`
       - `Build with CMake (ubuntu-22.04, Debug)`
       - `Build with CMake (ubuntu-22.04, Release)`
       - `Build with Make (Legacy)`
       - `Build Examples`
       - `Static Analysis`
       - `All Checks Passed`

   #### Additional Protections
   - ☑ **Require conversation resolution before merging**
   - ☑ **Do not allow bypassing the above settings**
   - ☑ **Restrict who can push to matching branches** (optional but recommended)
     - Add specific users or teams who can push directly

   #### Force Push and Deletion
   - ☑ **Do not allow force pushes**
   - ☑ **Do not allow deletions**

4. **Save Changes**
   - Click "Create" or "Save changes" at the bottom

## What This Protects Against

With these settings enabled:

✓ **No direct pushes to main branch** - All changes must go through pull requests
✓ **CI must pass** - All builds, examples, and static analysis must succeed
✓ **Code review required** - At least one approval needed before merge
✓ **Up-to-date branches** - PRs must be rebased/merged with latest main
✓ **No force pushes** - Prevents rewriting history on protected branch
✓ **Conversation resolution** - All review comments must be addressed

## Pipeline Checks Enforced

The CI pipeline runs the following checks that must pass:

### 1. Build with CMake
- Tests both Debug and Release builds
- Tests on Ubuntu 20.04 and 22.04
- Uses strict compiler flags: `-Wall -Wextra -Werror`
- Verifies library installation

### 2. Build with Make (Legacy)
- Ensures backward compatibility with Makefile
- Verifies libalibc.a is created

### 3. Build Examples
- Compiles all example programs
- Ensures API compatibility
- Validates library linkage

### 4. Static Analysis
- Runs cppcheck for code quality
- Checks for common issues
- Validates code formatting

## Enforcement Timeline

Once configured, the branch protection rules take effect immediately:
- Existing pull requests will need to pass all checks
- Direct pushes to protected branches will be rejected
- Force pushes will be blocked

## For Repository Administrators

If you need to bypass these protections in an emergency:
1. You can temporarily disable branch protection
2. Make your changes
3. **Immediately re-enable protection**

However, this should be avoided except in critical situations.

## Testing the Configuration

To verify branch protection is working:

1. Create a test branch:
   ```bash
   git checkout -b test-branch-protection
   ```

2. Make a small change and push:
   ```bash
   echo "test" >> README.md
   git add README.md
   git commit -m "Test: branch protection"
   git push origin test-branch-protection
   ```

3. Create a pull request to main branch

4. Verify that:
   - You cannot merge until CI checks pass
   - All required status checks appear in the PR
   - You cannot push directly to main

## Updating Status Checks

If you add or modify workflow jobs in `.github/workflows/ci.yml`, remember to:
1. Update the list of required status checks in branch protection settings
2. The job names must match exactly (case-sensitive)

## Troubleshooting

**Status checks not appearing?**
- Ensure the CI workflow has run at least once on the branch
- Check that workflow file is valid YAML
- Verify the job names match exactly

**Cannot push to main?**
- This is expected! Create a pull request instead
- If you're an admin and need to push, temporarily adjust settings

**CI checks failing?**
- Review the workflow logs in the "Actions" tab
- Fix the issues in your branch
- Push new commits to trigger checks again

## Additional Resources

- [GitHub Branch Protection Documentation](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-protected-branches/about-protected-branches)
- [GitHub Actions Status Checks](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/collaborating-on-repositories-with-code-quality-features/about-status-checks)
