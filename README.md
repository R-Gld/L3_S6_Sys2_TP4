# L3 Système 2 - TP n°4

## Installation:
Run this:
```shell
git clone https://github.com/R-Gld/L3_S6_Sys2_TP4.git
mkdir L3_S6_Sys2_TP4/build
cd L3_S6_Sys2_TP4/build
cmake ..
make
```

## Usage

### Exercice 1:

```shell
./build/TP3_ex2_modified
```

### Exercice 2:
Show the table generated

- `<min>`/`<max>` min/max bound
- `<line_size>` number of row / col to create.

```shell
./build/TP4_ex2 <min> <max> <line_size>
```

### Exercice 3:

Sum every number generated.
Create two thread for each line:
 - One for the initialization and generation of the number
 - One for the sum part.

- `<min>`/`<max>` min/max bound
- `<line_size>` number of row / col to create.

```shell
./build/TP4_ex3 <min> <max> <line_size>
```