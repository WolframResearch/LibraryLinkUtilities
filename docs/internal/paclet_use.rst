====================================
Use in paclets
====================================

## Note for Import/Export developers

There is an alternative approach to what is described above that Import/Export developers tend to use.
In I/E paclets :term:`LLU` is included as a git submodule. Submodules are simply git repos inside other repos but working with them may sometimes be tricky. See this
excellent [tutorial on submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

For SourceTree users there is also a helpful [blog post](https://blog.sourcetreeapp.com/2012/02/01/using-submodules-and-subrepositories/).

In most cases you will access *LibraryLink Utilities* in "read-only" manner, i.e. you will just update the submodule to make sure you use the most recent version.

When you work on your paclet you may occasionally find a bug in LLU or a missing feature. You should either report it or try to make changes yourself following the usual workflow:
create new branch - implement changes - open PR - merge. It is possible to do it with LLU as a submodule in your project but you should really read the tutorial before you try.


Here is a list of commands that will be useful to developers working on Import/Export paclets (names ending with "Tools").
Usually these paclets have _CPPSource/_ directory containing the source code. It is easy to modify these commands so that they work for arbitrary project.

##### Adding *LibraryLink Utilities* to your paclet
```bash
git submodule add ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git CPPSource/LibraryLinkUtilities
```

##### Cloning a project that already uses *LibraryLink Utilities*
```bash
git clone --recursive ssh://git@stash.wolfram.com:7999/imex/<paclet_name_in_lowercase>.git
```

##### Updating *LibraryLink Utilities* in your project
```bash
git submodule update --remote CPPSource/LibraryLinkUtilities/
```
Submodules work in a "detached head" state which means they stick to a chosen commit, so even if there are backwards incompatible changes merged to _LLU_ master
your project will not be affected unless you manually update the submodule.

With _LLU_ attached to your project in a submodule you always have the sources so you only need to follow steps 2 - 4 described above.

## Paclets that currently use *LibraryLink Utilities*
If you look for examples of _LLU_ usage and whatever is in this README was not enough you can take a look at paclets that already use _LLU_.
The list below may not be complete.

- [PacletTemplate](https://stash.wolfram.com/projects/IMEX/repos/paclettemplate) - this is a model paclet for Import/Export developers
- [GIFTools](https://stash.wolfram.com/projects/IMEX/repos/giftools)
- [MediaTools](https://stash.wolfram.com/projects/IMEX/repos/mediatools)
- [RAWTools](https://stash.wolfram.com/projects/IMEX/repos/rawtools)
