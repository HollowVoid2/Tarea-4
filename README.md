# Mazmorra BSP con Habitacion META

## Caracteristicas
- Generacion de mazmorras con BSP
- Habitacion META (final) con cubo amarillo
- Al tocar el cubo amarillo se reinicia el mapa
- Jugador controlable con WASD
- Conexiones organicas estilo BSP
- Sin memory leaks ni dangling pointers

## Estructura del Proyecto

TBOF_chafa/
├── src/
│ ├── main.cpp
│ ├── Room.hpp/cpp
│ ├── BSPNode.hpp/cpp
│ ├── MazeGenerator.hpp/cpp
│ ├── Player.hpp/cpp
│ └── Renderer.hpp/cpp
├── Makefile
└── README.md

text

## Compilacion
```bash
make
Ejecucion
bash
make run
Controles
Tecla	Accion
W	Moverse arriba
A	Moverse izquierda
S	Moverse abajo
D	Moverse derecha
R	Regenerar mapa
ESC	Salir
Objetivo
Encuentra el cubo AMARILLO en la habitacion META para ganar.

---

## Compilación

```bash
make clean
make
make run
Todos los archivos están listos para copiar y pegar. La estructura está organizada y el código es limpio y funcional.