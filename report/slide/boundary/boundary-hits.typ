
#import "@local/tymer:0.1.0": *
#show: slides.with(
    authors: [Prakash Gautam\   #text(font:"Noto Serif Devanagari", fill: white.darken(71%))[〈 प्रकाश गौतम|  پرکاش گوتم 〉]],
    institute: Institute("University of Virginia",short:"UVa"),
    short-authors: "P. Gautam",
    //title: "Spectrometer, Beamline, Simulation and Timeline",
    title: "Boundary hits in Geant4 : `remoll`",
    location: "MOLLER Simulation Meeting",
    //thm: (fg:black.darken(00%),bg:white.lighten(00%),ac: orange),
    //date: "2024-01-17",
    aspect: 16/9
)

#titlepage()

#slide(title: "Question")[
  #align(center)[#image("./asset/image/boundary/one-tile-cfg-03.svg",width:45%)]
  - When only boundary hits is requested in remoll simulation with
  ` /remoll/SD/boundaryhits <detid>`

  does remoll store hits on both the entering and exiting planes as boundary hits?
]


#slide(title: "Tracks and Steps in Geant4")[
  #align(center)[#image("./asset/image/boundary/bh.svg",width:50%)]
  - Each step has a pre-point and a post-point. Both have identical coordinates.
  - A step point is always guarnateed at the boundary.
  - At the boundary
    - The pre-point  logically belongs to the "entering volume".
    - Thep post-point logically belongs to the "exiting volume".
]

#slide(title: "Tracks and Steps in Geant4")[
  #align(center)[#image("./asset/image/boundary/reject.svg",width:50%)]
  - Since pre- and post- points are identical in coordinates, we can choose to write either of those consistantly.
  - `remoll` writes  prepoints for all _tracks except_ for optical photons, for which it writes post points.
  - `if( prepoint  == boundary );` its guranteed to be entering track.
]


#slide(title: "Test Case: Simple Geometry")[
  #align(center)[#image("./asset/image/rempton-almunium-hits-tracks2.png",width:60%)]
  - Very simple geometry with with two solids back to back. Both made of `G4_Al`
  - Secondaries are generated from both. The right side solid is made sensitive.
]

#slide(title: "Test Case: Simple Geometry")[
  #grid(columns:(50%,50%),[
    Boundary hits enabled
    #align(center)[#image("./asset/image/boal-only-boundary-hit.svg",width:50%) ]

    - There are hits mostly in the entering boundary.
    - The hits in the back face is approaching from right to left (pz < 0).
  ],[
     Boundary hit not enabled
    #align(center)[#image("./asset/image/boal-no-boundary-hit.svg",width:50%) ]
    - There are hits throughout the detector.
    - Most of the hits are along the path of beam in the second detector.
  ]
)
]


#slide(title: "Test Case: Quartz Tile")[
  #grid(columns:(50%,50%),[
    #v(0.5em)
    - One Quartz tile
    #align(center)[#image("./asset/image/boundary/one-tile-cfg-03.svg",width:50%)]
    #align(center)[#image("./asset/image/boundary/one-tile-cfg-03-fwd.svg",width:50%)]
  ],[
    #v(0.5em)
    - One Quartz tile back face
    #align(center)[#image("./asset/image/boundary/one-tile-cfg-03-back-face.svg",width:50%)]
    #align(center)[#image("./asset/image/boundary/one-tile-cfg-03-back-face-fwd.svg",width:50%)]
  ]
)
- Top row: all hits, bottom row only `pz > 0`
  //#image("./asset/image/only-ring5-open-sectors.svg",width:20%) #image("./asset/image/only-ring5-open-sectors.svg",width:20%)
]


#slide(title: "Summary")[
  - Boundary hits cuts are working as expected.
  - No two hits share the track ID, so one track only registers in on boundary.
  - No secondary produced within the volume are stored when only boundary hits is enabled.
  - The hits in the outgoing boundary are mostly particles approaching into the volume from the back boundary.
]
