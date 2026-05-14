# Explicatie proiect

## 1. Setup proiect

Proiectul foloseste CMake pentru a lega OpenCV:

```cmake
find_package(OpenCV REQUIRED)
target_link_libraries(Project_PI PRIVATE ${OpenCV_LIBS})
```

Codul sursa este impartit in fisiere din `src/`, iar datele de intrare si iesire sunt in `data/`.

## 2. Citirea perechii stereo

`ImageLoader` verifica daca fisierele exista, daca imaginile pot fi citite, daca sunt grayscale pe 8 biti/pixel si daca au aceeasi dimensiune.

## 3. Anaglif simplu

`AnaglyphGenerator::createSimpleAnaglyph` creeaza o imagine `CV_8UC3`.

Pentru fiecare pixel:

```text
B = right
G = right
R = left
```

## 4. Disparity map

`DisparityMap::computeSAD` cauta pentru fiecare pixel cea mai buna deplasare orizontala intre imaginea stanga si imaginea dreapta.

Parametri importanti:

- `windowSize`: dimensiunea ferestrei comparate, de exemplu 3, 5 sau 7;
- `maxDisparity`: deplasarea maxima cautata, de exemplu 16, 32 sau 64.

## 5. Normalizare depth map

Disparitatea calculata este in intervalul `0..maxDisparity`. Pentru salvare ca BMP grayscale, se transforma in `0..255`.

## 6. Anaglif cu disparity

Imaginea dreapta este citita din pozitia `x - shift`, unde `shift` este obtinut din disparity map. Daca pozitia iese din imagine, pixelul este tratat cu valoarea 0 pentru a evita crash-ul.

## 7. Procesarea secventei

Aplicatia cauta automat perechi:

```text
left_000.bmp / right_000.bmp
left_001.bmp / right_001.bmp
...
```

Pentru fiecare pereche salveaza depth map si frame anaglif.

## 8. Video final

`VideoGenerator` citeste cadrele `anaglyph_000.bmp`, `anaglyph_001.bmp`, ... si le scrie intr-un fisier AVI folosind `cv::VideoWriter`.

