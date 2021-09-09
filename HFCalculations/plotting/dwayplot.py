#from matplotlib.collections import PolyCollection
import matplotlib
matplotlib.use('TkAgg')
from matplotlib.collections import PolyCollection
from matplotlib import colors as mcolors
import matplotlib.pyplot as plt
import numpy as np
import pandas

lowval=np.log10(1E-8)

def polygon_under_graph(xlist, ylist):
    """
    Construct the vertex list which defines the polygon filling the space under
    the (xlist, ylist) line graph.  Assumes the xs are in ascending order.
    """
    return [(xlist[0], lowval), *zip(xlist, ylist), (xlist[-1], lowval)]

def polygon_btw_graph(xlist, ylist, blist):
    """
    Construct the vertex list which defines the polygon filling the space under
    the (xlist, ylist) line graph.  Assumes the xs are in ascending order.
    """
    blist.reverse()
    xblist=xlist.copy()
    xblist.reverse()
    return [*zip(xlist, ylist), *zip(xblist,blist)]

def cc(arg,shade):
    return mcolors.to_rgba(arg, alpha=shade)


#filelist = ["../output/TSV/Doorway_J4m.tsv","../output/TSV/Doorway_J5m.tsv","../output/TSV/Doorway_J6m.tsv","../output/TSV/Doorway_J7m.tsv","../output/TSV/Doorway_J8m.tsv"]
#filelist = ["../output/TSV/Doorway_J4m.tsv","../output/TSV/Doorway_J5m.tsv","../output/TSV/Doorway_J6m.tsv","../output/TSV/Doorway_J7m.tsv"]
#filelist = ["../output/TSV/Doorway_J5m.tsv","../output/TSV/Doorway_J6m.tsv","../output/TSV/Doorway_J7m.tsv","../output/TSV/Doorway_J8m.tsv"]
filelist = ["../output/TSV/Doorway_J5m.tsv","../output/TSV/Doorway_J6m.tsv"]
#filelist = ["../output/TSV/Doorway_J5m.tsv"]

initS = 5

nf = len(filelist)

zs=list(range(0,nf))
ax = plt.figure(facecolor="white").add_subplot(projection='3d')
plt.subplots_adjust(left=0,bottom=0,right=1,top=1)
ax.grid(False)
ax.set_box_aspect(aspect=(2,1,1))
ax.w_xaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
ax.w_yaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
ax.w_zaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))

verts_0 = []
verts_1 = []
verts_2 = []

colnames = ["Energy", "seven", "three", "nine"]

for i in range(0,nf):
    with open(filelist[i]) as fin:
        rin=pandas.read_csv(fin,names=colnames,delimiter="\t")
    energy = rin.Energy.tolist()
    sevens = rin.seven.tolist()
    threes = rin.three.tolist()
    nines  = rin.nine.tolist()        
    
    sevens = np.multiply(sevens,1.0e8)
    threes = np.multiply(threes,1.0e8)
    nines  = np.multiply(nines,1.0e8)

    nines  = np.add(sevens,nines)
    threes = np.add(threes,nines)
    
    sevens = np.log10(sevens)
    lowval = -1.0*min(sevens)
    sevens = np.add(sevens,lowval)

    threes = np.log10(threes)
    threes = np.add(threes,lowval)

    nines = np.log10(nines)
    nines = np.add(nines,lowval)

    #lowvals = [min(sevens),min(threes),min(nines)]
    #lowval = min(lowvals)
    lowval = min(sevens)

    verts_0.append(polygon_under_graph(energy, sevens))
    verts_1.append(polygon_btw_graph(energy, nines.tolist(), sevens.tolist()))
    verts_2.append(polygon_btw_graph(energy, threes.tolist(), nines.tolist()))
    
    #verts_1.append(polygon_under_graph(energy, threes))
    #verts_2.append(polygon_under_graph(energy, nines))

#print(verts)

zs = [x+initS for x in zs]
#zs = [x+initS+0.1 for x in zs]
#print(zs)
poly1 = PolyCollection(verts_0, facecolors=cc("b",0.9), edgecolors=cc("b",0.5), linewidths=0.2,label="7/2")
ax.add_collection3d(poly1, zs=zs, zdir='y')

poly2 = PolyCollection(verts_1, facecolors=cc("r",0.9), edgecolors=cc("r",0.5),linewidths=0.2,label="9/2")
ax.add_collection3d(poly2, zs=zs, zdir='y')

#zs = [x-0.05 for x in zs]
#print(zs)

poly3 = PolyCollection(verts_2, facecolors=cc("g",0.9), edgecolors=cc("g",0.5),linewidths=0.2,label="3/2")
ax.add_collection3d(poly3, zs=zs, zdir='y')

#zs = [x-0.05 for x in zs]
#print(zs)



ax.set_xlabel('Energy [MeV]')
ax.set_ylabel('Spin')
ax.set_zlabel('Intensity [a.u.]')
ax.set_xlim(6.5, 8.5)
ax.set_ylim(initS-0.1, initS+nf-0.9)
ax.set_zlim(lowval,10)
ax.set_xticks(np.arange(6.5,8.5,step=0.5))
ax.set_yticks(np.arange(initS,initS+nf,step=1))
ax.set_zticks([])
#ax.set_zticks(np.arange(0,15,step=5))


#ax.set_zscale('log')
#ax.zaxis.set_scale('log')

#ax.legend([poly1,poly2,poly3])

#plt.savefig("myplot.png")
plt.show()

#exit()