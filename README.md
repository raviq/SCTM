<img src="https://raw.githubusercontent.com/raviq/SCTM/master/docs/banner.svg" alt="SCTM" width="100%"/>

<p align="center">
  <strong>Stochastic Cell Transmission Model for Urban Traffic Simulation</strong><br/>
  A C++ implementation of macroscopic traffic flow modeling with stochastic dynamics
</p>

<p align="center">
  <a href="https://www.gnu.org/licenses/gpl-3.0"><img src="https://img.shields.io/badge/License-GPL%20v3-blue.svg" alt="License: GPL v3"/></a>
  <img src="https://img.shields.io/badge/language-C%2B%2B-orange.svg" alt="C++"/>
  <img src="https://img.shields.io/badge/analysis-R-276DC3.svg" alt="R"/>
</p>

---

## Overview

This project implements the **Modified Stochastic Cell Transmission Model (M-SCTM)**, a macroscopic traffic simulation model that extends the conventional SCTM to handle complex urban road networks.

The classical SCTM captures traffic state uncertainty and changing demand/supply conditions with high accuracy, but was originally limited to freeways and simple networks with a single origin-destination pair. M-SCTM overcomes this limitation by introducing **vehicle agents** with **route choice behavior**, enabling simulation on realistic urban networks where turning ratios are not predefined but determined through a route search algorithm.

### Key Features

- 🔀 **Urban network support** — extends SCTM beyond freeways to complex multi-OD urban networks.
- 🎲 **Stochastic dynamics** — models uncertainty in traffic states, travel demand, and supply conditions.
- 🚗 **Agent-based route choice** — vehicles select routes dynamically rather than relying on fixed turning ratios.
- 📊 **Statistical analysis** — includes R scripts for post-simulation data analysis and validation.
- ⚡ **Efficient C++ core** — built on Eigen for fast linear algebra and Boost for utilities.

---

## Getting Started

### Prerequisites

| Dependency | Purpose |
|---|---|
| **g++** | C++ compiler (C++11 or later) |
| [Boost](https://www.boost.org/) | General-purpose C++ libraries |
| [Eigen](http://eigen.tuxfamily.org) | Linear algebra (matrix operations for the CTM) |
| [R](https://www.r-project.org/) | Post-simulation data analysis |

### Build

```bash
make
```

### Run

```bash
bin/release/simulator
```

Simulation output is written to `data/` and `test_data/`.

---

## Project Structure

```
SCTM/
├── src/                # C++ source code (M-SCTM implementation)
├── bin/release/        # Compiled simulator binary
├── build/release/src/  # Build artifacts
├── data/               # Simulation output data
├── test_data/          # Test output data
├── Makefile            # Build configuration
└── simulator           # Entry point script
```

---

## Background

The **Cell Transmission Model (CTM)** discretizes road segments into cells and simulates how traffic density propagates through them over time steps, based on fundamental traffic flow relationships. The **Stochastic CTM** adds probabilistic elements to capture real-world uncertainty in driver behavior and network conditions.

M-SCTM further extends this framework by resolving a cell-duplication problem that arises when applying the original SCTM's subsystem assignment to adjacent intersections in urban networks. It introduces individual vehicle agents that make route decisions, making the model applicable to networks with arbitrary topology and multiple origin-destination pairs.

---

## Citation

If you use this software in your research, please cite:

```bibtex
@article{hadfi2017sctm,
  title     = {Traffic Simulation in Urban Networks Using Stochastic Cell Transmission Model},
  author    = {Hadfi, Rafik and Tokuda, Sho and Ito, Takayuki},
  journal   = {Computational Intelligence},
  volume    = {33},
  number    = {4},
  pages     = {826--842},
  year      = {2017},
  publisher = {Wiley}
}
```

---

## Related Projects

- [**Simultra**](https://github.com/raviq/Simultra) — A multiagent hybrid (microscopic + mesoscopic) road traffic simulator with OpenStreetMap rendering, by the same author.

---

## Contributing

Comments, feedback, and ideas are welcome. If you find bugs or omissions in the code or documentation, please don't hesitate to [open an issue](https://github.com/raviq/SCTM/issues) or submit a pull request.

---

## License

Distributed under the **GNU General Public License v3.0**. See [`LICENSE`](LICENSE) for details.

© 2016 [Rafik Hadfi](mailto:rafik.hadfi@gmail.com)
