import os
import random
import sys
import time 
import unittest

import numpy as np
from scipy import signal

sys.path.append(os.getcwd())

import fir_filters

class TestDTFT(unittest.TestCase):
    def test_cheating(self):
        print("\nTesting to make sure no firls()")

        with open("fir_filters.py", "r") as file:
            content = file.read()

        firls_check = False
        # check if string present or not
        if "firls" in content:
            eigen_check = True

        self.assertFalse(firls_check,
                         "You are illegally using "
                         "the built-in firls function")
        
    def test_phase_correct(self):
        print("\nTesting fir_least_squares() function")

        M = random.randint(50,100)
        if M % 2:
            M += 1

        bands_temp = sorted(np.random.uniform(0,np.pi,size=4))

        bands = [[0,1],[1.5,np.pi]]

        amp_my = [0,1]
        amp_sol = [0,0,1,1]
            
        h_my = fir_filters.fir_least_squares(M+1,bands,amp_my)
        h_sol = signal.firls(M+1, bands, amp_sol, nyq=np.pi)

        np.testing.assert_array_almost_equal(h_my, h_sol,decimal=3)

        amp_my = [1,0]
        amp_sol = [1,1,0,0]
            
        h_my = fir_filters.fir_least_squares(M+1,bands,amp_my)
        h_sol = signal.firls(M+1, bands, amp_sol, nyq=np.pi)

        np.testing.assert_array_almost_equal(h_my, h_sol,decimal=3)

        
if __name__ == "__main__":
    unittest.main()


