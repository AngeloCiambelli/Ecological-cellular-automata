import scipy.stats 
import numpy as np


print(scipy.stats.norm.ppf( [1-0.5928], 0,1)*1/np.sqrt(10)-0.5)
 
