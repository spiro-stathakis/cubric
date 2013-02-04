#! /usr/bin/X11/python

import subprocess
import sys
output = {}
result =0 
cmd = subprocess.Popen(["/cubric/software/bin/cubricc" , sys.argv[1]],stdout=subprocess.PIPE)
for line in cmd.stdout:
    parts = line.split(':')
    output[parts[0].strip()]  = int(parts[1].strip()) 

if 'freeram' in output.keys(): 
	result =  (output['freeram'] / output['cpus']) 

sys.stdout.write(str(result)); 
