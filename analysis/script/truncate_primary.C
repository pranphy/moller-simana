#include <iostream>
#include <string>

#include "RemollData.hpp"


void truncate_primary(std::string primaryfile,std::string truncatedfile,unsigned count)
{
  RemollData primary = RemollData(primaryfile,"skim tree",RemIO::READ);
  RemollData truncated = RemollData(truncatedfile.c_str(),"T",RemIO::WRITE);
  // get firt `count` entries from the primary tree
  std::vector<RemollHit> rh = primary.get_values<RemollHit>("hit",identity<RemollHit>,count);
  truncated.add_branch<RemollHit>(rh,"hit");
}

