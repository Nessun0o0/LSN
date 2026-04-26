import os
import sys
import numpy as np

def edit_input(temp, H, restart, sim_type, nsteps):
    input_data = None
    with open("../INPUT/input.dat", "r", encoding="utf-8") as infile:
        input_data = infile.readlines()

    for i, line in enumerate(input_data):
        data = line.split(" ")
        if (data[0] == "TEMP"):
            input_data[i] = f"TEMP                   {temp}\n"
        elif (data[0] == "SIMULATION_TYPE"):
            data = [dato for dato in data if dato != ""]
            input_data[i] = f"SIMULATION_TYPE        {sim_type}    {data[2]}    {H}\n"
        elif (data[0] == "RESTART"):
            input_data[i] = f"RESTART                {restart}\n"
        elif data[0] == "NSTEPS":
            input_data[i] = f"NSTEPS                 {nsteps}\n"
        
    with open("../INPUT/input.dat", "w", encoding="utf-8") as infile:
        infile.writelines(input_data)

temps = np.arange(0.5,2.1,0.1)[::-1]

simulation_types = []
simulation_type_name = []
output_files = ["total_energy", "specific_heat", "susceptibility"]
nsteps = 20000
H_off = False
H_on = False

for arg in sys.argv[1:]:
    if arg == "METROPOLIS" or arg == "all":
        simulation_types.append(2)
        simulation_type_name.append("METROPOLIS")
    if arg == "GIBBS" or arg == "all":
        simulation_types.append(2)
        simulation_type_name.append("GIBBS")
    if arg=="H_off" or arg == "all":
        H_off = True
    if arg=="H_on" or arg == "all":
        H_on = True
    


for i, simulation_type in enumerate(simulation_types):
    sym_name = simulation_type_name[i]

    if H_off:
        H = 0.0
        restart = 0
        for temp in temps:
            if (temp <= 1.9):
                os.replace("../OUTPUT/CONFIG/config.spin", "../INPUT/CONFIG/config.spin")
                restart = 1

            edit_input(temp, H, restart, simulation_type, int(nsteps*(3-temp)))
            os.system("./simulator.exe")

            for outfile in output_files:
                os.replace(f"../OUTPUT/{outfile}.dat", f"../OUTPUT-{sym_name}/{outfile}_{temp:.1f}.dat")
            
            print(f"Completed simulation with {sym_name} algorithm\tT={temp:.1f}\tH={H}")

    if H_on:
        H = 0.02
        restart = 0
        for temp in temps:
            if (temp <= 1.9):
                os.replace("../OUTPUT/CONFIG/config.spin", "../INPUT/CONFIG/config.spin")
                restart = 1

            edit_input(temp, H, restart, simulation_type, int(nsteps*(3-temp)))
            os.system("./simulator.exe")

            os.replace(f"../OUTPUT/magnetization.dat", f"../OUTPUT-{sym_name}/magnetization_{temp:.1f}.dat")
            
            print(f"Completed simulation with {sym_name} algorithm\tT={temp:.1f}\tH={H}")