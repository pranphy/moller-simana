#import "@local/tymer:0.1.0": *
#show: slides.with(
    authors: [Prakash Gautam\   #text(font:"Noto serif", fill: white.darken(71%))[〈   پرکاش گوتم | प्रकाश गौतम 〉]],
    institute: Institute("University of Virginia",short:"UVa"),
    short-authors: "P. Gautam",
    //title: "Spectrometer, Beamline, Simulation and Timeline",
    title: "Background from Bellows4",
    location: "MOLLER Simulation Meeting",
    //thm: (fg:green.lighten(75%),bg:rgb("282a37"),ac: orange),
    //date: "2024-01-17",
    aspect: 16/9
)


#titlepage()


#slide(title: "Bellows 4")[
  #grid(columns:(80%,20%),[
      - IR                : 658.60mm
      - R Thickness       :  57.15mm
      - OR                : 715.75mm
      - Bellows Length    : 369.57mm
      - Flange Length     :  15.24mm
      - Flange US rmax    : 760.20mm
      - Flange DS rmax    : 812.80mm

      - Zpos = 12443.714mm

      - Edges zpos including flange: 12243.68 - 126433.739 mm

    ],
    image("./asset/image/bellows4/bellows4-gdml.png",height:80%)
  )
]


#slide(title: "Background: Ring 5")[
  #grid(columns:(60%,40%),[
    #v(2em)
    - Primary sim     : 100M
    - Skim (bellows 4): 7233
    - Secondary sim   : 38M
    - Ring5 count     : 16878
    - Moller rate     : 0.0106% (Assume 1 moller per 3300 beam events)

    - Rate            : $3.21 times 10^(-8)$ per eot.
    ],

    grid(rows:(auto,auto),
      image("./asset/image/bellows4/bellows4_bellows4-ring5.png",width:90%),
      image("./asset/image/bellows4/bellows4sa_bellows4-ring5.png",width:90%)
    )

  )
]

#slide(title: "Background: All Rings")[
  #grid(columns:(60%,40%),[
    #v(2em)
    - Primary sim     : 100M
    - Skim (bellows 4): 7233
    - Secondary sim   : 38M
    - Ring0 count     : 55099
    - Moller rate     : 0.034 % (Assume 1 moller per 3300 beam events)

    - Rate            : $1.048 times 10^(-7)$ per eot.

    ],
    grid(rows:(auto,auto),
      image("./asset/image/bellows4/bellows4_bellows4-ring0.png",width:90%),
      image("./asset//image/bellows4/bellows4sa_bellows4-ring0.png",width:90%)
    )
  )
]

#slide(title: "Summary")[
 - The background from bellows4 comes mostly from the DS flange edge.
 - The rate at$quad$ Ring 5: is 0.0106%  $equiv$ $03.21 times 10^(-8)$ per eot.
 - The rate at All Rings: is 0.0340%  $equiv$ $10.48 times 10^(-8)$ per eot.
]




#backup(text(size:50pt)[ Backup ])

#slide(title: "3D Plot")[
  #image("./asset/image/bellows4/bellows4-to-ring5-3d-small.png",width:60%);
]

/*

#slide(title: "Transverse Polarization")[
]

*/
