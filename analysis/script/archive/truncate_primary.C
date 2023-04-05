#include <iostream>
#include <string>

#include "RemollData.hpp"


void truncate_primary_og(std::string primaryfile,std::string truncatedfile,unsigned count)
{
  RemollData primary = RemollData(primaryfile,"skim tree",RemIO::READ);
  RemollData truncated = RemollData(truncatedfile.c_str(),"T",RemIO::WRITE);
  // get firt `count` entries from the primary tree
  std::vector<RemollHit> rh = primary.get_values<RemollHit>("hit",identity<RemollHit>,count);
  truncated.add_branch<RemollHit>(rh,"hit");
}

RemollHit modify_hit(RemollHit oldhit)
{
    RemollHit newhit  = oldhit;
    newhit.x =  oldhit.x;
    newhit.y =  oldhit.y;
    newhit.z =  oldhit.z-0.1; // 0.1mm subtracted
    return newhit;
}

ostream& operator<<(ostream& os, RemollHit& hit)
{
    os<<"("<<hit.x<<","<<hit.y<<","<<hit.z<<")";
    return os;
}

void truncate_primary(std::string primaryfile,std::string truncatedfile,unsigned count) {
  RemollData primary = RemollData(primaryfile,"skim tree",RemIO::READ);
  RemollData truncated = RemollData(truncatedfile.c_str(),"T",RemIO::WRITE);
  // get firt `count` entries from the primary tree
  std::vector<RemollHit> rh = primary.get_values<RemollHit>("hit",identity<RemollHit>,count);
  std::cout<<"The hit object "<<std::endl;
  for(RemollHit obj: rh)
  {
     //std::cout<<"(x,y,z) = ("<<obj.x<<","<<obj.y<<","<<obj.z<<")"<<std::endl;
     std::cout<<"(x,y,z) = "<<obj;
     RemollHit modified = modify_hit(obj);
     std::cout<<"--> "<<modified<<std::endl;
     //std::vector<RemollHit> v = {modified};
     std::vector<RemollHit> v = {obj};
     truncated.add_branch(v,"hit");
  }
}

