
using UnROOT

workdir = "/w/halla-scshelf2102/moller12gev/pgautam/pf"
filename="$(workdir)/simana/sim/output/primary/helicoilout-911-beam-1k-new-geometry-no-disk-from-script.root"

#filename = "simana/sim/output/primary/helicoilout-911-beam-1k-new-geometry-no-disk-from-script.root"

f = ROOTFile(filename)

dirname(pwd())

mt = LazyTree(f,"T",r"hit_(det|x|y|z)$")

mt
ee = hcat(mt.hit_e...)