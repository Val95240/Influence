
- Map serialized format:
  + `.map` file
  + hexagonal grid
  + first two bytes: height & width
  + Then: `h*w` uint16 that need:
    * 1 bit for the existence
    * 6 bits for the links with neighbours
    * 1 bit for the limit (8 or 12)
    * 4 bits for the team (15 teams max), `0000` for empty cell
    * 4 bits for the value (0 - 8/12)


- Files:
  + struct `Cell`:
    * `links`, `limit`, `team`, `value`

  + class `Map`:
    * grid of `Cell`
    * `get_neighbours(cell_pos)`
    * `serialize()`, `unserialize()`
    * `save(path)`, `load(path)`

  + class `Field(Map)`:
    * `grow_cell(cell_pos)`, `grow_cells([cell_pos, ...])`
    * `attack_cell(attacker_pos, defender_pos)`

  + `MapGenerator`:
    * Settings: overall size (S, M, ...), width, height, number of players, symmetry (1 or 2 axis depending on number of players)
    * `generate_map()`: must be connected

  + `MapDrawer`:
    * `draw(map)`

  + `MapEditor` on GUI ?
