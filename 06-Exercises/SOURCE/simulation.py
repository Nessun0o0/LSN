import os
import numpy as np

def edit_input(temp, H, restart, sim_type):
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
        
    with open("../INPUT/input.dat", "w", encoding="utf-8") as infile:
        infile.writelines(input_data)

temps = np.arange(0.5,2.1,0.1)[::-1]
output_files = ["total_energy", "specific_heat", "susceptibility"]
simulation_types = [2, 3]
simulation_type_name = ["Metropolis", "Gibbs"]

H = 0.0
restart = 0

for i, simulation_type in enumerate(simulation_types):
    for temp in temps:
        if (temp <= 1.9):
            os.replace("../OUTPUT/CONFIG/config.spin", "../INPUT/CONFIG/config.spin")
            restart = 1

        edit_input(temp, H, restart, simulation_type)
        os.system("./simulator.exe")

        for outfile in output_files:
            os.replace(f"../OUTPUT/{outfile}.dat", f"../OUTPUT_06.1/{outfile}_{temp:.1f}.dat")
        
        print(f"Completed simulation with {simulation_type_name[i]} algorithm\tT={temp:.1f}\tH={H}")

    H = 0.02
    restart = 0
    
    for temp in temps:
        if (temp <= 1.9):
            os.replace("../OUTPUT/CONFIG/config.spin", "../INPUT/CONFIG/config.spin")
            restart = 1

        edit_input(temp, H, restart, simulation_type)
        os.system("./simulator.exe")

        os.replace(f"../OUTPUT/magnetization.dat", f"../OUTPUT_06.1/magnetization_{temp:.1f}.dat")
        
        print(f"Completed simulation with {simulation_type_name[i]} algorithm\tT={temp:.1f}\tH={H}")