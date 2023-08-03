// ==============================
// ======== GLOBAL STATE ========
// ==============================

#let section = state("section", none)
#let subslide = counter("subslide")
#let logical-slide = counter("logical-slide")
#let repetitions = counter("repetitions")
#let global-theme = state("global-theme", none)
#let handout-mode = state("handout-mode", false)

#let new-section(name) = section.update(name)

#let Institute(full, short:none) = {
  (
    full: full,
    short: if short==none { full } else { short }
  )
}

#let alert(body,fill: orange) = {
  align(center,box(fill: fill.darken(20%), outset: 1em, width: 95%,radius: 10pt,align(center,body)))
}

#let Author(full-name, abbr:none) = {
  (full-name: full-name, abbr: "P. Gautam")
}

// ================================
// ======== SLIDE CREATION ========
// ================================

#let slide(
    max-repetitions: 10,
    theme-variant: "default",
    override-theme: none,
    ..args
) = {

    locate( loc => {
        if counter(page).at(loc).first() > 1 {
            pagebreak(weak: true)
        }
    })
    logical-slide.step()
    locate( loc => {
        subslide.update(1)
        repetitions.update(1)

        let slide-content = global-theme.at(loc).at(theme-variant)
        if override-theme != none {
            slide-content = override-theme
        }
        let slide-info = args.named()
        let bodies = args.pos()

        for _ in range(max-repetitions) {
            locate( loc-inner => {
                let curr-subslide = subslide.at(loc-inner).first()
                if curr-subslide <= repetitions.at(loc-inner).first() {
                    if curr-subslide > 1 { pagebreak(weak: true) }

                    slide-content(slide-info, bodies)
                }
            })
            subslide.step()
        }
    })
}

#let titlepage() = { slide(theme-variant: "title slide") }

// ===============================
// ======== DEFAULT THEME ========
// ===============================



#let slide-number(obj) = {
  let current = locate(loc => {obj.at(loc).at(0) })
  let total = locate(loc => {obj.final(loc).at(0) })
  [#(current)/#(total)]
}

#let two_col(bodies) = {
  let cell = rect.with(radius: 4pt, inset: 1em , outset: 0em,stroke: none)
  grid( columns: (4fr, 4fr), column-gutter: 0.01fr,
      cell(bodies.at(0)), cell(bodies.at(1))
  )
}

#let slides-default-theme() = data => {
    let title-slide(slide-info, bodies) = {
        if bodies.len() != 0 {
            panic("title slide of default theme does not support any bodies")
        }

        align(center + horizon)[
            #block(
                stroke: ( y: 1mm + data.thm.ac),
                inset: 1em,
                breakable: false,
                [
                    #text(1.3em)[*#data.title*] \
                    #{
                        if data.subtitle != none {
                            parbreak()
                            text(.9em)[#data.subtitle]
                        }
                    }
                ]
            )
            #set text(size: .8em)
            #grid(
                columns: (1fr,) * calc.min(data.authors.len(), 3),
                column-gutter: 1em,
                row-gutter: 1em,
                ..data.authors
            )
            #data.institute.full
            #v(1em)
            #data.location \
            #data.date
        ]
    }


    let default(slide-info, bodies,content:none) = {
        if bodies.len() != 1 and content == none {
            panic("default variant of default theme only supports one body per slide")
        }
        let body = bodies.first()


        let decoration(position, body) = {
            let border = 1mm + color
            let strokes = (
                header: ( bottom: border ),
                footer: ( top: border )
            )
            block(
                stroke: strokes.at(position),
                width: 100%,
                fill: rgb("ff8d1f"),
                inset: .3em,
                text(.5em, body)
            )
        }


        // header
        //decoration("header", section.display())

        if "title" in slide-info {
            block(
                width: 100%, inset: (x: 1em, y: 0.5em), breakable: false, fill: data.thm.bg.lighten(20%),
                outset: 0.6em, height: 6%,
                heading(level: 1, slide-info.title)
            )
        }

        v(1fr)
        if content == none {
          block( width: 100%, inset: (x: 1em), breakable: false, outset: 0em, body)
        } else { content(bodies) }
        v(2fr)

        let cbox(body,fill: orange, width: 25%, al: center) = {
          box(inset:(x: 2pt, y: 1pt), height: 1.1em, width: width,fill: fill,
          align(al,body))
        }
        //let cbox(body,fill:none) = { body }

        let footer() = {
            block(width:100%, fill: data.thm.bg.darken(20%))[
              #text(fill:data.thm.fg.darken(30%), size:12pt)[
              #h(0.08fr)
              #cbox(fill: data.thm.bg.lighten(20%),[
                #(data.short-authors.at(0)) (#data.institute.short)
              ])
              #h(1fr)
              //#cbox(data.date)
              #cbox(width: 50%, fill: none,[#text(data.thm.fg.darken(50%),data.title) ])
              #h(1fr)
              #cbox(fill: data.thm.bg.lighten(20%),al: right,[#data.date #h(5em)  #slide-number(logical-slide)])]
              #h(0.02fr)
            ]
        }
        footer()
    }
    (
        "title slide": title-slide,
        "default": default,
    )
}

// ===================================
// ======== TEMPLATE FUNCTION ========
// ===================================

#let slides(
    title: none,
    authors: none,
    institute: none,
    location: none,
    subtitle: none,
    short-title: none,
    short-authors: none,
    date: datetime.today().display(),
    theme: slides-default-theme(),
    aspect-ratio: "16-9",
    handout: false,
    thm: (fg:white,bg:rgb("282a36"),ac: orange),
    body
) = {

    set page(
        //paper: "presentation-" + aspect-ratio,
        width: 16*1.70cm,
        height: 9*1.70cm,
        margin: 0pt,
        fill: thm.bg
    )

    let data = (
        title: title,
        authors: if type(authors) == "array" {
            authors
        } else if type(authors) in ("string", "content", "none") {
            (authors, )
        } else {
            panic("if not none, authors must be an array, string, or content.")
        },
        short-authors : if type(short-authors) in ("none") {
             (authors,)
        } else if type(short-authors) == "array" {
          short-authors
        } else if type(short-authors) in ("string","content"){
          (short-authors, )
        } else { panic("So much panic short-authors") },
        subtitle: subtitle,
        short-title: short-title,
        date: date,
        institute: institute,
        location: location,
        thm: thm,
    )
    let the-theme = theme(data)
    global-theme.update(the-theme)
    handout-mode.update(handout)

    set text(font: "cmss10", size: 20pt, fill: thm.fg)

    set list(marker: ([▶],[•], [--]))


    body
}
