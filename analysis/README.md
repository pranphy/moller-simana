# Analysis
This contains the analysis scripts.

You can look up \ref utils.hh files to see a bunch of functions.

# Skimmig file

Selecting the subset of remoll data is something we do very often. The relevant data structure of remoll is available from the header
file `remolltypes.hh`. This header is available when executing the root macro with `reroot`. You could also use the `root` executable
but then you would have to manually import `libremoll.so` prior to executing script. Something like this on the top of the macro should
work for this purpose:
```cpp
gSystem->Load("~/sft/remoll/build/quick-remoll/libremoll.so")
```

Where `libremoll.so` is the compiled library which is generated when you compile remoll and is found in the same directory as the `reroot`
executable.


# Using RemollTree.hpp
There is \ref RemollTree.hpp, which can be used to loop through each event in the remoll output root tree "T" (there is a better way to do 
it).


To loop through the tree you can do this in a macro `loop_tree.C`

```{cpp}
#include "RemollTree.hpp"

void loop_tree(){
    std::string filename = "path/to/remoll/rootfile.root";
    auto T = utl::get_tree(filename);
    RemollTree RT(T,{"hit"}); // This tells to only access the "hit" branch, makes it faster, you can pass other branches as well.
    for(RT.loop_init(); RT.next(); ){
        hit_list hl = *RT.cur_hit;
    }
}
```


