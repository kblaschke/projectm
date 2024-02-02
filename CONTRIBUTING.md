# How to Contribute

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any
contributions you make are greatly appreciated. projectM can only improve with input from the community.

You can help improving the core library and associated tools in several ways, even if you're not a coder:

- Look through the [issue tracker](https://github.com/projectM-visualizer/projectm/issues) and fix bugs or implement
  features listed there.
- Use projectM and report any issues you find.
- Write or improve documentation, e.g. in the [Wiki](https://github.com/projectM-visualizer/projectm/wiki).
- Port projectM to new platforms.

If you're unsure where you can help or what is needed most, please ask the team on GitHub or on our Discord server.

# Contribution Guidelines

To keep the projectM codebase in a clean, consistent and working state on all platforms, we have several rules and
guidelines in place which developers must follow to contribute code to any of our repositories. In addition to that, we
suggest a few best practices to make the contribution process as easy and quick as possible for both contributors and
the project maintainers.

Before starting to write or edit any code, please familiarize yourself with these guidelines to avoid having to change
or rewrite your code later.

## Language

As an international team of developers, our working language in both code and public communication (e.g. pull requests,
issues, discussions and our Discord server) is English.

## Code of Conduct

We adopt the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md) to keep a healthy community. Contributors are
required to implement these rules.

**Be excellent to each other!**

## Coding Style

projectM uses the [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html) as our base for formatting the
source code, with some case-by-case relaxations where it makes arguable sense. If in doubt whether you must apply a rule
or not, don't hesitate to ask the projectM team.

On top of the LLVM style, we have a few
tweaks in the formatting rules which can be found in the [.clang-format](.clang-format) file in the root of each
repository.

If possible, use an IDE which has integrated support for .clang-format files, so your code will be properly formatted as
you write it. If that isn't possible, e.g. your IDE doesn't provide this support, please download clang-format for your
platform (Windows users can download an [LLVM Snapshot here](https://llvm.org/builds/)) and run it on any changed files
before you commit/push your changes.

### Notable Deviations from LLVM Guidelines

This is a list of exceptions where we deviate from LLVM's guidelines.

- Use of exceptions is allowed. Care must be taken to catch them and not cross the libprojectM API boundaries.
- We do not use file header texts except for Doxygen comments.

### Naming Conventions



## Platform Support

As a cross-platform library, projectM should compile and run on as many platforms and operating system versions as
possible. To achieve this goal, we have to follow a few limitations, some of which may change over time if there is a
good technical reason:

### Keep the code compatible with C++14

As long as some operating systems such as macOS 10.x are in use which don't support C++17 in their OS runtime libraries,
we keep the code compatible with C++14. If there is a fallback option for some C++17-specific types/classes, e.g. std::
filesystem -> Boost::filesystem, it can be used carefully. Newly introduced attributes like `[[nodiscard]]` and others
should be avoided, even inside an `#ifdef` guard.

### Reduce platform-specific code to an absolute minimum

If possible, do not (directly) use any operating-system or platform-specific APIs. Either use the C++ standard library
or wrappers like Poco if already used in a project.

Should a specific feature you're implementing require introducing a new third-party library as an additional dependency,
please talk to the project maintainers if that's a workable solution.

In addition to OS-specific API calls:

- Do not use assembly code anywhere.
- Do not use CPU-specific features/intrinsics directly, such as SSE, AVX or NEON.
- If a specific feature is limited to a single platform, separate the code (e.g. using pimpl or separate implementation
  files) and set up the build system of the project in a way that it doesn't break on other platforms.

## API/ABI Stability

Since version 4.0, libprojectM ships with a new C-based API. Besides making it easier to use projectM from different
languages, we now guarantee forward binary and API compatibility. This means that any application built against a
specific version in a major release will work with any future minor version of libprojectM as well without the need to
change/rebuild code or relink the library, for example by simply replacing the shared library.

This means that any contribution that requires a change to an _existing_ API call, e.g. adding/removing or changing a
parameter or return value cannot be accepted until the next _major_ release of libprojectM.

Contributions which require _adding new API functions_ can be accepted and will be released in the next minor version.
In any case, if you're not implementing an already planned issue from our GitHub tracker, please contact the projectM
team with your idea and discuss it before writing code or creating a pull request.

As a rule of thumb, this applies to all headers installed alongside the core libraries, which normally reside in `api`
directories in the libprojectM repository.

Any internal code that does not affect the public C API can be changed, including the `ProjectM` class.

## Git Usage and Best Practices

projectM uses Git to track changes and collaborate with contributors. Since Git can be used in many different ways and
workflows, we have defined a few best practices you should follow to keep the repository clean and the review process
easy for all participants.

**Important:** To be able to contribute to projectM, you will need a [GitHub](https://github.com/) account, as we can't
accept contributions/changes over other channels like E-Mail or sent in via ZIP archives.

Please try to apply the following guidelines as strict as possible before creating a pull request. Otherwise, a reviewer
might ask you to redo/split some commits, clean up your branch history or rephrase certain commit messages, which can
cause a lot of work if done in retrospect.

### Fork and use Branches

If you're going to implement a feature, fix a bug or even a single typo, please use this general process:

- Fork (copy) the respective repository on GitHub into your personal account using the "Fork" button.
- -or- if you've already forked a while ago, update/reset your `master` branch to the upstream repository's `master`
  branch.
- Create a working branch based on the _latest_ `master` branch in your fork.
- Commit your work into this branch.
- If you take longer to implement your feature, frequently rebase your work on the upstream `master` branch to avoid
  piling up conflicts.
- If the repository has GitHub Actions (e.g. files in `.github/workflows`), ideally enable the actions in your fork to
  run the build checks.
- When done, create a pull request in the original repository to merge _your_ branch into the upstream `master` branch.

### Commit Guidelines

To keep the history clean and readable, we have a few rules regarding commits:

- Keep your commits as small and precise as possible.
- Make good use of the commit message: clearly summarize in ~80 characters what has been done in this change and why. If
  more details are required, then add a blank line and write all further information below.
- Do not use merge commits. Any pull request containing such a commit will not be merged until it has been fixed.
- If you separately committed small fixes for errors introduced in previous commits of your branch, use Git's
  interactive rebase feature to reorder and fixup the original (faulty) commits.

### Pull Requests

If you find your work is ready to be merged, you have to create a pull request on GitHub from your working branch into
the target repository. Changes have to be reviewed by one or more maintainers and can only be merged by a team member if
accepted. This also applies to changes made by the project maintainers.

When creating a pull request, please check the following items:

- Is the commit history in your branch clean, e.g. free of merge and "fixup" commits?
- Are all commits stating clearly what has been changed and why?
- Does the code still build on all required platforms?
- Are existing and new tests, if there are any, still passing on all platforms?
- If your code formatted properly?

If you've answered all these questions with yes, go ahead and create the pull request. Take your time to add a proper
headline and description, so reviewers will be able to quickly understand what you've worked on.

Once your pull request is open, it will be reviewed by the maintainers at some point. Please be patient, as it can take
a few days for someone to pick up the work. If there's no reaction within a week or so, contact us on Discord or via
email and politely ask for a review.

If the reviewer finds one or more issues with your pull request, do your best to implement the changes you've been asked
for. If you insist on keeping a certain change as-is even while being asked to amend it, this can be discussed
_politely_ via more direct channels to find a solution or compromise.

Should the maintainers decide not to merge your changes for any reason, this will be stated accordingly in the pull
request.

Be aware that keeping to insist on a merge is regarded as inappropriate behavior and, if done repeatedly or in a
derogatory way may result in your account being banned from contributing to the project.

Pull requests which are not ready for merge and without showing any activity for several weeks will be closed.