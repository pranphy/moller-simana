#import "@local/tymer:0.1.0": *
#show: slides.with(
    authors: [Prakash Gautam\   #text(font:"Noto serif", fill: white.darken(71%))[〈 प्रकाश गौतम|  پرکاش گوتم 〉]],
    institute: Institute("University of Virginia",short:"UVa"),
    short-authors: "P. Gautam",
    //title: "Spectrometer, Beamline, Simulation and Timeline",
    title: "Implementation of Custom Rotation Order in remoll GDML",
    location: "MOLLER Simulation Meeting",
    //thm: (fg:black.darken(00%),bg:white.lighten(00%),ac: orange),
    //date: "2024-01-17",
    aspect: 16/9
)

#titlepage()


#slide(title: "Rotation Order: Current Implementation")[
  - When geometry is imported with geometry we can specify the rotation of physical volume with `rotation` tag
  ```xml
     <rotation unit="degree" x="45" y="25" z="90"/>
  ```
  this always does the rotation $R_x(45) arrow R_y(25) arrow R_z(90)$.
  - As far as I can tell there is not other way to specify custom rotation order in standard GDML parser of Geant4.
  - The rotation order is hardcoded
  ```cpp
    // G4GDMLReadParamvol.cc#L1207 geant4.10.07.p04
    parameter.pRot = new G4RotationMatrix();

    parameter.pRot->rotateX(rotation.x());
    parameter.pRot->rotateY(rotation.y());
    parameter.pRot->rotateZ(rotation.z());
  ```
]


#slide(title: "Custom Rotation Order")[
  #v(0.7em)
  - I created a new tag that can beused in remoll GDML files `<orotation>` for ordered rotation.
  ```xml
     <orotation unit="degree" x="45" y="25" z="90" order="zxy"/>
  ```
  this would allow the rotation $R_z(90) arrow R_x(45) arrow R_y(25)$. The value of order parameter can be any of the six permutations of string "xyz".

  #text(font:"Noto Sans")[Example]:
  #grid(columns:(60%,40%),
  [
    #v(0.1em)
    - Yellow: Unrotated
    - Green: $R_x(90) arrow R_y(45) arrow R_z(0)$
    - Orange: $R_y(45) arrow R_x(90) arrow R_z(0)$
    - #text(font:"Noto Serif")[_there is a translation between them_]
  ],
  image("./asset/image/remoll-rotation-order.png", width:90%)
  )
]


#slide(title: "Remoll GDML Parser")[
  - Remoll only parses the auxiliary tags in GDML by itself and the rest is done by standard `G4GDMLParser`.
  ```xml
      <auxiliary auxtype="SensDet" auxvalue="coilDet"/>
      <auxiliary auxtype="DetNo" auxvalue="3001"/>
      <auxiliary auxtype="Color" auxvalue="magenta"/>
  ```
  - In `remollDetectorConstruction.hh` there is an object of `G4GDMLParser`.
  ```cpp
    G4GDMLParser fGDMLParser; //remollDetectorConstruction.hh 
    fGDMLParser.Read(fGDMLFile,fGDMLValidate); //remollDetectorConstruction.cc 
    ...
    ...
 ```
 - The constructor of G4GDMLparser takes an optional argument of `G4GDMLReadStructure`. The object
 ```cpp
    G4GDMLParser();
    G4GDMLParser(G4GDMLReadStructure*);
 ```
]

#slide(title: "The hierarchy of class G4GDMLReadStructure")[
  #grid(columns:(70%,30%),
    [ 
      - G4GDMLReadStructure has two three virtual functions that can be overridden.
      - G4GDMLReadParamvol reads the parameters of `<phyvol>` tag.
      - G4GDMLReadStructure parses `<structure>` tags etc.
    ],
    image("./asset/image/upto-G4GDMLReadStructure.png",width:92%)
  )
]


#slide(title: "Creating a new tag in ")[
  #grid(columns:(70%,30%),
    [ 
      - A new class `remollGDMLReadStructure` is derived from `G4GDMLReadStructure`
      - It overrides some methods inside G4GDMLReadParamvol and some from `G4GDMLReadStructure`.
      - In the standard Geant4 implementation the rotation angles are parsed as vector of three components so the order of rotation is not possible.
      - The new implementation for `<orotation>` parses the angles and order parameter to create a rotation matrix, thus preserving order information for further rotation.
    ],
    image("./asset/image/remollGDMLReadStructure-dependency.png",width:92%)
  )
]



#slide(title: "Summary")[
  - Current GDMLParser from geant4 doesn't allow custom order rotation of `<phyvol>` and always does the order $R_x arrow R_y arrow R_z$.
  - A new tag is now available `<orotation>` that behaves in the same way as the `<rotation>` tag except it also takes an optional argument `order` which can have values that are any permutation of string "xyz".  (Defaults to "xyz").
  ```xml
     <orotation unit="degree" x="45" y="25" z="90" order="zxy"/>
  ```
  - Every instance of `<rotation>` can in principle be replaced by `<orotation>` without any impact.
  - It has been tested by Xiang to change rotation order of Quartz tiles.
  - More tests needed to make sure it doesn't affect any other functionality.
  - A new header/source file pair will be added in remoll which can be compiled with the standard procedure without any further dependency.
]




/*
#backup(text(size:50pt)[ Backup ])

#slide(title: "Transverse Analyzing power")[
]


#slide(title: "Transverse Polarization")[
]

*/
