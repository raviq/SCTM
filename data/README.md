# Data Directory — SCTM Traffic Simulation

This directory contains all input data and simulation output for the **Stochastic Cell Transmission Model (SCTM)** — a road traffic simulator for urban networks.

Reference paper:
> Hadfi, R., Tokuda, S., & Ito, T. (2017). *Traffic Simulation in Urban Networks Using Stochastic Cell Transmission Model.* Computational Intelligence, 33(4), 826–842.

---

## Directory Structure

```
data/
├── network.csv              # Road network topology (input)
├── vehicle.csv              # Vehicle demand / OD matrix (input)
├── signal.csv               # Traffic signal configuration (input)
├── result/                  # All simulation outputs
│   ├── density/             # Per-cell traffic density over time
│   ├── flow/                # Per-cell inflow and outflow
│   ├── mode/                # Stochastic traffic mode probabilities
│   ├── traffic_volume/      # Aggregated 10-minute traffic counts
│   ├── time/                # Per-cell travel time statistics
│   ├── outflow/             # Vehicles exiting each cell per step
│   ├── surplus/             # Blocked vehicles (spillback)
│   ├── signal/              # Signal phase state per intersection
│   ├── coefficient/         # Coefficient of variation of density
│   ├── sctm_result_vehicle_number.csv   # Vehicles per cell, all time steps
│   ├── sctm_result_freespace.csv        # Free capacity per cell, all time steps
│   ├── sctm_result_adjacent_flow.csv    # Flow between adjacent subsystems
│   ├── sctm_result_move_time.csv        # Individual vehicle trip times
│   ├── sctm_result_each_move.csv        # Cells each vehicle passed through
│   └── sctm_result_route.csv            # Node-level route for each vehicle
└── graph/                   # R scripts and generated PNG visualizations
    ├── density/             # Density mean±SD plots per cell
    ├── mode/                # Mode probability plots per cell
    ├── traffic_volume/      # 10-minute traffic volume plots
    ├── coefficient/         # Coefficient of variation plots
    ├── observer_location.csv            # Sensor-to-cell mapping
    ├── result_trip.csv                  # Observed OD trip data
    ├── travel_time.png                  # Travel time distribution plot
    ├── mean_variance_density.r          # R script: density plots
    ├── mode_probability.r               # R script: mode probability plots
    ├── traffic_volume.r                 # R script: volume plots
    ├── travel_time.r                    # R script: travel time plot
    ├── coefficient.r                    # R script: CV plots
    ├── real_density.r                   # R script: observed vs. simulated density
    └── make_graph.sh                    # Shell script: runs all R scripts
```

---

## Input Files

### `network.csv`

Defines the directed road network graph. Each row is one directed link (road segment). The file has no header.

**Format:** `from_node, to_node, length_m, capacity_veh, is_special`

| Field | Type | Description |
|---|---|---|
| `from_node` | int | ID of the upstream node |
| `to_node` | int | ID of the downstream node |
| `length_m` | int | Link length in metres |
| `capacity_veh` | int | Maximum number of vehicles the link can hold |
| `is_special` | int | `0` = standard road link; `1` = special link (pedestrian crossing, access road, or connector) |

**Network statistics:**
- 57 nodes (IDs 0–56), where nodes 55 and 56 are virtual/dummy connector nodes
- ~130 undirected road pairs → ~274 directed cell segments
- Link lengths range from 45 m to 435 m
- Capacity ranges from 10 to 96 vehicles
- 18 special links (flag = 1) handle pedestrian crossings and sub-network connectors

**Example rows:**
```
0,4,180,40,0     # Node 0 → Node 4, 180 m, 40-vehicle capacity, standard
4,0,180,40,0     # Node 4 → Node 0, same link in reverse
10,55,105,24,1   # Node 10 → Node 55 (virtual), 105 m, special link
```

---

### `vehicle.csv`

Specifies vehicle demand as an Origin-Destination (OD) matrix sampled every 10 minutes over a 130-minute period. The file has no header.

**Format:** `origin_node, destination_node, vehicle_count, time_offset_min`

| Field | Type | Description |
|---|---|---|
| `origin_node` | int | Node ID where vehicles depart |
| `destination_node` | int | Node ID where vehicles are destined |
| `vehicle_count` | int | Number of vehicles for this OD pair in this time window |
| `time_offset_min` | int | Start of the 10-minute loading window (0, 10, 20, …, 120) |

**Demand characteristics:**
- 13 time windows: t = 0, 10, 20, …, 120 minutes
- Multiple OD pairs active per window
- Major attractors (destinations): nodes 0, 2, 3, 6, 8
- Highest-demand origins: nodes 8 (up to 71 vehicles/window), 48, 56, 3
- Total simulation demand: several thousand vehicles over the full period

**Example rows:**
```
3,0,2,0      # 2 vehicles from node 3 to node 0, loaded at t=0
8,0,14,0     # 14 vehicles from node 8 to node 0, loaded at t=0
8,0,12,10    # 12 vehicles from node 8 to node 0, loaded at t=10
```

---

### `signal.csv`

Defines traffic signal timing at signalised intersections. The file has no header.

**Format:** `node, approach_id, green_start, yellow_start, red_start, cycle_length`

| Field | Type | Description |
|---|---|---|
| `node` | int | Node ID of the signalised intersection |
| `approach_id` | int | Approach direction index (or -1 for no signal on this entry) |
| `green_start` | int | Time step within cycle when green begins (-1 if unused) |
| `yellow_start` | int | Time step within cycle when yellow begins |
| `red_start` | int | Time step within cycle when red begins |
| `cycle_length` | int | Total signal cycle length in seconds |

**Signal configuration:**
- 17 signalised nodes, all with a 120-second cycle
- Green, yellow, and red phases defined per approach direction
- Phase constants: BLUE/GREEN = 1, YELLOW = 2, RED = 3 (see `src/setting.h`)
- Some entries use `-1` for approach_id or phase times, indicating unsignalised or unused approaches

**Example rows:**
```
4,3,50,112,115,120   # Node 4, approach 3: green at t=50, yellow at 112, red at 115, cycle 120s
4,0,0,42,45,120      # Node 4, approach 0: green at t=0, yellow at 42, red at 45
30,-1,-1,-1,-1,-1    # Node 30, no signal on this entry
```

---

## Simulation Output Files

The simulation runs for 1000 time steps (1 step = 1 second). Most outputs are indexed by OD pair or cell number, producing **274 files per output type** — one per directed cell (road segment).

All output CSVs include a `Time` column as their first field.

---

### `result/density/sctm_result_density_N.csv`

Traffic density statistics for cell N over time. This is the primary SCTM output.

**Columns:** `Time, Mean, Mean+SD, Mean-SD, SD, Real`

| Column | Description |
|---|---|
| `Time` | Simulation time step |
| `Mean` | Mean density (vehicles/m) averaged over 1000 stochastic noise samples |
| `Mean+SD` | Mean plus one standard deviation |
| `Mean-SD` | Mean minus one standard deviation |
| `SD` | Standard deviation of density across noise samples |
| `Real` | Deterministic (noise-free) density for reference |

The spread between `Mean+SD` and `Mean-SD` reflects demand and supply uncertainty introduced by SCTM's stochastic noise model (controlled by `RATIO = 0.05` in `setting.h`). 1001 rows (header + 1000 time steps).

---

### `result/flow/sctm_result_inflow_outflow_N.csv`

Vehicle flow at the boundaries of cell N.

**Columns:** `Time, inflow, outflow`

| Column | Description |
|---|---|
| `inflow` | Vehicles entering cell N at this time step |
| `outflow` | Vehicles leaving cell N at this time step |

Outflow is zero for the first few steps until vehicles reach the cell exit.

---

### `result/mode/sctm_result_mode_N.csv`

Probability distribution of traffic flow modes in cell N at each time step. This captures the stochastic regime of the cell — a core SCTM innovation over deterministic CTM.

**Columns:** `Time, FF, CC, CF, FC1, FC2`

| Column | Mode | Description |
|---|---|---|
| `FF` | Free-Free | Cell is free-flowing; downstream cell is also free-flowing |
| `CC` | Congested-Congested | Cell is congested; downstream is also congested |
| `CF` | Congested-Free | Cell is congested; downstream has free capacity |
| `FC1` | Free-Congested (type 1) | Cell is free; downstream is partially congested |
| `FC2` | Free-Congested (type 2) | Cell is free; downstream is fully congested |

All five probabilities sum to 1.0 at each time step.

---

### `result/traffic_volume/sctm_result_traffic_volume_N.csv`

Aggregated 10-minute traffic volume for cell N, used to compare simulated output against real-world traffic counts.

**Columns:** `Time, Flow`

- `Time` is reported at the end of each 10-minute window (e.g., t = 600 for the first window)
- `Flow` is the total number of vehicles observed passing through the cell in that window

---

### `result/time/sctm_result_necessary_time_N.csv`

Stochastic travel time (in seconds) for a vehicle to traverse cell N.

**Columns:** `Time, Mean, Mean+SD, Mean-SD, SD`

Mirrors the density statistics format. Higher values indicate congestion-induced delay. SD grows as density increases toward jam density.

---

### `result/outflow/sctm_result_move_number_N.csv`

Number of vehicles that successfully exited cell N per time step.

**Columns:** `Time, Move_Number`

Complements the `flow` files. Zero during the early simulation period before vehicles reach a given cell.

---

### `result/surplus/sctm_result_surplus_outflow_N.csv`

Number of vehicles in cell N that attempted to exit but were blocked by downstream congestion (i.e., spillback).

**Columns:** `Time, Surplus`

Non-zero values indicate that cell N is acting as a queue origin. Large sustained surplus values identify chronic bottlenecks in the network.

---

### `result/signal/sctm_result_signal_K.csv`

Traffic signal state at signalised intersection K for each time step. Only 17 files are present, one per signalised node.

**Columns:** `Time, North, East, South, West`

Each directional column holds the current signal phase: `red`, `blue` (green), or `yellow`.

---

### `result/coefficient/sctm_result_coefficient_N.csv`

Coefficient of Variation (CV) of density for cell N over time.

**Columns:** `"", Time, CV`

CV = SD / Mean. Measures relative stochastic variability. Values near zero indicate stable free-flow or congestion; higher values indicate transitional or highly uncertain states.

---

### `result/sctm_result_vehicle_number.csv`

Wide-format matrix of the total number of vehicles in each cell at every time step.

**Columns:** `Time, Cell 0, Cell 1, …, Cell 273`

One row per time step; one column per cell (274 columns). Allows reconstruction of the spatial vehicle distribution across the entire network over time.

---

### `result/sctm_result_freespace.csv`

Wide-format matrix of remaining free capacity in each cell at each time step.

**Columns:** `Time, Cell 0, Cell 1, …, Cell 273`

`freespace = capacity - vehicle_number`. Values approaching zero indicate near-saturated or fully saturated links. Structure is identical to `sctm_result_vehicle_number.csv`.

---

### `result/sctm_result_adjacent_flow.csv`

Vehicle flow transferred between adjacent subsystems (cell pairs) at each time step.

**Columns:** `Time, Subsystem 0, Subsystem 1, …, Subsystem 136`

137 subsystem columns — one per directed cell boundary. This is the raw transfer quantity used in the CTM update equation at each step.

---

### `result/sctm_result_move_time.csv`

Individual vehicle trip record. One row per vehicle that reached its destination.

**Columns:** `Vehicle, Origin, Destination, Time`

| Column | Description |
|---|---|
| `Vehicle` | Vehicle ID |
| `Origin` | Origin node ID |
| `Destination` | Destination node ID |
| `Time` | Total travel time in minutes from departure to arrival |

Vehicles that did not complete their journey within the simulation window are not recorded.

---

### `result/sctm_result_each_move.csv`

Records which cell each vehicle was in at each time step it moved.

**Columns:** `Vehicle, Pass 0, Pass 1, Pass 2, …`

Each `Pass N` column holds the cell ID that vehicle visited on its Nth move. The number of passes varies by route length.

---

### `result/sctm_result_route.csv`

Node-level route taken by each vehicle through the network.

**Columns:** `Vehicle, Route 1, Route 2, …, Route N`

Each `Route N` column holds a node ID. Reading across the columns gives the sequence of nodes the vehicle traversed from origin to destination.

---

## Graph Directory (`graph/`)

Pre-generated visualisations and the R scripts used to produce them.

### Input data for graphs

| File | Description |
|---|---|
| `observer_location.csv` | Maps virtual observer IDs to cell IDs (`observe, link`). Used to align simulation output with real-world sensor positions. |
| `result_trip.csv` | Sample observed OD trip data (`Origin, Destination, Time`). Used for model validation. |

### PNG outputs

| Subdirectory / File | Count | Content |
|---|---|---|
| `density/density_mean_variance_N.png` | 274 | Density mean ± SD over time for cell N |
| `mode/mode_probability_N.png` | 137 | Traffic mode probability evolution for cell N |
| `traffic_volume/` | varies | 10-minute traffic volume bar charts |
| `coefficient/sctm_result_coefficient_0.png` | 1 | Coefficient of variation over time |
| `travel_time.png` | 1 | Overall travel time distribution across all vehicles |

### R scripts

| Script | Purpose |
|---|---|
| `mean_variance_density.r` | Plots density mean and ±SD envelope for each cell |
| `mode_probability.r` | Plots stacked mode probability (FF/CC/CF/FC1/FC2) over time |
| `traffic_volume.r` | Plots 10-minute traffic counts, comparing simulated vs. observed |
| `travel_time.r` | Plots travel time histogram and CDF |
| `coefficient.r` | Plots coefficient of variation of density |
| `real_density.r` | Overlays real (observed) density against simulated mean density |
| `make_graph.sh` | Convenience script that runs all the above R scripts in sequence |

To regenerate all graphs:
```bash
cd data/graph/
sudo ./make_graph.sh
```

---

## Notes

- All CSV files use comma (`,`) as delimiter and `\r\n` (Windows-style) line endings.
- Indexed output files (e.g. `sctm_result_density_N.csv`) use zero-based cell indices matching the order cells are constructed from `network.csv`.
- The simulation is controlled by constants in `src/setting.h`: time step (`STEP = 1` s), noise samples (`NOISE_NUM = 1000`), uncertainty ratio (`RATIO = 0.05`), and total duration (`MAX_T = 1000` steps).
- Setting `TEST = true` in `setting.h` redirects all outputs to `test_data/` instead of `data/`.
