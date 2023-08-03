#import "mlides.typ": *
#show: slides.with(
    authors: "Prakash Gautam",
    institute: Institute("Uniersity of Virginia",short:"UVa"),
    short-authors: "P. Gautam",
    title: "Background from all edges",
    short-title: "Shorter title for slide footer",
    location: "Moller Simulation Meeting",
    thm: (fg:white.darken(00%),bg:black.lighten(21%),ac: orange)
    //thm: (fg:black,bg:white,ac: orange)
    //thm: (fg:black,bg:rgb("caeec2").darken(20%),ac: green)
)

#titlepage()

#new-section("Collar 2")

#slide(title: "Background from different edges")[
  - Ran a simulation with 760M  beam events with symmetric fields.
    - **Was aiming 1B but some jobs were incomplete.**
  - Assumes 1 Moller event per 3300 beam events in Ring 5 $ p_z > 0$.
  $  "Moller Rate"  =  "# of events in Ring 5" / ("# number of primary"/ 3300) times 100% $
  //- Geometry from develop branch as of July 10.
  - Edge scattering at various collimation components and edges.
]

#slide(title: [Charged Backgrounds $e^plus.minus$])[
  #figure(image("asset/image/all-bg/epm-no-qtz-no-target-vz.png",width:90%))
  - The quartz region taken out [$(v_z,v_r) in.not  (20500,800) - (22500,1250)$]
    - This region is roughly where the big spike from the Quartz appeared.
]


#slide(title: [Charged Backgrounds $e^plus.minus$])[
  #figure(image("asset/image/all-bg/all-bg-bar-32-epm.png",width:67%))
  - Approximately in z order.

]

#slide(title: [$gamma$ Backgrounds ])[
  #figure(image("asset/image/all-bg/all-bg-bar-32-photon.png",width:67%))
  - Approximately in z order.
]

#slide[
  #alert[backup]
]

/*
#{
  for x in range(1,88,step:1){
    let num = str(x)
    if x < 10 { num = "0"+str(x) }
    slide[
      #figure(image("asset/image/all-bg/38/converted-vz-vr-for-"+num+".png",width:70%))
    ]
  }
}
*/
