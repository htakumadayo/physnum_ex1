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


#nsteps = np.geomspace(1.1e4, 10e4, 50).astype(np.int32)
# print(nsteps[-1])
nsteps = np.array([1000000]) # TODO change
nsimul = len(nsteps)  # Number of simulations to perform

tfin = 3592000  # TODO: Verify that the value of tfin is EXACTLY the same as in the input file

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
    convergence_list.append(yy)
    # TODO compute the error for each simulation
    error[i] =  abs(3.507e8 - xx); 

lw = 1.5
fs = 16

fig, ax = plt.subplots(constrained_layout=True)
ax.plot(data[:, 3], data[:, 4])
ax.set_xlabel('x [m]', fontsize=fs)
ax.set_ylabel('y [m]', fontsize=fs)

xl = 3.80321e+08
Rl = 1737.1e+03

ax.plot(xl, 0, 'yo')
theta = np.linspace(0, 1) * 2 * np.pi
ax.plot(xl + Rl * np.cos(theta), Rl * np.sin(theta), 'y+-', linewidth=1)

#print(len(data[:,5]))
#plt.figure()
#plt.plot(dt * np.arange(0, nsteps[-1] + 1), data[:, 5], 'y-', linewidth=lw)
#plt.xlabel("Time t [s]", fontsize=fs)
#plt.ylabel("$E_m [J]", fontsize=fs)
#plt.xticks(fontsize=fs)
#plt.yticks(fontsize=fs)

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
norder = 2  # Modify if needed

plt.figure()
plt.plot((dt**norder)[35:], convergence_list[35:], 'k+-', linewidth=lw)
plt.xlabel(r"$(\Delta t \mathrm {[s]})" + f" ^{norder}$", fontsize=fs)
plt.ylabel(r'$y\'(t_{fin})$', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)

plt.show()
