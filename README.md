# Remoll SIMulation & ANAlysis (simana)
This is my simulation and analysis repository for the MOLLER experiment. This repository lives in [github here](https://github.com/pranphy/moller-simana) and the doxygen documentation lives [here](https://simana-moller.pgautam.com.np/md_analysis_readme).


## Usage
Most of scripts under [Simulation](sim/README.md) are used to run simulation with [remoll](https://github.com/JeffersonLab/remoll) and the scripts under [Analysis](analysis/README.md) are used for analysis of the output files from remoll.

For example one of the most common tasks is the [Skimming Files][#skim]. Which for example is done with a script like \ref select_subset_MD.C which makes heavy use of \ref RemollTree.hpp, \ref RemollData.hpp and \ref utils.hh.

## Organization
The directory structure is organized as:

~~~{.sh}
.
├── _nb
├── analysis
│  ├── _nb
│  ├── cpp
│  ├── files
│  ├── julia
│  ├── python
│  ├── README.md
│  └── script
├── asset
│  ├── files
│  └── image
├── report
│  ├── note
│  └── slide
├── sff
│  ├── _nb
│  ├── analysis
│  └── sim
├── shared
│  └── cancel_jobs.sh
└── sim
   ├── bin
   ├── files
   ├── geometry
   ├── jobs
   ├── log
   ├── macros
   ├── README.md
   └── script

~~~

- simana / (root)
  - `sim` :   All scripts for [Simulation](sim/README.md)
    - `script` : All scripts related to simulation. There are mostly bash scripts.
    - `output` : 
  - `analysis` : All script for [Analysis](analysis/README.md)


The scripts are organized according to languagee.
