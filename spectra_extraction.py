"""
Read and process data
"""

import sys
import struct
import logging

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

from scipy import stats
from typing import Any, List, Dict, Optional
from timeit import default_timer as timer

"""
Data format:

[p0, p1, p2....T, P, Time]
"""
DATA_LENGTH = 445 # length of the data, could be inferred
TEST_ENV_VALS = 3 # Number of test measurement values to use for offset
TOTAL_LEN = DATA_LENGTH + TEST_ENV_VALS # total length for calculations
DELIMETER = 9999 # data delimeter
LOGGING_THREASHOLD = 0.02 # how precise the logging interval is
LOGGING_DURATION = 30 # duration between logs

logging.basicConfig(level=logging.INFO) # display info logs by default

def read_file_manager(file_path: str, data_type: str):
	# open file with context manager so it auto closes when were done
	with open(file_path, "rb") as data_file:
		while True:
			try:
				data = struct.unpack(data_type, data_file.read(4))[0]
			except struct.error:
				logging.info("Reached EOF")
				break #exit loop
			if data != 0:
				yield data
			else:
				logging.info("Zero found")

"""
begin execution
"""

try:
        file_path = sys.argv[1]
        data_type = sys.argv[2]
except IndexError:
        print("File path/name & data type are required required arguments")
        exit(-1)

data_array = []
data_consumend_count = 0
scan = np.zeros((1,TOTAL_LEN))
start = timer() # begin timing
for data_value in read_file_manager(file_path, data_type):
        data_consumend_count += 1
        data_array.append(data_value)
        # process the data in TOTAL LEN sized chunks (448)
        if data_value == DELIMETER and len(data_array) == TOTAL_LEN:
                scan=np.vstack([scan, np.flip(data_array)])
                data_array = [] # reset data array for next chunk
                end = timer()
                if ((end - start) % LOGGING_DURATION) < LOGGING_THREASHOLD:
                        logging.info(f"Time elapsed: {int(end-start)}, data consumed: {data_consumend_count}")

end = timer() # end timing
logging.info(f"Time elapsed: {end - start}")
# below is unchanged
step =[]
for i in range(442):
    dummy = 0.00052+i*5.00*10**(-7)
    step = np.append(step,dummy)   
for i in range(442):
    step[i]=step[i]+step[i-1]
    
temperatures=scan[:,2]*0.01
pressures=scan[:,1]*0.01
times=scan[:,TOTAL_LEN-1]
for i in range(len(times)):
        if times[i]==9999:
                times[i]=scan[i,TOTAL_LEN-2]

scans=scan[:,3:445]
scans = np.delete(scans, 0, 0)

signal=[]
for i in range(len(temperatures)):

        idv = scan[i,:]
        subset1 = idv[20:50]
        combined=subset1
        subset2 = idv[380:400]
        combined=np.append(subset1,subset2)

        subset1 = step[20:50]
        subset2 = step[380:400]
        combinedpoints=np.append(subset1,subset2)

        slope, intercept, r_value, p_value, std_err = stats.linregress(combinedpoints,combined)
        y=step*slope+intercept

        
        A = np.log(y[25:410]/idv[25:410])
        dummy = np.sum(A)
        signal=np.append(signal,dummy)

butthole = [times,temperatures,pressures,signal]
butthole = np.transpose(butthole)
buttholefinal = pd.DataFrame(data=butthole)
buttholefinal.to_csv('/Users/emilywein/Desktop/bk2/until2fmd',index=False)

scansfinal = pd.DataFrame(data=scans)
scansfinal.to_csv('/Users/emilywein/Desktop/bk2/until2fscans',index=False)

end = timer()
logging.info(f"Total time: {end-start}")










