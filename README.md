=============================
HuxleyMuscleModel
=============================

Huxley Muscle Model implementation on DFE.

<img src="http://appgallery.maxeler.com/v0.1/app/Huxley%20Muscle%20Model/icon" alt="Jacobi Solver">

Description
------------

A skeletal muscle fiber generates tension when properly stimulated, for instance by the nervous system or by electrical impulses.
Theory developed by Huxley describes active shortening of muscles as the result of myosin cross-bridges, causing the myosin filaments to slide relative to actin filaments.


Content
-----

The repo root directory contains the following items:

- APP
- DOCS
- ORIG
- LICENCE.txt

### APP

Directory containing project sources.

### DOCS

Documentation of the project.

### ORIG

Directory containing the original CPU implementation
  
### LICENSE.txt

License of the project.


Information for compile
--------

Ensure the environment variables below are correctly set:
  * `MAXELEROSDIR`
  * `MAXCOMPILERDIR`

To compile the application, run:

    make -C APP/CPUCode/ RUNRULE="<ProfileName>"

If would like to remove the distributed maxfiles before recompiling the application run the following command before compilation:

    make -C APP/CPUCode/ RUNRULE="<ProfileName>" distclean


Huxley Muscle Model on [AppGallery](http://appgallery.maxeler.com/)
