# Analysis
This contains the analysis scripts.

You can look up \ref utils.hh files to see a bunch of functions.

## utils.hh
This file (utils.hh) is in center of a lot of other scripts. It defines bunch of little tiny functions that are used throughout in all scripts.
Care is taken to make sure that this runs with `c++14` and later. Because at some point I realized that the widely used version of root
at ifarm is compiled with c++14 so some of the c++17 features don't compile. I have since staretd to use a custom root compiled with c++17
 so there might be some simple function that slipped that require c++17.


## Compiled library

Selecting the subset of remoll data is something we do very often. The relevant data structure of remoll is available from the header
file `remolltypes.hh`. This header is available when executing the root macro with `reroot`. You could also use the `root` executable
but then you would have to manually import `libremoll.so` prior to executing script. Something like this on the top of the macro should
work for this purpose:
```cpp
gSystem->Load("~/sft/remoll/build/quick-remoll/libremoll.so")
```

Where `libremoll.so` is the compiled library which is generated when you compile remoll and is found in the same directory as the `reroot`
executable.



## Executing  macros
Using `reroot` vs `root`.

Imagine you have a root macro namemd `loop_tree.C`

```{cpp}
void loop_tree(){
    // ...
    // ...
}
```

- 1. If using `reroot` you could just execute this as 

```{bash}
$ reroot -b -l -q loop_tree.C
```

 - 2. If using `root` then you need to import `libremoll.so` in the script. so your `loop_tree.C` has to be somethign like
```{cpp}
// loop_tree.C
gSystem->Load("~/sft/remoll/build/quick-remoll/libremoll.so")
//

void loop_tree(){
    // ...
    // ...
}

```
The libremoll.so file is produced in the default build directory  that executables reroot  and remoll are produced.


Then execution is again

```{bash}
$ root -b -l -q loop_tree.C
```


## Usage example: Skimming a file
Look at  [select_subset_MD.C](https://github.com/pranphy/moller-simana/blob/master/analysis/script/select_subset_MD.C) (updated over time) for code and \ref select_subset_MD.C for documentation, as an example of a skim script. We will try to breakdown the code in that file.

Here is a content of that script (static) for reference.
```{cpp}
#include <string>
#include <vector>

#include "RemollTree.hpp"
#include "RemollData.hpp"

bool cut(RemollHit hit){
    return utl::md_ring_cut(hit,0) && hit.e > 1 && (hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON);
}


hit_list get_subset_on_det(hit_list hits){
    hit_list  rev_hits(0);
    for(auto hit: hits) if(cut(hit)) rev_hits.push_back(hit);
    return rev_hits;
}


void select_subset_MD(std::string inputfile, std::string outputfile,std::string extension=".root"){
    RemollTree RT(inputfile,"T");
    std::vector<hit_list> selected;
    for(RT.loop_init(); RT.next();){
        hit_list hits_on_md = get_subset_on_det(*RT.cur_hits); // get subset on Moller Detector
        if (hits_on_md.size()  > 0) selected.push_back(hits_on_md);
    }
    RemollData RD(outputfile+extension,"T",RemIO::WRITE);
    RD.add_branch(selected,"hit",true); // true because we want to write each element as row.
}
```

Lets breakdown each function part by part.

 - 1. `bool cut(RemollHit hit);`

This function takes a \ref RemollHit object and returns a boolean depending on the curt you want.
For example if we want to filter a event that hits ring 5  on moller detector 28, we would have
```{cpp}
bool cut(RemollHit hit){
    return  hit.det == 28 and hit.r > 930 and hit.r < 1070 and abs(hit.pid) == 11;
}
```

Selecting moller ring is somethign we do very very often. So there are utilities function in `utils.hh` 
for selecting moller ring, so for clarity we can equivalently write this function as
```{cpp}
bool cut(RemollHit hit){
    return  utl::mid_ring_cut(hit,5) and  abs(hit.pid) == 11;
}
```

 - 2. `hit_list get_subset_on_det(hit_list hits);`

This function takes a list of hits. This is what the hit tree contains per event. This is essentially an
array of hits across all enabled sensitive detectors. This function returns a subset of hits in the same
data structure as the input (the std::vector of RemollHit). This function uses the `cut` function defined earlier
to make the subset selection.

```{cpp}
hit_list get_subset_on_det(hit_list hits){
    hit_list  rev_hits(0);
    // For each hit in the hits array, if it passes the cut , keep it in the new hit list
    for(auto hit: hits) if(cut(hit)) rev_hits.push_back(hit);
    return rev_hits;
}
```
Using these two functions we can select a subset of hits.


 - 3. `void select_subset_MD(std::string, std::string, std::string);`

The function takes path to input file and writes an output file with 
the subset of events selected by the use of above two functions.
Reading the remoll tree and looping through it, is done with RemollTree.hpp.

There is \ref RemollTree.hpp, which can be used to loop through each event in the remoll output root tree "T" (there is a better way to do 
it). Also look documentation for \ref RemollTree::loop_init and \ref RemollTree::next()

```{cpp}
#include "RemollTree.hpp"

//std::string filename = "path/to/remoll/rootfile.root";

void select_subset_MD(){
    // Creates RemollTree object from the root file to read just "hit" branch.
    RemollTree RT("path/to/input/file.root",{"hit"});
    // This is std::vector of hit_list ( std::vector of std::vector in essence )
    // Because hist_list is hits per event. So std::vector<hist_list> is hits 
    // over entire simulation.
    std::vector<hit_list> selected;


    //initialize the loop, this loops event by event.
    for(RT.loop_init(); RT.next();){
        // *RT.cur_hits is equivalent to hit_list. This is all the hits per event.
        hit_list hits_on_md = get_subset_on_det(*RT.cur_hits); // get subset on Moller Detector
        // There might be some event that dont' have any hits that pass the cut, skip them.
        if (hits_on_md.size()  > 0) selected.push_back(hits_on_md);
    }


    

    // Prepare to write an root output file with tree name "T".jK:w
    RemollData RD("Outputfilename.root","T",RemIO::WRITE);

    // Write the entire seelcted hits into the root file on a branch named "hit", \ref RemollData::add_branch
    RD.add_branch(selected,"hit",true); // true because we want to write each element as row.
}
```


## Other scripts
There are multiple scripts  inside `analysis/scripts/` directory that do various things.

