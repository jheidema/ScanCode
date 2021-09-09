#from matplotlib.collections import PolyCollection
import matplotlib
matplotlib.use('TkAgg')
from matplotlib.collections import PolyCollection
from matplotlib import colors as mcolors
import matplotlib.pyplot as plt
import numpy as np
import pandas

lowval=0
dz=0.5

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
    #return [(xlist[0], lowval), *zip(xlist, ylist), (xlist[-1], lowval)]


def cc(arg,shade):
    return mcolors.to_rgba(arg, alpha=shade)


minS = 6
maxS = 6
maxS=maxS+1
dS = maxS-minS
zs=list(range(minS,maxS))

#zs = [0]*(maxS-minS)
#for i in range(minS,maxS):
#    zs[i-minS] = i

ax = plt.figure(facecolor="white").add_subplot(projection='3d')
ax.grid(False)
ax.set_box_aspect(aspect=(1,1.5,1))
plt.subplots_adjust(left=0,bottom=0,right=1,top=1)
ax.w_xaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
ax.w_yaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
ax.w_zaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))

verts_0 = []
verts_1 = []
verts_2 = []

filelist = ["../fromRobert/8/GT/4.332gt.summary","../fromRobert/8/GT/4.533gt.summary","../fromRobert/8/GT/4.734gt.summary","../fromRobert/8/GT/4.935gt.summary","../fromRobert/8/GT/5.136gt.summary","../fromRobert/8/GT/5.337gt.summary","../fromRobert/8/GT/5.538gt.summary","../fromRobert/8/GT/5.739gt.summary","../fromRobert/8/GT/5.94gt.summary","../fromRobert/8/GT/6.141gt.summary","../fromRobert/8/GT/6.342gt.summary","../fromRobert/8/GT/6.543gt.summary","../fromRobert/8/GT/6.744gt.summary","../fromRobert/8/GT/6.945gt.summary","../fromRobert/8/GT/7.146gt.summary","../fromRobert/8/GT/7.347gt.summary","../fromRobert/8/GT/7.548gt.summary","../fromRobert/8/GT/7.749gt.summary","../fromRobert/8/GT/7.95gt.summary","../fromRobert/8/GT/8.151gt.summary","../fromRobert/8/GT/8.352gt.summary","../fromRobert/8/GT/8.553gt.summary","../fromRobert/8/GT/8.754gt.summary","../fromRobert/8/GT/8.955gt.summary","../fromRobert/8/GT/9.156gt.summary","../fromRobert/8/GT/9.357gt.summary","../fromRobert/8/GT/9.558gt.summary","../fromRobert/8/GT/9.759gt.summary","../fromRobert/8/GT/9.96gt.summary"]

#filelist = ["../fromRobert/8/GT/7.146gt.summary","../fromRobert/8/GT/7.347gt.summary"]

nf = len(filelist)

colnames = ["Energy", "spin", "Pn", "Pgam", "seven", "three", "nine", "five"]

energy = [0]*nf
s_sevens = [ [ 0 for i in range(nf) ] for j in range(maxS) ]
s_threes = [ [ 0 for i in range(nf) ] for j in range(maxS) ]
s_nines  = [ [ 0 for i in range(nf) ] for j in range(maxS) ]

#energy = [0]*nf
#s_sevens = [[0]*nf]*(9)
#s_threes = [[0]*nf]*(9)
#s_nines  = [[0]*nf]*(9)

for i in range(0,nf):
    with open(filelist[i]) as fin:
        rin=pandas.read_csv(fin,names=colnames,sep="\s+")
    en = rin.Energy.tolist()
    sev = rin.seven.tolist()
    thre = rin.three.tolist()
    nin = rin.nine.tolist()        

    energy[i] = en[0]
    
    nin = np.add(nin,sev)
    thre = np.add(thre,nin)

    for j in range(minS,maxS):
        s_sevens[j][i] = sev[j]/thre[j]+dz*(j-minS)
        #s_sevens[j-1][i] = sev[j-1] 
        s_threes[j][i] = thre[j]/thre[j]+dz*(j-minS)
        s_nines[j][i] = nin[j]/thre[j]+dz*(j-minS)
        

for i in range(minS,maxS):
    lowval = dz*(i-minS)
    verts_0.append(polygon_under_graph(energy, s_sevens[i]))
    #verts_1.append(polygon_under_graph(energy, s_nines[i]))
    verts_1.append(polygon_btw_graph(energy, s_nines[i], s_sevens[i]))
    verts_2.append(polygon_btw_graph(energy, s_threes[i], s_nines[i]))
    

poly = PolyCollection(verts_0, facecolors=cc("b",0.05), edgecolors=cc("b",1), linewidths=0.75)
ax.add_collection3d(poly, zs=zs, zdir='y')

poly = PolyCollection(verts_1, facecolors=cc("r",0.05), edgecolors=cc("r",1.0),linewidths=0.75)
ax.add_collection3d(poly, zs=zs, zdir='y')

poly = PolyCollection(verts_2, facecolors=cc("g",0.05), edgecolors=cc("g",1),linewidths=0.75)
ax.add_collection3d(poly, zs=zs, zdir='y')


#### Data Info ####
spins = [4, 5, 6, 7, 8]

#data_e =   [7.81, 7.37, 7.18, 6.88]
#data_gs =  [0.15, 0.4, 0.49, 0.74]
#data_gse = [0.1, 0.05, 0.06, 0.07]
#data_1 =   [0.94, 0.93, 0.99, 0.92]
#data_1e =  [0.35, 0.03, 0.03, 0.01]
#data_2 =   [1, 1, 1, 1]
#data_2e =  [0.02, 0, 0, 0.02]

data_e =   [8.31, 7.81, 7.37, 7.18, 6.88]
data_gs =  [0.35, 0.15, 0.4, 0.49, 0.74]
data_gse = [0.5, 0.1, 0.05, 0.06, 0.07]
data_1 =   [0.63, 0.94, 0.93, 0.99, 0.92]
data_1e =  [0.13, 0.35, 0.03, 0.03, 0.01]
data_2 =   [1, 1, 1, 1, 1]
data_2e =  [0.23, 0.02, 0, 0, 0.02]

#7.81, 7.37, 7.18, 6.88
err_gs = [[0.34, 0.11, 0.03, -0.2],[0.35, 0.13, 0.06, -0.18],[0.36, 0.13, 0.07, -0.16],[0.4, 0.15, 0.05, -0.2],[0.6, 0.44, 0.4, 0.18]]
err_1 =  [[-0.14, -0.12, -0.17, -0.1],[-0.05, -0.01, -0.06, 0.03],[-0.05, -0.01, -0.06, 0.02],[0.06, 0.07, 0.01, 0.08],[0.06, 0.07, 0.01, 0.08]]

###################

### Spin Vector ###

svec = [minS]*len(data_e)

###################

#ax.plot(data_e, svec, data_2, linestyle="None", marker="o",mfc="green",mec="black",label="3/2")
#ax.plot(data_e, svec, data_gs, linestyle="None", marker="o",mfc="blue",mec="black",label="7/2")
#ax.plot(data_e, svec, data_1, linestyle="None", marker="o",mfc="red",mec="black",label="9/2")

ax.plot(data_e, svec, data_gs, linestyle="None", marker="o",mfc="blue",mec="black",label="7/2")
ax.plot(data_e, svec, data_1, linestyle="None", marker="o",mfc="red",mec="black",label="9/2")
ax.plot(data_e, svec, data_2, linestyle="None", marker="o",mfc="green",mec="black",label="3/2")

if dS==1 :
    for j in range(0,4):
        ax.plot([data_e[j],data_e[j]],[svec[j],svec[j]],[data_2[j]+data_2e[j],data_2[j]-data_2e[j]],marker="_",color="green",mfc="green")
        ax.plot([data_e[j],data_e[j]],[svec[j],svec[j]],[data_gs[j]+data_gse[j],data_gs[j]-data_gse[j]],marker="_",color="blue",mfc="blue")
        ax.plot([data_e[j],data_e[j]],[svec[j],svec[j]],[data_1[j]+data_1e[j],data_1[j]-data_1e[j]],marker="_",color="red",mfc="red")

for i in range(minS+1,maxS):
    svec = [i]*len(data_e)
    offvec = [dz*(i-minS)]
    print(offvec)
    ax.plot(data_e, svec, np.add(data_2,offvec), linestyle="None", marker="o",mfc="green",mec="black")
    ax.plot(data_e, svec, np.add(data_gs,offvec), linestyle="None", marker="o",mfc="blue",mec="black")
    ax.plot(data_e, svec, np.add(data_1,offvec), linestyle="None", marker="o",mfc="red",mec="black")
    
    if i==6 :
     for j in range(0,4):
        ax.plot([data_e[j],data_e[j]],[svec[j],svec[j]],[data_2[j]+offvec[0]+data_2e[j],data_2[j]+offvec[0]-data_2e[j]],marker="_",color="green",mfc="green")
        ax.plot([data_e[j],data_e[j]],[svec[j],svec[j]],[data_gs[j]+offvec[0]+data_gse[j],data_gs[j]+offvec[0]-data_gse[j]],marker="_",color="blue",mfc="blue")
        ax.plot([data_e[j],data_e[j]],[svec[j],svec[j]],[data_1[j]+offvec[0]+data_1e[j],data_1[j]+offvec[0]-data_1e[j]],marker="_",color="red",mfc="red")



#for i in range(0,len(data_e)):
#    ax.plot([data_e[i], data_e[i]],[minS,maxS-1],[data_gs[i],data_gs[i]],color="blue",linestyle="--", linewidth=0.75)
#    ax.plot([data_e[i], data_e[i]],[minS,maxS-1],[data_1[i],data_1[i]],color="red",linestyle="--", linewidth=0.75)
#    #ax.plot([data_e[i], data_e[i]],[minS,maxS-1],[data_2[i],data_2[i]],color="",linestyle="--", linewidth=0.75)
#    
#    for j in range(0,len(spins)):
#        ax.plot([data_e[i], data_e[i]],[spins[j],spins[j]],[data_gs[i],data_gs[i]+err_gs[j][i]],color="black",linestyle="--", linewidth=1)
#        ax.plot([data_e[i], data_e[i]],[spins[j],spins[j]],[data_1[i],data_1[i]+err_1[j][i]],color="black",linestyle="--", linewidth=1)
#    
    
lowval = 0
ax.set_xlabel('Energy')
ax.set_ylabel('Spin')
ax.set_zlabel('Rel. Intensity')
ax.set_xlim(4, 10)
ax.set_ylim(minS-0.1,maxS-1+0.1)
#ax.set_zlim(lowval,dS*dz+1)
ax.set_zlim(lowval,3.5)

ax.set_zticklabels([])

ax.set_yticks(zs)
ax.set_zticks([0])
#ax.set_zscale('log')
#ax.zaxis.set_scale('log')

ax.legend(loc=[0.15,0.65],fontsize="x-large")

#plt.savefig("myplot.png")
plt.show()

#exit()