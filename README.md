# EpiGIMP

Éditeur graphique raster minimaliste, inspiré de GIMP, écrit en C++ / Dear ImGui.
Projet Epitech (professional work).

## Prérequis

- CMake >= 3.16
- Un compilateur supportant C++20 (g++ ou clang++ récent)
- Les headers de développement OpenGL (`libgl-dev` / `libglvnd-dev` sous Debian/Ubuntu)

## Dépendances (dossier `vendor/`)

| Lib | Récupération |
|---|---|
| GLFW 3.4 | Automatique via `FetchContent` (CMake), rien à faire |
| glad (OpenGL 3.3 Core) | `vendor/glad/` — généré depuis https://gen.glad.sh (langage C, API `gl`, version 3.3 Core) |
| Dear ImGui 1.92.9 | `vendor/imgui/` — cloné depuis https://github.com/ocornut/imgui (avec le dossier `backends/`) |
| stb_image / stb_image_write | `vendor/stb/` — récupérés via : |

```bash
mkdir -p vendor/stb
curl -L -o vendor/stb/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
curl -L -o vendor/stb/stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
```

## Build

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

L'exécutable `EpiGIMP` est généré à la racine du projet (pas dans `build/`).

## Lancer

```bash
./EpiGIMP chemin/vers/une/image.png
```

Formats d'entrée supportés : PNG, JPG, BMP (via stb_image).

## Fonctionnalités actuelles

- Chargement d'une image et affichage dans un canvas (texture OpenGL)
- Export de l'image affichée vers PNG / BMP / JPG, via un champ de texte + bouton dans l'interface

## Structure du projet

```
project/
├── include/    # headers, un sous-dossier par module (Core, UI, Canvas, Error, Printer)
└── src/        # sources, même arborescence que include/
```