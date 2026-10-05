# Guide de démarrage — CASIO GUI sur GRAPH 90+E

Ce guide décrit une chaîne de développement simple sous Windows pour créer, compiler et tester des add-ins C++ pour **CASIO GRAPH 90+E / fx-CG50** avec **fxSDK**, **gint**, **CASIO_GUI** et **casio-emu**.

La chaîne obtenue est :

```text
Windows 11
   │
   └── WSL 2 / Ubuntu
          │
          ├── GiteaPC
          │    ├── fxSDK
          │    ├── sh-elf-binutils
          │    ├── sh-elf-gcc / libstdc++
          │    ├── OpenLibm
          │    ├── fxlibc
          │    └── gint
          │
          ├── CASIO_GUI
          │
          ├── fxsdk build-cg
          │       │
          │       └── MonProjet.g3a
          │
          └── casio-emu
                  │
                  └── test immédiat sur le PC
```

---

## 1. Installer WSL 2 et Ubuntu

Ouvrir **PowerShell en administrateur** puis exécuter :

```powershell
wsl --install
```

Redémarrer Windows si demandé.

La commande installe normalement WSL 2 et Ubuntu.

Pour vérifier les distributions :

```powershell
wsl -l -v
```

On doit obtenir quelque chose de ce type :

```text
NAME      STATE    VERSION
Ubuntu    Running  2
```

Si Ubuntu n'est pas installé :

```powershell
wsl --list --online
wsl --install -d Ubuntu
```

Au premier lancement d'Ubuntu, créer le nom d'utilisateur Linux et le mot de passe demandés.

### Mise à jour de WSL

Depuis PowerShell administrateur :

```powershell
wsl --update
wsl --shutdown
```

Puis relancer Ubuntu.

Cette étape est également utile pour disposer correctement du support des applications graphiques Linux via WSLg.

---

## 2. Préparer Ubuntu

Dans le terminal Ubuntu/WSL :

```bash
sudo apt update
sudo apt upgrade -y
```

Installer les outils de base et les dépendances utilisées par fxSDK, gint et l'émulateur :

```bash
sudo apt install -y \
    build-essential \
    git \
    curl \
    cmake \
    pkg-config \
    python3 \
    python3-pil \
    ca-certificates \
    openssl \
    libusb-1.0-0-dev \
    libudev-dev \
    libsdl2-dev \
    libpng-dev \
    libncurses-dev
```

Pour certaines fonctions de `fxlink`, on peut également installer :

```bash
sudo apt install -y libudisks2-dev libglib2.0-dev
```

Il est conseillé de conserver les projets Linux directement dans le système de fichiers WSL, par exemple :

```bash
mkdir -p ~/casio
cd ~/casio
```

---

## 3. Installer GiteaPC

GiteaPC facilite l'installation et la mise à jour de la toolchain CASIO.

Télécharger puis lancer l'installateur :

```bash
curl "https://git.planet-casio.com/Lephenixnoir/GiteaPC/raw/branch/master/install.sh" \
    -o /tmp/giteapc-install.sh && \
bash /tmp/giteapc-install.sh
```

Pendant l'installation, accepter l'ajout de :

```text
~/.local/bin
```

au `PATH`.

Ensuite, fermer/réouvrir Ubuntu ou exécuter :

```bash
source ~/.profile
```

Vérifier :

```bash
giteapc --help
```

### En cas d'erreur TLS avec GiteaPC

Si `curl` affiche par exemple :

```text
TLS connect error
unexpected eof while reading
```

réinstaller les certificats :

```bash
sudo apt install --reinstall ca-certificates curl openssl
sudo update-ca-certificates
```

Puis réessayer en forçant IPv4 :

```bash
curl -4 "https://git.planet-casio.com/Lephenixnoir/GiteaPC/raw/branch/master/install.sh" \
    -o /tmp/giteapc-install.sh
```

Vérifier que le fichier existe :

```bash
ls -lh /tmp/giteapc-install.sh
```

Puis :

```bash
bash /tmp/giteapc-install.sh
```

---

## 4. Installer fxSDK et le compilateur SuperH

Installer fxSDK, binutils et GCC :

```bash
giteapc install \
    Lephenixnoir/fxsdk \
    Lephenixnoir/sh-elf-binutils \
    Lephenixnoir/sh-elf-gcc
```

Le compilateur utilisé pour les add-ins est notamment :

```text
sh-elf-gcc
sh-elf-g++
```

Vérifier :

```bash
fxsdk --version
sh-elf-gcc --version
sh-elf-g++ --version
```

---

## 5. Installer les bibliothèques nécessaires à gint et au C++

Installer OpenLibm et fxlibc :

```bash
giteapc install \
    Lephenixnoir/OpenLibm \
    Vhex-Kernel-Core/fxlibc
```

Relancer ensuite l'installation de GCC :

```bash
giteapc install Lephenixnoir/sh-elf-gcc
```

Cette seconde passe permet notamment de disposer correctement de la bibliothèque standard C++ pour la toolchain.

Installer enfin gint :

```bash
giteapc install Lephenixnoir/gint
```

Le debugger GDB est facultatif :

```bash
giteapc install Lephenixnoir/sh-elf-gdb
```

### Vérifications

```bash
giteapc list
```

Puis :

```bash
find ~/.local -iname "FindGint.cmake" -print
```

Une installation fonctionnelle peut notamment contenir :

```text
~/.local/lib/cmake/fxsdk/FindGint.cmake
```

---

## 6. Tester fxSDK avant CASIO_GUI

Créer un répertoire de travail :

```bash
cd ~/casio
```

Créer un projet fxSDK :

```bash
fxsdk new TestFXSDK
cd TestFXSDK
```

Compiler pour GRAPH 90+E / fx-CG50 :

```bash
fxsdk build-cg
```

Le résultat attendu est un fichier :

```text
*.g3a
```

à la racine du projet.

Pour le rechercher :

```bash
find . -maxdepth 2 -name "*.g3a" -print
```

Pour refaire une compilation complètement propre :

```bash
rm -rf build-cg
fxsdk build-cg
```

---

# 7. Créer un projet utilisant CASIO_GUI

Exemple d'arborescence :

```text
CASIO_GAME/
├── CMakeLists.txt
├── assets-cg/
│   ├── icon-uns.png
│   └── icon-sel.png
└── src/
    ├── main.cpp
    └── CASIO_GUI/
        ├── CMakeLists.txt
        ├── casio.hpp
        ├── cursor/
        │   └── cursor.cpp
        ├── gui/
        │   └── gui.cpp
        ├── item/
        │   └── item.cpp
        ├── screenshot/
        │   └── screenshot.cpp
        └── lib/
            └── libc_stubs.c
```

Les éventuels `.hpp` de CASIO_GUI restent naturellement placés avec les modules correspondants.

---

## 8. CMakeLists.txt principal

Voici un modèle simple destiné à la GRAPH 90+E :

```cmake
# Configure with "fxsdk build-cg".
cmake_minimum_required(VERSION 3.15)

project(CASIO_GAME VERSION 1.0.0 LANGUAGES C CXX)

include(GenerateG3A)
include(Fxconv)

find_package(Gint 2.9 REQUIRED)

# ---------------------------------------------------------------------------
# CASIO GUI
# ---------------------------------------------------------------------------

add_subdirectory(src/CASIO_GUI)

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------

set(SOURCES
    src/main.cpp
)

# Ajouter ici les images réellement utilisées par le programme.
# Les deux icônes du G3A ne doivent pas être placées dans cette liste.
set(ASSETS_cg
    # assets-cg/mon_image.png
)

fxconv_declare_assets(${ASSETS_cg} WITH_METADATA)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${ASSETS_cg}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Os
)

target_link_libraries(${PROJECT_NAME}
    CASIO_GUI::CASIO_GUI
    Gint::Gint
    stdc++
)

# ---------------------------------------------------------------------------
# Génération du fichier .g3a
# ---------------------------------------------------------------------------

generate_g3a(
    TARGET ${PROJECT_NAME}
    OUTPUT "${PROJECT_NAME}.g3a"
    NAME "CASIO GAME"
    ICONS
        assets-cg/icon-uns.png
        assets-cg/icon-sel.png
)
```

Les icônes G3A sont normalement des images PNG de **92 × 64 pixels**.

---

## 9. CMakeLists.txt de CASIO_GUI

Dans :

```text
src/CASIO_GUI/CMakeLists.txt
```

utiliser par exemple :

```cmake
add_library(CASIO_GUI STATIC
    cursor/cursor.cpp
    item/item.cpp
    gui/gui.cpp
    screenshot/screenshot.cpp
    lib/libc_stubs.c
)

add_library(CASIO_GUI::CASIO_GUI ALIAS CASIO_GUI)

target_include_directories(CASIO_GUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(CASIO_GUI PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(CASIO_GUI
    PUBLIC
        Gint::Gint
)

# Nécessaire pour le code C++ actuel de CASIO_GUI dans la toolchain fxSDK.
target_compile_options(CASIO_GUI
    PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:-fno-freestanding>
)
```

La bibliothèque est alors construite sous forme statique :

```text
libCASIO_GUI.a
```

avant le linkage de l'application.

---

# 10. Premier programme CASIO_GUI

Exemple minimal :

```cpp
#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    casio::RawItemColor buttonColor(
        casio::WHITE,
        casio::BLACK,
        casio::BLACK
    );

    casio::Button btnA(
        casio::Position{10, 10, 100, 50},

        casio::ItemEvent{
            casio::ItemEvent::eventType::KEY_DOWN,
            KEY_EXE,
            [&gui]() {
                gui.openScreenshotPrompt();
            }
        },

        casio::ItemStatus{
            false,
            false,
            false,
            false,
            false,
            true,
            false
        },

        casio::ItemColor{
            buttonColor,
            buttonColor,
            buttonColor,
            buttonColor,
            buttonColor,
            buttonColor,
            buttonColor
        },

        casio::ItemLabel{
            "Take Screenshot",
            "",
            "",
            "",
            "",
            ""
        }
    );

    // Un item créé mais non ajouté à la GUI n'est pas affiché.
    gui.addItem(&btnA);

    gui.runGUI();

    return 0;
}
```

Point important :

```cpp
gui.addItem(&btnA);
```

est indispensable pour enregistrer le bouton dans la GUI.

---

## 11. Compiler le projet CASIO_GUI

Depuis la racine du projet :

```bash
cd ~/casio/CASIO_GAME
fxsdk build-cg
```

Compilation complètement propre :

```bash
rm -rf build-cg
fxsdk build-cg
```

On doit voir une séquence proche de :

```text
Building CXX object ... CASIO_GUI ...
Linking CXX static library libCASIO_GUI.a
Built target CASIO_GUI
Building CXX object ... src/main.cpp ...
Linking ...
Built target CASIO_GAME
```

Puis :

```bash
ls -lh *.g3a
```

---

# 12. Installer casio-emu

L'émulateur permet de tester directement le `.g3a` sur le PC sans le recopier systématiquement sur la calculatrice.

Créer un répertoire pour les outils :

```bash
mkdir -p ~/casio-tools
cd ~/casio-tools
```

Les dépendances nécessaires sont déjà normalement présentes. Sinon :

```bash
sudo apt install -y build-essential cmake git libsdl2-dev
```

Cloner l'émulateur :

```bash
git clone https://github.com/Heath123/casio-emu.git
cd casio-emu
```

Construire la version SDL :

```bash
cmake -S . -B build -DUSE_SDL_GUI=ON
cmake --build build -j$(nproc)
```

L'exécutable obtenu est :

```text
~/casio-tools/casio-emu/build/calcemu
```

---

## 13. Lancer un G3A dans l'émulateur

Exemple :

```bash
cd ~/casio/CASIO_GAME

~/casio-tools/casio-emu/build/calcemu \
    ./CASIO_GAME.g3a
```

Le cycle de développement devient donc :

```text
Modifier le code
      │
      ▼
fxsdk build-cg
      │
      ▼
CASIO_GAME.g3a
      │
      ▼
casio-emu
      │
      ▼
Test sur PC
```

Il reste conseillé de faire une validation finale sur une vraie GRAPH 90+E, en particulier pour les fonctionnalités matérielles.

---

# 14. Compiler et lancer en une seule commande

Créer à la racine du projet :

```text
run-emu.sh
```

avec :

```bash
#!/usr/bin/env bash

set -euo pipefail

PROJECT_NAME="CASIO_GAME"
EMULATOR="${HOME}/casio-tools/casio-emu/build/calcemu"

if [ "${1:-}" = "clean" ]; then
    echo "=== Clean ==="
    rm -rf build-cg
fi

echo "=== Build ${PROJECT_NAME} ==="
fxsdk build-cg

echo "=== Run emulator ==="
"${EMULATOR}" "./${PROJECT_NAME}.g3a"
```

Le rendre exécutable :

```bash
chmod +x run-emu.sh
```

Utilisation normale :

```bash
./run-emu.sh
```

Compilation propre + lancement :

```bash
./run-emu.sh clean
```

---

# 15. Utiliser VS Code avec WSL

Il est possible d'éditer directement le projet WSL depuis VS Code.

Installer dans VS Code l'extension Microsoft :

```text
WSL
```

Puis dans Ubuntu :

```bash
cd ~/casio/CASIO_GAME
code .
```

VS Code doit indiquer qu'il travaille dans l'environnement WSL.

Le terminal intégré peut alors directement exécuter :

```bash
fxsdk build-cg
```

ou :

```bash
./run-emu.sh
```

---

# 16. Problèmes fréquents

## `giteapc: command not found`

Recharger le profil :

```bash
source ~/.profile
```

Vérifier :

```bash
echo "$PATH"
```

Si nécessaire :

```bash
export PATH="$PATH:$HOME/.local/bin"
```

---

## `Could not find Gint`

Vérifier :

```bash
find ~/.local -iname "FindGint.cmake" -print
```

Une installation normale peut renvoyer :

```text
/home/<user>/.local/lib/cmake/fxsdk/FindGint.cmake
```

Puis reconstruire complètement :

```bash
rm -rf build-cg
fxsdk build-cg
```

---

## `RawitemColor is not a member of casio`

Utiliser :

```cpp
casio::RawItemColor
```

et non :

```cpp
casio::RawitemColor
```

---

## `C_WHITE` ou `C_BLACK` inconnus

Avec l'API publique actuelle de CASIO_GUI :

```cpp
casio::WHITE
casio::BLACK
```

---

## `ItemColor` attend 7 paramètres

`ItemColor` contient les couleurs correspondant aux différents états visuels de l'item.

Pour un test simple, on peut réutiliser le même état :

```cpp
casio::RawItemColor c(
    casio::WHITE,
    casio::BLACK,
    casio::BLACK
);

casio::ItemColor{
    c, c, c, c, c, c, c
}
```

---

## La GUI est vide

Vérifier que chaque widget a bien été ajouté :

```cpp
gui.addItem(&btnA);
```

Créer un objet ne suffit pas à l'enregistrer dans la GUI.

---

## L'émulateur ne s'ouvre pas sous WSL

Vérifier que la distribution utilise WSL 2 :

```powershell
wsl -l -v
```

Depuis PowerShell administrateur :

```powershell
wsl --update
wsl --shutdown
```

Puis relancer Ubuntu.

---

## Limites de casio-emu

`casio-emu` est particulièrement pratique pour le développement rapide d'add-ins utilisant gint, mais ce n'est pas une émulation complète du système d'exploitation CASIO.

En particulier, le projet indique que :

- les add-ins dépendant de syscalls non émulés peuvent ne pas fonctionner ;
- les add-ins utilisant le pilote USB ne démarrent pas ;
- certaines différences avec le matériel réel restent possibles.

La GRAPH 90+E physique reste donc la référence pour la validation finale.

---

# 17. Mise à jour de la toolchain

GiteaPC permet de mettre à jour les composants installés.

Exemple :

```bash
giteapc install -u \
    Lephenixnoir/fxsdk \
    Lephenixnoir/sh-elf-binutils \
    Lephenixnoir/sh-elf-gcc \
    Lephenixnoir/OpenLibm \
    Vhex-Kernel-Core/fxlibc \
    Lephenixnoir/gint
```

Après une mise à jour importante, reconstruire le projet proprement :

```bash
rm -rf build-cg
fxsdk build-cg
```

---

# 18. Mémo rapide

Installation initiale :

```text
Windows
  └─ wsl --install
       │
       ▼
Ubuntu
  └─ apt install dépendances
       │
       ▼
GiteaPC
  └─ fxSDK + sh-elf-gcc + gint
       │
       ▼
CASIO_GUI
       │
       ▼
fxsdk build-cg
       │
       ▼
CASIO_GAME.g3a
       │
       ├─ casio-emu
       └─ GRAPH 90+E
```

Commandes utilisées au quotidien :

```bash
cd ~/casio/CASIO_GAME

# Compiler
fxsdk build-cg

# Recompiler complètement
rm -rf build-cg
fxsdk build-cg

# Compiler + émuler
./run-emu.sh

# Clean + compiler + émuler
./run-emu.sh clean
```

---

# 19. Références

- Microsoft Learn — Installation et utilisation de WSL / WSL 2.
- Planète Casio — GiteaPC, fxSDK et documentation CMake.
- Lephenixnoir/gint — installation via GiteaPC.
- Heath123/casio-emu — émulateur fx-CG50/GRAPH 90+E pour add-ins gint.

Document préparé pour une chaîne **Windows 11 + WSL 2 + Ubuntu + fxSDK + gint + CASIO_GUI + casio-emu**.
