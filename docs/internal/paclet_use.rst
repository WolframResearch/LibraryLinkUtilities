====================================
Use in paclets
====================================

Submodule
=============================

There is an alternative approach to official use instructions that Import/Export developers tend to choose.
In I/E paclets :term:`LLU` is often included as a git submodule. Submodules are simply git repos inside other repos but working with them may sometimes be tricky.
See this excellent `tutorial on submodules <https://git-scm.com/book/en/v2/Git-Tools-Submodules>`_.

For SourceTree users there is also a helpful `blog post <https://blog.sourcetreeapp.com/2012/02/01/using-submodules-and-subrepositories/>`_.

In most cases you will access *LibraryLink Utilities* in "read-only" manner, i.e. you will just update the submodule to make sure you use the most recent version.

When you work on your paclet you may occasionally find a bug in *LLU* or a missing feature. You should either report it or try to make changes yourself
following the usual workflow: create new branch - implement changes - open PR - merge.

.. warning::

   All development of *LLU* should happen on GitHub. We still maintain a Stash mirror of the repo that paclets should link to, but no changes should be made there.


Here is a list of commands that will be useful to developers working on Import/Export paclets (names ending with "Tools").
Usually these paclets have :file:`CPPSource/` directory containing the source code. It is easy to modify these commands so that they work for arbitrary project.

* Adding *LibraryLink Utilities* to your paclet

   .. code-block:: bash

      git submodule add ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git CPPSource/LibraryLinkUtilities

* Cloning a project that already uses *LibraryLink Utilities*

   .. code-block:: bash

      git clone --recursive ssh://git@stash.wolfram.com:7999/imex/<paclet_name_in_lowercase>.git

* Updating *LibraryLink Utilities* in your project

   .. code-block:: bash

      git submodule update --remote CPPSource/LibraryLinkUtilities/

Submodules work in a "detached head" state which means they stick to a chosen commit, so even if there are backwards incompatible changes merged to *LLU* master
your project will not be affected unless you manually update the submodule.

With *LLU* attached to your project in a submodule you always have the sources so you only need to follow steps 2 - 4 described
:doc:`in the official build instructions<../basic/how_to_use>`.

Paclets that use LLU
==========================================================

If you look for examples of *LLU* usage and whatever is in this documentation was not enough you can take a look at paclets that already use *LLU*.
The list below may not be complete.

- `PacletTemplate <https://stash.wolfram.com/projects/IMEX/repos/paclettemplate>`_ - this is a model paclet for Import/Export developers
- `GIFTools <https://stash.wolfram.com/projects/IMEX/repos/giftools>`_
- `SVTools <https://stash.wolfram.com/projects/IMEX/repos/svtools>`_
- `XLTools <https://stash.wolfram.com/projects/IMEX/repos/xltools>`_