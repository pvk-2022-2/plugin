import os
import sys
import shutil

test_folder = "tests/"
build_folder = f"{test_folder}Build/"
if not os.path.exists(build_folder):
    os.mkdir(build_folder)

import patchbuild
ops = ['0', 'z', '2']

for file in os.listdir(test_folder):
    pathsplit = os.path.splitext(file)
    if(pathsplit[1] != '.c'):
        continue
    
    for o in ops:
        output = f"{build_folder}TEST_{pathsplit[0]}_O{o}.repatch"
        if patchbuild.build_repatch(test_folder + file, output, False, o):
            shutil.copy(output, f'../Resources/Public/')
