# EpiGIMP

Éditeur graphique raster minimaliste, inspiré de GIMP, écrit en C++20 avec OpenGL et Dear ImGui.
Projet Epitech (professional work).

On ouvre une image, on peint dessus sur plusieurs calques, on applique des filtres, puis on exporte le résultat.

## Fonctionnalités

**Image**
- Ouverture d'une image passée en argument (PNG, JPG, BMP, TGA, GIF, PSD, HDR, PNM — tout ce que lit `stb_image`)
- Export de l'image composée en **PNG**, **BMP** ou **JPG** (qualité 90), le format est déduit de l'extension

**Calques**
- Ajout / suppression de calques (le dernier calque ne peut pas être supprimé)
- Visibilité par calque, sélection du calque actif
- Composition alpha de tous les calques visibles, du bas vers le haut
- **Masque de calque** en niveaux de gris : blanc = visible, noir = caché

**Outils**

| Outil | Raccourci | Effet |
|---|---|---|
| Pinceau | `P` | Peint avec la couleur courante. En mode masque : rend la zone visible |
| Gomme | `Shift+E` | Rend les pixels transparents. En mode masque : cache la zone |
| Pipette | `O` | Reprend la couleur de l'image sous le curseur |
| Sélection rectangulaire | `R` | Limite le pinceau et la gomme à l'intérieur du rectangle |

Chaque bouton de la barre d'outils affiche une infobulle avec son nom, son raccourci et ce qu'il fait.

**Filtres** (menu *Filtres*, appliqués au calque actif entier)
- Niveaux de gris
- Inversion des couleurs
- Luminosité / Contraste (de -100 à +100)

**Historique**
- Annuler / Rétablir sur **un seul niveau** (dernier coup de pinceau ou dernier filtre)

### Raccourcis clavier

| Action | Raccourci |
|---|---|
| Pinceau / Gomme / Pipette / Sélection | `P` / `Shift+E` / `O` / `R` |
| Annuler | `Ctrl+Z` |
| Rétablir | `Ctrl+Y` |
| Désélectionner | `Ctrl+Shift+A` |
| Taille du pinceau − / + | `[` / `]` ou `-` / `+` du pavé numérique |

Les raccourcis sont désactivés pendant la saisie dans un champ texte (ex. chemin d'export).

## Prérequis

- CMake ≥ 3.16
- Un compilateur C++20 (g++ ou clang++ récent)
- `git` (CMake télécharge GLFW au premier build)
- Les headers OpenGL et ceux nécessaires à GLFW (X11 et Wayland)

Sous Debian / Ubuntu :

```bash
sudo apt install build-essential cmake git \
    libgl-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
    libwayland-dev libxkbcommon-dev wayland-protocols
```

## Build

```bash
mkdir -p build && cd build
cmake ..
cmake --build . -j
```

L'exécutable `EpiGIMP` est généré **à la racine du dépôt** (pas dans `build/`).
Le projet compile avec `-Wall -Wextra -Werror` : tout warning casse le build.

## Lancer

```bash
./EpiGIMP chemin/vers/une/image.png
```

| Code de retour | Signification |
|---|---|
| `0` | Fermeture normale |
| `2` | Fichier d'entrée introuvable ou illisible comme image |
| `84` | Erreur (argument manquant, échec GLFW / OpenGL…) |

Le chemin d'export saisi dans l'interface est relatif au dossier depuis lequel on lance le programme.
Les icônes sont chargées depuis `assets/` via un chemin absolu fixé à la compilation, le programme peut donc être lancé depuis n'importe où.

## Dépendances

Toutes les dépendances sont fournies avec le dépôt ou récupérées automatiquement, il n'y a rien à installer à la main.

| Lib | Emplacement | Origine |
|---|---|---|
| GLFW 3.4 | téléchargée par CMake (`FetchContent`) | https://github.com/glfw/glfw |
| glad (OpenGL 3.3 Core) | `vendor/glad/` | généré depuis https://gen.glad.sh (C, API `gl`, 3.3 Core) |
| Dear ImGui 1.92.9 | `vendor/imgui/` | https://github.com/ocornut/imgui (avec `backends/`) |
| stb_image / stb_image_write | `vendor/stb/` | https://github.com/nothings/stb |

Les icônes de `assets/icons/` viennent de GIMP (CC BY-SA 4.0) et de Material Icons (Apache 2.0), voir [assets/CREDITS.md](assets/CREDITS.md).

## Architecture

```
main ──► Core ──► UI ─────────► Document ──► Layer (buffer RGBA + masque)
                  │               │
                  │               └─► Composite() ──► Canvas (texture OpenGL)
                  ├─► Filter    (filtres appliqués sur un Layer)
                  ├─► Tool      (description des outils : icône, nom, raccourci)
                  ├─► Widgets   (boutons-icônes, infobulles)
                  └─► IconLibrary ──► Icon (PNG → texture OpenGL)
```

| Module | Rôle |
|---|---|
| `Core` | Point d'entrée applicatif, boucle principale |
| `UI` | Fenêtre GLFW, contexte ImGui, entrées souris/clavier, panneaux (outils, calques, export, menu) |
| `Document` | Liste ordonnée des calques, calque actif, composition, chargement / export |
| `Layer` | Buffer RGBA, masque 8 bits, dessin au pinceau avec découpe optionnelle par sélection |
| `Canvas` | Texture OpenGL affichant le résultat composé |
| `Filter` | Niveaux de gris, inversion, luminosité / contraste |
| `Tool` | Table des outils : ajouter un outil = ajouter une entrée |
| `Widgets` | Widgets ImGui réutilisables (`IconButton`, `RichTooltip`, `Hint`) |
| `Icon` | Chargement d'icônes PNG en texture, recolorables à l'affichage |
| `Error` / `Printer` | Exceptions du projet (`Error`, `FileNotFound`, `Warning`) et affichage coloré |

## Structure du dépôt

```
.
├── CMakeLists.txt
├── assets/
│   ├── CREDITS.md       # sources et licences des icônes
│   └── icons/
├── project/
│   ├── include/         # headers, un sous-dossier par module
│   └── src/             # sources, même arborescence que include/
└── vendor/              # glad, imgui, stb
```

## Contribuer

- Une branche par issue GitHub, nommée `<numéro>-<titre-de-l-issue>` (ex. `45-upgrade-ui-ux-tools-bar`)
- Merge dans `main` via Pull Request
- Le build doit passer sans warning avant de merger
