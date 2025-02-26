import numpy as np
import subprocess
import matplotlib.pyplot as plt
import pdb
import os

# Parameters
# TODO adapt to what you need (folder path executable input filename)
executable = 'physnum_ex1'  # Name of the executable (NB: .exe extension is required on Windows)
repertoire = r"./"
os.chdir(repertoire)

input_filename = 'configuration.in.example'  # Name of the input file


nsteps = np.geomspace(9000, 10e4, 50).astype(np.int32);
# nsteps = np.array([4000, 6000, 11000, 13e3, 51e3, 60e3, 15e4]) # TODO change
nsimul = len(nsteps)  # Number of simulations to perform

tfin = 259200  # TODO: Verify that the value of tfin is EXACTLY the same as in the input file

dt = tfin / nsteps


paramstr = 'nsteps'  # Parameter name to scan
param = nsteps  # Parameter values to scan

# Simulations
outputs = []  # List to store output file names
convergence_list = []
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
    convergence_list.append(xx)
    # TODO compute the error for each simulation
    error[i] =  0 

lw = 1.5
fs = 16

fig, ax = plt.subplots(constrained_layout=True)
ax.plot(data[:, 3], data[:, 4])
ax.set_xlabel('x [m]', fontsize=fs)
ax.set_ylabel('y [m]', fontsize=fs)



# uncomment the following if you want debug
#import pdb
#pbd.set_trace()
plt.figure()
plt.loglog(dt, error, r'r+-', linewidth=lw)
plt.xlabel(r'$\Delta t \mathrm{[s]}$', fontsize=fs)
plt.ylabel('final position error [m]', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)

"""
Si on n'a pas la solution analytique: on représente la quantite voulue
(ci-dessous v_y, modifier selon vos besoins)
en fonction de (Delta t)^norder, ou norder est un entier.
"""
norder = 3  # Modify if needed

plt.figure()
plt.plot(dt**norder, convergence_list, 'k+-', linewidth=lw)
plt.xlabel(r"$(\Delta t \mathrm {[s]})" + f" ^{norder}$", fontsize=fs)
plt.ylabel(r'$xx$', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)

plt.show()
