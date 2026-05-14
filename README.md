# Project_PI - Crearea filmelor 3D

Proiect C++ cu OpenCV pentru generarea unei secvente video anaglife dintr-o secventa stereo de imagini BMP grayscale pe 8 biti/pixel.

## Cerinta

Pentru fiecare cadru exista doua imagini:

- `data/left/left_000.bmp`, `data/left/left_001.bmp`, ...
- `data/right/right_000.bmp`, `data/right/right_001.bmp`, ...

Aplicatia trebuie sa genereze:

- harti de disparitate / depth map in `data/output/depth_maps`;
- cadre anaglife in `data/output/frames`;
- un video final `data/output/anaglyph_video.avi`.

## Structura proiectului

```text
Project_PI/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── src/
│   ├── main.cpp
│   ├── ImageLoader.cpp
│   ├── ImageLoader.h
│   ├── AnaglyphGenerator.cpp
│   ├── AnaglyphGenerator.h
│   ├── DisparityMap.cpp
│   ├── DisparityMap.h
│   ├── VideoGenerator.cpp
│   └── VideoGenerator.h
├── data/
│   ├── left/
│   ├── right/
│   └── output/
│       ├── frames/
│       ├── depth_maps/
│       └── anaglyph_video.avi
└── docs/
    ├── screenshots/
    └── explanation.md
```

## Tehnologii folosite

- C++17
- OpenCV
- CMake
- CLion
- imagini BMP grayscale, 8 biti/pixel
- output video AVI cu `cv::VideoWriter`

## Algoritm

### Anaglif simplu

Pentru fiecare pixel se construieste o imagine color BGR:

- `B = pixel din imaginea dreapta`
- `G = pixel din imaginea dreapta`
- `R = pixel din imaginea stanga`

In OpenCV ordinea canalelor este BGR, nu RGB.

### Disparity map cu SAD

Pentru fiecare pixel din imaginea stanga se cauta pe orizontala un pixel corespondent in imaginea dreapta. Pentru fiecare disparitate posibila `d`, se compara doua ferestre patrate cu dimensiunea `windowSize`.

Costul folosit este SAD:

```text
SAD = suma |left(y, x) - right(y, x - d)|
```

Disparitatea cu cel mai mic cost este salvata pentru pixelul curent. Rezultatul este apoi normalizat in intervalul `0..255` si salvat ca imagine grayscale.

### Anaglif cu disparity

Canalul rosu vine din imaginea stanga, iar canalele verde si albastru vin din imaginea dreapta. Pixelul din dreapta este deplasat in functie de disparitatea calculata pentru a accentua efectul 3D.

## Rulare in CLion

1. Instaleaza OpenCV.
   Pe macOS cu Homebrew:

   ```bash
   brew install opencv
   ```

2. Deschide folderul `Project_PI` in CLion.

3. Verifica `CMakeLists.txt`.
   Daca OpenCV nu este in `/opt/homebrew/opt/opencv/lib/cmake/opencv4`, modifica linia:

   ```cmake
   set(OpenCV_DIR "/opt/homebrew/opt/opencv/lib/cmake/opencv4")
   ```

4. Apasa Reload CMake Project.

5. Ruleaza target-ul `Project_PI`.

## Meniu aplicatie

```text
1 - Test OpenCV
2 - Generate simple anaglyph from one stereo pair
3 - Generate disparity/depth map from one stereo pair
4 - Generate anaglyph using disparity map
5 - Process full stereo sequence
6 - Generate final video
0 - Exit
```

## Exemple input/output

Input pentru primul cadru:

```text
data/left/left_000.bmp
data/right/right_000.bmp
```

Output:

```text
data/output/depth_maps/depth_000.bmp
data/output/frames/anaglyph_000.bmp
data/output/anaglyph_video.avi
```

## Testare

1. Alege optiunea `1` pentru a verifica daca OpenCV este legat corect.
2. Pune imaginile `left_000.bmp` si `right_000.bmp` in folderele potrivite.
3. Alege optiunea `2` pentru anaglif simplu.
4. Alege optiunea `3` pentru depth map.
5. Alege optiunea `4` pentru anaglif cu disparity.
6. Pentru secventa completa, adauga perechi numerotate consecutiv sau partial si alege optiunea `5`.
7. Alege optiunea `6` pentru video final.

## Limitari

- Algoritmul SAD este simplu si lent pentru imagini mari.
- Cautarea este doar orizontala.
- Nu exista filtrare avansata pentru zone fara textura sau ocluzii.
- Marginile imaginii nu primesc disparitate calculata.
- Disparitatea este aproximativa, nu calibrare 3D reala.

## Posibile imbunatatiri

- optimizare SAD cu imagini integrale;
- filtrare median blur pe depth map;
- comparatie optionala cu `StereoBM` sau `StereoSGBM`;
- parametri configurabili din linia de comanda;
- export MP4 daca sistemul are codec potrivit;
- calibrare stereo reala pentru masuratori mai precise.

