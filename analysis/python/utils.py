import numpy as np
import matplotlib.pyplot as plt
import easyhist as eh


fpo = np.array( [-1e5,-3700,2000,8500,14500,22200, 35000,1e6])
fp = fpo
fmo = np.array([-5000,-850,5250,11500,18350,28600,37000])
fpl = np.array( [-1e5,-3700,2000,8500,14500,18932,19320,22200, 35000,1e6]) # hack to accomodate longer partition on collar2 region
flm = (fp[1:]+fp[:-1])/2
flmo = np.array([-5000,-850,5250,11500,18350,19350,20000,28600,37000])
flm = flmo

to_degree = lambda x : x*180/3.14159;
to_radian = lambda x : x*3.14159/180;

def show_boundary(ax,lw=None,ls='--',alpha=0.3,**kw):
    for bo in fp[1:-2]:
        ax.axvline(bo,ls='--',lw=0.5,**kw)


def up_read_hist_1d(data,directory,name):
    hist = data[f"{directory}/{name}"]
    be = hist.axis().edges()
    H = hist.values()
    err = hist.errors()


    xlabel = hist.member('fXaxis').member('fTitle')
    ylabel = hist.member('fYaxis').member('fTitle')
    title = hist.member('fTitle')
    attrib = dict(title=title,xlabel=xlabel,ylabel=ylabel)

    ha = eh.Hist1D()
    ha.construct(H,be,err,attrib)
    return ha

def get_hist(data,ir6a=55, ir6b=80,ir4 = 40, ring=5,kind='epm',field="",id=None,**kw):
    id = id or f"4IR{ir4}6A{ir6a}6B{ir6b}{field}"
    directory = f"{id}/plot"
    name = f"ring-{ring}-vz-{kind}"
    return up_read_hist_1d(data,directory,name)

def plot_vz_hist(ax,data,ir6a=55, ir6b=80,ir4 = 40, ring=5,kind='epm',field="",id=None,**kw):
    h = get_hist(data,ir6a,ir6b,ir4,ring,kind,field,id)
    kk = "$e^{\pm}$" if kind == "epm" else "$\gamma$"
    label = id or f"4 IR {ir4} 6A {ir6a} 6B {ir6b} {field} ({kk}) "
    h.plot(ax,steps=True,filled=True,ebar=False,lw=1.5,label=label)
    #show_boundary(ax)


def up_read_hist_2d(data,directory,name):
    hist = data[f"{directory}/{name}"]
    xe = hist.member('fXaxis').edges()
    ye = hist.member('fYaxis').edges()
    H = hist.values()


    xlabel = hist.member('fXaxis').member('fTitle')
    ylabel = hist.member('fYaxis').member('fTitle')
    title = hist.member('fTitle')
    attrib = dict(title=title,xlabel=xlabel,ylabel=ylabel)

    ha = eh.Hist2D((None,None))
    ha.H, ha.xe, ha.ye, ha.attrib = H,xe,ye,attrib
    return ha


def get_data(data,ir6a=55, ir6b=80,ir4 = 40,kind='epm',field="",id=None,**kw):
    l,h = 1,2
    if id == 'col2-cfg06':   # Another hack here to read longer ones because I made a partition later
        l,h = 3,4            # I kind of will keep things organized later. But it is what it is;  LOL
    T = f'T;{h}' if kind.endswith('epm') else f"T;{l}"
    id = id or f"4IR{ir4}6A{ir6a}6B{ir6b}{field}";
    directory = f"{id}/data/{T}/all-ring-counts-{kind}"
    return data[directory].array()



def moller_fraction(count, primary=1e8):
    return count/(primary/3300)

mf = lambda p: [moller_fraction(x)*100 for x in p]

def plot_moller_rate(ax,data,ir6a=55, ir6b=80,ir4 = 40, ring=5,kind='photon',field="",id=None,nevt=1e8,flm=fmo,**kw):
    dt = np.array(get_data(data,ir6a, ir6b, ir4 ,kind,field,id))
    mf = lambda p : moller_fraction(p,nevt)*100
    tot = sum(mf(dt[:,ring]))
    kk = "$e^{\pm}$" if kind == "epm" else "$\gamma$"
    labelid = id or f"[{kk}]IR 4 {ir4} 6A {ir6a} 6B {ir6b}"
    ax.plot(flm[:-1],mf(dt[:,ring])[:-1],'o-',label=f"{labelid}  R:{ring} {field} ({tot:.2f}\%)")
    ax.set_ylabel("Moller rate \%"); ax.set_xlabel("vz[mm]");
    #ax.set_ylim(-0.05,0.8)
    #show_boundary(ax)

def plot_hitr(ax,data,ir4=25,det=66,kind='epm'):
    pdr = 'plots' if ir4==25 else 'plot'
    kk = r"$\gamma$" if kind=='photon' else r'$e^{\pm}$'
    h1 = up_read_hist_1d(data,f"IR{ir4}NO6A6B/{pdr}",f"r-hist-hitting-{det}-{kind}").normalize()
    h1.plot(ax,ebar=False,filled=True,steps=True,lw=1.5,label=f"4IR {ir4} {kk}")
    ax.set_ylabel('PDF');

def plot_hitn(ax,data,ir4=25,det=66,kind='epm',yscale=1):
    pdr = 'plots' if ir4==25 else 'plot'
    kk = r"$\gamma$" if kind=='photon' else r'$e^{\pm}$'
    rhist = up_read_hist_1d(data,f"IR{ir4}NO6A6B/{pdr}",f"r-hist-hitting-{det}-{kind}")
    rhist.H  /= yscale
    rhist.plot(ax,ebar=False,filled=True,steps=True,lw=1.5,label=f"4IR {ir4} {kk}")
    ax.set_ylabel('PDF');

def print_table(data,ir6a=55,ir6b=80,ir4=25,kind='epm',field="",id=None,rings=[0,5],nevt=1e8,fp=fpo,**kw):
    dt = np.array(get_data(data,ir6a,ir6b,ir4,kind,field,id))
    dl = 10+ 20*(len(rings)+1)
    print("   (vzmin  -     vzmax]     ",end="")
    for ring in rings:
        print(f"Ring{ring} (moller%)    ",end="")
    print("")
    print("-"*dl)
    for pb in range(0,len(fp)-1):
        print(f"{fp[pb]:>10} - {fp[pb+1]:>10} ",end="")
        rowtot = np.sum(dt[pb])
        for ring in rings:
            mf = moller_fraction(dt[pb][ring],nevt)*100
            print(f"  {dt[pb][ring]:>6} ({mf:6.3f})% ",end="")
        print("")
    print("-"*dl)
    print(f"{fp[0]:>10} - {fp[-1]:>10}",end="");
    for ring in rings:
        tot = np.sum(dt[:,ring])
        print(f"  {tot:>6} ({moller_fraction(tot,nevt)*100:6.3f})% ",end="")
    print("")
    print("-"*dl)

def savefig(fig,name,prefix="",dir="asset/image/nb",format="png"):
    filename = f"{dir}/{prefix}_{name}.{format}"
    fig.savefig(filename);
    print(f"{filename} Written")

sixau1  = np.array( [
    [49.836,    -07.792],
    [49.836,    -24.000],
    [95.250,    -45.870],
    [95.250,    -35.113],
    [91.821,    -35.113],
    [91.821,    -26.985],
    [95.250,    -26.985],
    [95.250,    -25.080],
    [70.787,    -25.080],
    [70.787,    -07.792],
    [49.836,    -07.792],
])
sixau1

sixad1 = np.array([
    [50.277, 29.851],
    [50.277, 07.792],
    [70.787, 07.792],
    [70.787, 25.080],
    [95.250, 25.080],
    [95.250, 26.985],
    [91.821, 26.985],
    [91.821, 35.113],
    [95.250, 35.113],
    [95.250, 45.870],
    [93.046, 50.447],
    [50.277, 29.851],
])

sixau1c  = np.array( [
    [49.836,     02.000],
    [49.836,    -24.000],
    [95.250,    -45.870],
    [95.250,    -35.113],
    [91.821,    -35.113],
    [91.821,    -26.985],
    [95.250,    -26.985],
    [95.250,    -25.080],
    [70.787,    -25.080],
    [70.787,     02.200],
    [49.836,     02.000],
])

sixad1c = np.array([
    [50.277, 29.851],
    [50.277, -2.000],
    [70.787, -2.000],
    [70.787, 25.080],
    [95.250, 25.080],
    [95.250, 26.985],
    [91.821, 26.985],
    [91.821, 35.113],
    [95.250, 35.113],
    [95.250, 45.870],
    [93.046, 50.447],
    [50.277, 29.851],
])
def rotate(x,y,angle):
    return ( x * np.cos(angle)  - y * np.sin(angle), x * np.sin(angle) + y * np.cos(angle) )

def plot_sixa(ax,close=False):
    for angle in np.linspace(0,2*np.pi,8):
        (cu,cd) = (sixau1,sixad1) if close is False else (sixau1c,sixad1c)
        xu,yu = rotate(cu[:,0],cu[:,1],angle)
        xd,yd = rotate(cd[:,0],cd[:,1],angle)
        ax.plot(xu,yu,lw=-1.0,alpha=0.6)
        ax.plot(xd,yd,lw=-1.0,alpha=0.6)

def add_value_labels(ax,labels, showval=True,spacing=5):
    for k,rect in enumerate(ax.patches):
        y_value = rect.get_height()
        x_value = rect.get_x() + rect.get_width() / 2
        space = spacing
        va = 'bottom'
        if y_value < 0:
            space *= -1
            va = 'top'

        label = "({:.3f})".format(y_value) if showval else ""
        ax.annotate( f"{labels[k].replace('epm',' ')}{label}", (x_value, y_value), xytext=(0, space), textcoords="offset points", ha='center', rotation='vertical', va=va)
