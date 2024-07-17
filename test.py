import numpy as np
import matplotlib.pyplot as plt

# Percolation threshold and critical exponent for 2D square lattice
p_c = 0.592746
beta = 5 / 36

def percolation_probability(p):
    """Calculate the percolation probability P(p) for a given occupation probability p."""
    if p < p_c:
        return 0
    else:
        return (p - p_c) ** beta

# Generate a range of p values
p_values = np.linspace(0.4, 0.8, 100)
P_values = np.array([percolation_probability(p) for p in p_values])

# Plotting the percolation probability
plt.figure(figsize=(10, 6))
plt.plot(p_values, P_values, linestyle='-', color='b', label='Percolation Probability $P(p)$')
plt.axvline(p_c, color='r', linestyle='--', label='$p_c$')
plt.xlabel('Occupation Probability $p$')
plt.ylabel('Percolation Probability $P(p)$')
plt.title('Percolation Probability vs Occupation Probability')
plt.legend()
plt.grid(True)
plt.show()




