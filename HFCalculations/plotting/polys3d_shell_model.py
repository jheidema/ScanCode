from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import PolyCollection
import matplotlib.pyplot as plt
from matplotlib import colors as mcolors
from matplotlib import patches as mpatches
import numpy as np
import ROOT


fig = plt.figure(figsize=(12,9))
ax = fig.gca(projection='3d')


def cc(arg):
    return mcolors.to_rgba(arg, alpha=0.1)

if __name__ == '__main__':
    f = ROOT.TFile("yaml_levels.root")
    hp = f.Get("Levels_pos_84Ge")
    hn = f.Get("Levels_neg_84Ge")
    xsp = np.arange(0, 15, 0.1)
    zsp = np.arange(0, 10, 1)
    verts_p = []
    for z in zsp:
        ys = np.arange(len(xsp),dtype=float)
        bin_z = int(2*z+1)
        h_proj = hp.ProjectionY("pr_pos_"+str(z),bin_z,bin_z+1)
        for x in xsp:
            xbin = h_proj.FindBin(x)
            ys[xbin-1] = h_proj.GetBinContent(xbin)
        ys[0] = 0
        ys[-1] = 0
        verts_p.append(list(zip(xsp, ys)))

    poly_p = PolyCollection(verts_p, facecolors=cc("r"), edgecolors=("red",))
    ax.add_collection3d(poly_p, zs=zsp, zdir='y')

    xsn = np.arange(0, 15, 0.1)
    zsn = np.arange(0.5, 10.5, 1)
    verts_n = []
    for z in zsn:
        ys = np.arange(len(xsn),dtype=float)
        bin_z = int(2*z+1)
        h_proj = hn.ProjectionY("pr_neg_"+str(z),bin_z,bin_z+1)
        for x in xsn:
            xbin = h_proj.FindBin(x)
            ys[xbin-1] = h_proj.GetBinContent(xbin)
        ys[0] = 0
        ys[-1] = 0
        verts_n.append(list(zip(xsn, ys)))

    poly_n = PolyCollection(verts_n, facecolors=cc("b"), edgecolors=("blue",))
    ax.add_collection3d(poly_n, zs=zsn, zdir='y')

    ax.set_xlabel('Excitation energy (MeV)')
    ax.set_xlim3d(0, 15)
    ax.set_ylabel('J')
    ax.set_ylim3d(0, 10)
    ax.set_zlabel('Levels per 100 keV')
    ax.set_zlim3d(0, 50)
    red_patch = mpatches.Patch(facecolor=cc('r'), edgecolor='red', label='positive parity')
    blue_patch = mpatches.Patch(facecolor=cc('b'), edgecolor='blue', label='negative parity')
    plt.legend(handles=[red_patch,blue_patch])

    plt.savefig("ld_shell_model_matplotlib.png")
