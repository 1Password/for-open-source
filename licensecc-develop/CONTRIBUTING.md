# Contributing to Licensecc

## Interacting with the team 

If you're experiencing issues with the project, due to lack of documentation 
or you want to ask a question you can contact us on 
[project forum](https://groups.google.com/forum/#!forum/licensecc). 
The forum is the preferred way of contact because your question are useful
to other people.

If you're compiling `licensecc` following the guide and you meet a software issue
(eg. doesn't compile, it crashes) you can submit an issue. Please try to be very specific about your problem. 
You can use the guidelines in ["How to Submit A (Good) Bug Report"](#how-to-submit-a-good-bug-report) to structure your question. 

### Reporting Bugs

Before creating bug reports, please [check the repository](https://github.com/open-license-manager/licensecc/issues) to see if the problem has already been reported. If it has **and the issue is still open**, add a comment to the existing issue instead of opening a new one. When you are creating a bug report, please [include as many details as possible](#how-to-submit-a-good-bug-report).

#### How to submit a good Bug Report

Bugs are tracked as [GitHub issues](https://guides.github.com/features/issues/). Explain the problem and include additional details to help maintainers reproduce the problem:

-   **Use a clear and descriptive title** for the issue to identify the problem.
-   **Describe the exact steps which reproduce the problem** in as many details as possible. For example, start by explaining how are you using Licensecc.  
-   **Provide specific examples to demonstrate the steps**. Include links to files or GitHub projects, or licenses, which can cause the bug. If you're providing code snippets in the issue, use [Markdown code blocks](https://help.github.com/articles/markdown-basics/#multiple-lines). 
-   **Describe the current behavior and the expected one*** Describe the current result, and the expected behavior.
-   **Provide a unit test to demonstrate the bug**. The best way to report a bug, and to have it fixed **forever** is to design a test to demonstrate it. 
-   **If you're reporting that Licensecc crashed**, include a crash dump and the associated message. 
-   **Label the issue as bug.**

Provide more context by answering these questions:

-   **Can you reproduce the problem using the example application?**
-   **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.
-   If the problem is related integrating Licensecc with your application, **produce a minimal example to demonstrate it** Does the problem happen only with some license type? Does the problem only happen in Linux/Docker/Windows?

Include details about your configuration and environment:

-   **Update Licensecc to the latest version** If possible try to pull the latest changes from `develop` branch.
-   **What's the name and version of the OS you're using**?
-   **What's the name and version of the compiler you're using**? Are you cross compiling? If you're cross compiling specify the host and the target operating system.
-   **What's are the `cmake` command line you used to generate your build scripts**? 
-   **Are you running Licensecc in a virtual machine/docker?** If so, which VM software are you using and which operating systems and versions are used for the guest?

### Suggesting Enhancements or new features

If you have an idea about a new feature or a question about an environment support, the simplest way you can reach the team is through the [project forum](https://groups.google.com/forum/#!forum/licensecc).

Otherwise you can open an enhancement suggestion in github. Before you do so check [existing enhancement request](https://github.com/open-license-manager/licensecc/issues?utf8=%E2%9C%93&q=is%3Aissue+label%3Aenhancement) to see if the enhancement has already been suggested.

Please also check for the [current and planned features](https://github.com/open-license-manager/licensecc/wiki/features) in the wiki to see where the project is heading to.

### Code contributions

Contributions to `licensecc` will be subject to the following rules:

-   small patches eg. documentation changes, small bug fixes can be submitted directly on github, use the description of the patch to shortly explain the content of the patch. 
-   if the patch is not completely trivial please open an issue first and provide a description of what you want to achieve.
-   larger contributions should be discussed first on the [project forum](https://groups.google.com/forum/#!forum/licensecc). 

If you want to contribute we are happy to integrate your code. You can start by looking through the [`good first issue`](https://github.com/open-license-manager/licensecc/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) and [`help-wanted`](https://github.com/open-license-manager/licensecc/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) issues. 

You can have a look to the project upcoming milestones to see what's going to be implemented soon and what is the project direction. If you decide to contribute on an issue scheduled in the current milestone, comment on the issue first. If the issue is marked as "assigned" there may be already some work done for it. Commenting on the issue will ensure we don't duplicate our work. Also have a look to the branches:  there may be a feature branch corresponding to the issue with some work already done.   

If you have already forked the repository to implement a specific feature, and you want your code to be merged in the main repository please first file an enhancement request as explained in [suggesting enhancements](#suggesting-enhancements-or-new-features). If you found a bug and you want to propose a fix please [report a bug](#reporting-bugs) before.

We apply a subset of [GitFlow](https://nvie.com/posts/a-successful-git-branching-model) development workflow. Be sure to work against `develop` branch, since `master` is reserved for stable releases, and may be outdated.
 
#### General coding rules (for larger contributions)

Supposing you already know how to contribute to an open source project on GitHub (if you have doubts you can check this short [guide](https://git-scm.com/book/en/v2/GitHub-Contributing-to-a-Project) ), you're working on an existing issue the code is already committed on your fork.

-   Ensure your feature branch is up to date with the `develop`, eventually merge the latest changes from the `develop` branch. This will help us save time.
-   Reformat the changed code using "clang-format" to keep consistent formatting style. The style we use is in `.clang-format` at the base of the project.
-   Prepare your pull request, in the pull request comment reference the issue the pull request will fix.
-   Check your pull request compiles and pass the checks on Travis CI
-   In the pull request comment reference the issue you want to fix.

##### Don't
-   Don't reformat the code following your personal likes, it introduce a lot of "noise" and makes very hard to merge. Use the clang-format style provided at the base of the project.
-   Very large pull requests with few comments, no corresponding issue explaining what's it about will probably be rejected.
-   We understand that the project is still in beta stage, however we would like to discuss it with you before we take project changing decision. You can reach us on the [project forum](https://groups.google.com/forum/#!forum/licensecc). 
