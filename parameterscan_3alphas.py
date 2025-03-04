import numpy as np
import subprocess
import matplotlib.pyplot as plt
import pdb
import os
import sys
import threading

# Parameters
# TODO adapt to what you need (folder path executable input filename)
executable = 'physnum_ex1'  # Name of the executable (NB: .exe extension is required on Windows)
repertoire = r"./"
os.chdir(repertoire)

input_filename = 'configuration.in.example'  # Name of the input file


nsteps = np.geomspace(4e4, 2e5, 55).astype(np.int32)
# print(nsteps[-1])
# nsteps = np.array([int(sys.argv[1])]) # TODO change
nsimul = len(nsteps)  # Number of simulations to perform

tfin = 259200  # TODO: Verify that the value of tfin is EXACTLY the same as in the input file

dt = tfin / nsteps


paramstr = 'nsteps'  # Parameter name to scan
param = nsteps  # Parameter values to scan

alphaparamstr = 'alpha'
alphaparam = [0.5]
nalpha = len(alphaparam)

error = np.zeros(nsimul)

# Simulations
convergence_list_x = np.zeros((nsimul, nalpha))
convergence_list_y = np.zeros((nsimul, nalpha))

def simulate(idx):
    for i, alpha in enumerate(alphaparam):
        output_file = f"{paramstr}={param[idx]}_{alpha}.out"
        cmd = f"{repertoire}{executable} {input_filename} {paramstr}={param[idx]:.15g} {alphaparamstr}={alpha} output={output_file}"
        print(cmd)
        subprocess.run(cmd, shell=True)
        data = np.loadtxt(output_file)
        convergence_list_x[idx, i] = data[-1, 3]
        convergence_list_y[idx, i] = data[-1, 4]
        print(f'Done. ({paramstr}={param[idx]}, {alphaparamstr}={alpha})')

threads = []
for i in range(nsimul):
    th = threading.Thread(target=simulate, args=(i,))
    threads.append(th)
    threads[i].start()

for th in threads:
    th.join()

lw = 1.5
fs = 16
markersize=13
legendfs = 12


style = ["g^", "r+", "b*"]
norder = int(sys.argv[1])  # Modify if needed
skip = int(sys.argv[2])

plt.figure()
for i in range(nalpha):
    plt.plot((dt**norder)[skip:], convergence_list_x[skip:,i], style[i], linewidth=lw, label=f"$\\alpha={alphaparam[i]}$")
plt.xlabel(r"$(\Delta t \mathrm { [s] } )" + f" ^{norder}$", fontsize=fs)
plt.ylabel(r'$x\'(t_{fin})$', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)
plt.legend(fontsize=legendfs)
plt.show()

plt.figure()
for i in range(nalpha):
    plt.plot((dt**norder)[skip:], convergence_list_y[skip:,i], style[i], linewidth=lw, label=f"$\\alpha={alphaparam[i]}$")
plt.xlabel(r"$(\Delta t \mathrm { [s] } )" + f" ^{norder}$", fontsize=fs)
plt.ylabel(r'$y\'(t_{fin})$', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)
plt.legend(fontsize=legendfs)
plt.show()

