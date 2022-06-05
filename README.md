
# Influence

This repository contains a basic C++ implementation of the core concept of the mobile game [Influence](https://influence.teremokgames.com/).
Only the core mechanism is implemented, and only random agents are available.


## Installation

The code needs the following libraries to compile:
- C++17 compiler
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL2_gfx](https://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/)
- [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf)

To compile, simply clone the directory and run `make`.


## Usage

To run the game, simply run:
`./main path/to/file.map`

A GUI map editor is also available; to edit a `.map` file, run:
`./main -e path/to/file.map`
The map editor is controlled through the keyboard. Keybindings are as follows:
- `w`, `h` and `n`, followed by a number and confirmed by pressing Enter to resp. modify the width, the height and the number of teams on the map
- `hjkl` to move from cell to cell (vim style)
- `t/T` to modify the team of the current cell (forward/backward)
- `u/U` to modify the value of the current cell (more/less)
- `123456` to toggle existing links around the current cell
- `s/S` to save/save as... the current map


## Map format

The map files are binary files that contain the information of every cell in a grid.
These files can be generated through the map editor presented above, or by writing a CSV file that represents the map and then converting it using the `generate_map` binary.

A valid CSV file should have the following format:
**Header**: the first line must be `w,h,n` with `w`, `h` and `n` the width, height and number of teams on the map respectively
**h following lines**: each line must be composed of `w` cell encoding separated by commas, e.g. `C1,C2,C3,C4,C5` for a width of 5
**Note:** As the grid is hexagonal, the last cell of one line out of two is ignored.

A cell is encoded in the following format: `{value}[:{team}][/{missing_links}][+]` where:
- `value` is the value of the cell
- `team` represents the current team of the cell: from 1 to `n` for teams, and 0 for a neutral cell. If omitted, the team defaults to 0 (neutral cell).
- `missing_links` is a list of integers between 1 and 6 that correponds to the links from the current cell to the neighbour cells that must be removed. The link 1 is the link to the up-right cell, and the others follow in clockwise manner (2: cell to the right, 3: bottom-right etc.). If omitted, the cell has all possible links to its neighbours.
- `+` indicates if the cell has a max value of 8 (without `+`) or 12 (with `+`)
To omit a cell completely (inactive cell), just leave the encoding empty.

**Examples**:
- `2:1/13+` corresponds to a cell of value 2 in team 1 with the 1st and 3rd links missing, and of maximal value 12
- `4/2` corresponds to a neutral cell (team 0) of value 4 with the 2nd link missing (to the right)
- `0+` corresponds to a neutral cell of value 0 of maximal value 12

An example can be found in the file `maps/example.csv`.
To see its corresponding map, convert it using `./generate_map maps/example.csv` and play (or edit it) using `./main [-e] maps/example.map`.

**Warning:** The missing links must be coherent ! If a cell has the 2nd link removed (cell to the right), then the corresponding cell **must** have the 5th link removed !
If this condition is not respected, the `generate_map` binary will complain and won't create the `.map` file.

