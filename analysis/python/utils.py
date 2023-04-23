import numpy as np
import matplotlib.pyplot as plt
import easyhist as eh

fp = np.array( [-1e5,-3700,2000,8500,14500,22200, 35000,1e6])
flm = (fp[1:]+fp[:-1])/2
flm = np.array([-5000,-850,5250,11500,18350,28600,37000])


def show_boundary(ax,lw=None,ls='--',alpha=0.3,**kw):
    for bo in fp[1:-2]:
        ax.axvline(bo,ls='--',lw=0.5,**kw)


def get_hist(f,ir6a=55, ir6b=80,ir4 = 40, ring=5,kind='epm',**kw):
    id = f"4IR{ir4}6A{ir6a}6B{ir6b}/plot/ring-{ring}-vz-{kind}"
    return eh.hist1d_from_root(f.Get(id))

def plot_vz_hist(ax,f,ir6a=55, ir6b=80,ir4 = 40, ring=5,kind='epm',**kw):
    h = get_hist(f,ir6a,ir6b,ir4,ring,kind)
    kk = "$e^{\pm}$" if kind == "epm" else "$\gamma$"
    h.plot(ax,steps=True,filled=True,ebar=False,lw=1.5,label=f"4 IR {ir4} 6A {ir6a} 6B {ir6b} ({kk}) ")
    #show_boundary(ax)

def get_data(data,ir6a=55, ir6b=80,ir4 = 40,kind='epm',**kw):
    T = 'T;2' if kind=='epm' else "T;1"
    id = f"4IR{ir4}6A{ir6a}6B{ir6b}/data/{T}/all-ring-counts-{kind}"
    return data[id].array()



def moller_fraction(count, primary=1e8):
    return count/(primary/3300)

mf = lambda p: [moller_fraction(x)*100 for x in p]

def plot_moller_rate(ax,data,ir6a=55, ir6b=80,ir4 = 40, ring=5,kind='photon',**kw):
    dt = get_data(data,ir6a, ir6b, ir4 ,kind)
    ax.plot(flm[:-1],mf(dt[:,ring])[:-1],'o-',label=f"4:{ir4} 6A:{ir6a} 6B:{ir6b} R:{ring}")
    ax.set_ylabel("Moller rate \%"); ax.set_xlabel("vz[mm]");
    #show_boundary(ax)
                  

def print_table(data,ir6a,ir6b,ir4,kind,**kw):
    dt = np.array(get_data(data,ir6a,ir6b,ir4,kind))
    print("   (vzmin  -     vzmax]    ",end="")
    for ring in range(len(dt[0])):
        print(f"Ring{ring} (moller%)  ",end="")
    print("")
    print("-"*130)
    for pb in range(1,len(fp)-1):
        print(f"{fp[pb-1]:>10} - {fp[pb]:>10} ",end="")
        rowtot = np.sum(dt[pb-1])
        for ring in range(len(dt[pb-1])):
            mf = moller_fraction(dt[pb][ring],1e8)*100
            print(f"  {dt[pb][ring]:>5} ({mf:.3f})% ",end="")
        print("")
    print("-"*130)
    print(f"{fp[0]:>10} - {fp[-1]:>10}",end="");
    for ring in range(len(dt[0])):
        tot = np.sum(dt[:,ring])
        print(f"  {tot:>5} ({moller_fraction(tot,1e8)*100:.3f})% ",end="")
    print("")                      
    print("-"*130)
