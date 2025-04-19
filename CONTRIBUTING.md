# Contributing to OpenBangla Keyboard (OBK)

Thank you for your interest in contributing to OpenBangla Keyboard! We welcome contributions of all kinds, whether it's code, documentation, bug reports, feature requests, or helping fellow contributors.

Please take a moment to review this guide before submitting your contribution.

## Code of Conduct

We expect all contributors to adhere to our [Code of Conduct](CODE_OF_CONDUCT.md). Please read it carefully and ensure you understand and agree to its terms.

## How Can I Contribute?

There are many ways you can contribute to this project:

### Reporting Bugs

If you encounter a bug, please help us by submitting a detailed issue. When reporting a bug, please include:

- **A clear and descriptive title** for the issue.
- **Steps to reproduce the bug** as precisely as possible.
- **What you expected to happen** instead of the actual outcome.
- **Any relevant environment information**, such as your operating system, software versions, and any specific configurations.
- **If possible, include any error messages or screenshots** that might be helpful.

### Suggesting Enhancements

We are always open to suggestions for new features and improvements. When suggesting an enhancement, please consider:

- **Clearly describe the proposed enhancement** and its benefits.
- **Explain the use case** and why you believe this enhancement would be valuable.
- **If possible, suggest how the enhancement might be implemented.**
- **Check if a similar feature request already exists** before creating a new one.

### Contributing Code

If you'd like to contribute code, please follow these steps:

1.  **Fork the repository** on GitHub.
2.  **Clone your fork** to your local machine:

    ```bash
    git clone --recursive https://github.com/OpenBangla/OpenBangla-Keyboard.git
    cd OpenBangla-Keyboard
    ```

3.  **Create a new branch** for your changes. Choose a descriptive name:

    ```bash
    git checkout -b feature/your-feature-name
    # or
    git checkout -b bugfix/your-bug-fix
    ```

4.  **Make your changes** and ensure your code follows the project's coding style (if any).
5.  **Add your changes** to Git:

    ```bash
    git add .
    ```

6.  **Commit your changes** with a clear and concise commit message. Follow these guidelines for commit messages:

    - Separate the subject from the body with a blank line.
    - Try limiting the subject line to 50 characters.
    - Do not end the subject line with a period.
    - Use the imperative mood ("Fix bug", "Add feature").
    - Optionally, provide a more detailed body for context.

    ```bash
    git commit -m "Add feature: Implement bangla spell checker"
    ```

7.  **Push your branch** to your fork on GitHub:

    ```bash
    git push origin your-feature-name
    ```

8.  **Create a Pull Request (PR)** through the GitHub website. When creating a PR:

    - Provide a clear and descriptive title for your PR.
    - In the description, explain the changes you've made and the problem they solve (if applicable).
    - Reference any related issues in the PR description (e.g., `Closes #123`, `Fixes #456`).

### Contributing Documentation

Good documentation is crucial! We have a dedicated repository for OpenBangla Keyboard documentation: https://github.com/OpenBangla/manual. Most documentation contributions should be made there, unless it's something that is related to this repository. If you'd like to contribute to the documentation:

- **Submit issues** for any unclear or incorrect documentation you find.
- **Create pull requests** to fix typos, improve clarity, or add new documentation. Follow the same Git workflow as contributing code. Ensure your documentation changes are well-formatted (e.g., using AsciiDoc or Markdown).

`Note: The contribution guidelines in the OpenBangla/manual repository are the preferred and prioritized source for documentation contributions.`

## Pull Request Guidelines

To ensure your pull request is reviewed and merged efficiently, please follow these guidelines:

- **Target your pull request to the `develop` branch** (or the designated development branch), not the `main` branch.
- **Keep your PR focused** on a single issue or feature. Avoid making unrelated changes in the same PR.
- **Ensure your code builds and passes all tests** (if applicable).
- **Follow the project's coding style** and conventions.
- **Write clear and concise commit messages.**
- **Provide a detailed description** in your pull request explaining your changes and their purpose.
- **Be responsive to feedback** during the code review process.

## Questions?

If you have any questions about contributing, feel free to ask in [GitHub Discussions](https://github.com/OpenBangla/OpenBangla-Keyboard/discussions)!

Thank you again for your contribution! We appreciate your time and effort in making OBK better.
