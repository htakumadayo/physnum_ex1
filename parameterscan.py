import numpy as np
import subprocess
import matplotlib.pyplot as plt
import pdb
import os
import sys

# Parameters
# TODO adapt to what you need (folder path executable input filename)
executable = 'physnum_ex1'  # Name of the executable (NB: .exe extension is required on Windows)
repertoire = r"./"
os.chdir(repertoire)

input_filename = 'configuration.in.example'  # Name of the input file


#nsteps = np.geomspace(9e3, 12e4, 50).astype(np.int32)
# print(nsteps[-1])
nsteps = np.array([int(sys.argv[1])]) # TODO change
nsimul = len(nsteps)  # Number of simulations to perform

tfin = 259200  # TODO: Verify that the value of tfin is EXACTLY the same as in the input file

dt = tfin / nsteps


paramstr = 'nsteps'  # Parameter name to scan
param = nsteps  # Parameter values to scan

# Simulations
outputs = []  # List to store output file names
convergence_list_x = []
convergence_list_y = []
for i in range(nsimul):
    output_file = f"{paramstr}={param[i]}.out"
    outputs.append(output_file)
    cmd = f"{repertoire}{executable} {input_filename} {paramstr}={param[i]:.15g} output={output_file}"
    # cmd = f"{executable} {input_filename} {paramstr}={param[i]:.15g} output={output_file}"
    print(cmd)
    subprocess.run(cmd, shell=True)
    print('Done.')

error = np.zeros(nsimul)

for i in range(nsimul):  # Iterate through the results of all simulations
    data = np.loadtxt(outputs[i])  # Load the output file of the i-th simulation
    t = data[:, 0]

    vx = data[-1, 1]  # final position, velocity, energy
    vy = data[-1, 2]
    xx = data[-1, 3]
    yy = data[-1, 4]
    En = data[-1, 5]
    convergence_list_x.append(xx)
    convergence_list_y.append(yy)
    # TODO compute the error for each simulation
    error[i] =  abs(3.507e8 - xx); 

lw = 1.5
fs = 16
markersize=13
legendfs = 12


x_arr = np.array(data[:, 3])
y_arr = np.array(data[:, 4])
fig, ax = plt.subplots(constrained_layout=True)
ax.plot(x_arr, y_arr, label="Trajectoire satellite")
ax.set_xlabel('x [m]', fontsize=fs)
ax.set_ylabel('y [m]', fontsize=fs)

xl = 3.80321e+08
Rl = 1737.1e+03

plt.figure()
theta = np.linspace(0, 1) * 2 * np.pi
ax.plot(xl + Rl * np.cos(theta), Rl * np.sin(theta), 'y-', linewidth=1, label="Surface lunaire")

is_in_moon = ((x_arr - xl) ** 2 + (y_arr ** 2)) < Rl ** 2;
hit_moon = is_in_moon ^ np.roll(is_in_moon, -1)
hit_idx = np.argmax(hit_moon)
hitx = x_arr[hit_idx]
hity = y_arr[hit_idx]
#ax.plot(x_arr[hit_idx], y_arr[hit_idx], 'r+', label=f"Point de crash: (x,y)=({hitx:.3E},{hity:.3E})", markersize=markersize)

plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)
plt.legend(fontsize=legendfs)
#plt.savefig(str(sys.argv[2]))


#print(len(data[:,5]))
t_lst = dt[-1] * np.arange(0, nsteps[-1] + 1)
plt.figure()
plt.plot(t_lst, data[:, 5], 'y-', linewidth=lw, label="Mecanique")
plt.plot(t_lst, data[:, 6], 'r-', linewidth=lw, label="Cinetique")
plt.plot(t_lst, data[:, 7], 'b-', linewidth=lw, label="Centrifuge")
plt.plot(t_lst, data[:, 8], 'k-', linewidth=lw, label="Grav")
plt.xlabel("Time t [s]", fontsize=fs)
plt.ylabel("Energie [J]", fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.legend()

plt.figure()
plt.plot(t_lst, np.array(data[:, 6]) + np.array(data[:, 8]), label="Cin + grav")
plt.plot(t_lst, data[:, 7], label="Centrifuge")
plt.xlabel("t")
plt.ylabel("En")
plt.legend()
plt.show()
# uncomment the following if you want debug
#import pdb
#pbd.set_trace()
#plt.figure()
#plt.loglog(dt, error, r'r+-', linewidth=lw)
#plt.xlabel(r'$\Delta t \mathrm{[s]}$', fontsize=fs)
#plt.ylabel('final position error [m]', fontsize=fs)
#plt.xticks(fontsize=fs)
#plt.yticks(fontsize=fs)
#plt.grid(True)

"""
Si on n'a pas la solution analytique: on représente la quantite voulue
(ci-dessous v_y, modifier selon vos besoins)
en fonction de (Delta t)^norder, ou norder est un entier.
"""

"""
norder = int(sys.argv[1])  # Modify if needed
skip = int(sys.argv[2])
alpha = float(sys.argv[3])

plt.figure()
plt.plot((dt**norder)[skip:], convergence_list_x[skip:], 'k+-', linewidth=lw, label=r"$\alpha = " + f" {alpha}$")
plt.xlabel(r"$(\Delta t \mathrm { [s] } )" + f" ^{norder}$", fontsize=fs)
plt.ylabel(r'$x\'(t_{fin})$', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)
plt.legend(fontsize=legendfs)
plt.show()

plt.figure()
plt.plot((dt**norder)[skip:], convergence_list_y[skip:], 'k+-', linewidth=lw, label=r"$\alpha = " + f" {alpha}$")
plt.xlabel(r"$(\Delta t \mathrm { [s] } )" + f" ^{norder}$", fontsize=fs)
plt.ylabel(r'$y\'(t_{fin})$', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)
plt.legend(fontsize=legendfs)
plt.show()
"""
