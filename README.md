# Bode-Diagramm-Viewer

Desktop-Anwendung zur Darstellung des **Bode-Diagramms** einer generischen
Übertragungsfunktion `G(s)`. Die Software berechnet Amplituden- und Phasengang
numerisch über einen wählbaren Frequenzbereich, bewertet die Stabilität über
**Amplituden- und Phasenrand** und exportiert die Diagramme als PNG, JPG, SVG
oder PDF.

> Portfolio – Kurs *Programmierung mit C/C++* (DLBROEPRS01_D), Aufgabe 3.
> Phirass Al-Moufleh, Matrikelnummer 4252600.

---

## Funktionen

- Eingabe der Übertragungsfunktion als Zähler-/Nennerkoeffizienten
- Numerische Auswertung von `G(jω)` über einen logarithmischen Frequenzbereich
- Amplitudengang (dB) und Phasengang (Grad) mit logarithmischer Frequenzachse
- Berechnung von Amplitudenrand (gain margin) und Phasenrand (phase margin)
- Export als **PNG / JPG** (Raster) sowie **SVG / PDF** (Vektor)

## Modulare Struktur

| Modul | Datei | Aufgabe |
|-------|-------|---------|
| Modell (Übertragungsfunktion) | `TransferFunction` | speichert `G(s)`, wertet `G(jω)` aus |
| Berechnung (Kern) | `BodeCalculator` | Frequenzvektor, Betrag/Phase, Unwrapping |
| Stabilität | `StabilityAnalyzer` | Amplituden-/Phasenrand, Durchtrittsfrequenzen |
| Plot | `BodePlotPanel` | zwei Qt-Charts-Diagramme |
| Export | `PlotExporter` | PNG/JPG/SVG/PDF |
| GUI | `MainWindow` | Eingabe, Steuerung, Anzeige |

Datenfluss: `MainWindow` → `TransferFunction` → `BodeCalculator` →
`StabilityAnalyzer` → `BodePlotPanel` → `PlotExporter`.

## Eingabeformat der Koeffizienten

Koeffizienten werden in **absteigender Potenz** angegeben, getrennt durch
Leerzeichen, Komma oder Semikolon.

Beispiel: `1 3 2 0` im Nenner und `1` im Zähler ergibt

```
G(s) = 1 / (s^3 + 3 s^2 + 2 s)
```

---

## Voraussetzungen

- **Qt 6** (Widgets, Charts, Svg, PrintSupport)
- **CMake ≥ 3.16**
- Ein C++17-Compiler (MinGW oder MSVC) und **Ninja**

Am einfachsten liefert der **Qt Online Installer** alle Bestandteile in einem
Paket (Qt-Bibliotheken inkl. Modul *Qt Charts*, MinGW-Compiler, CMake, Ninja).

## Bauen und Ausführen

### Über die Kommandozeile (in der „Qt … (MinGW) Command Prompt“)

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Die ausführbare Datei liegt anschließend unter `build/BodeViewer(.exe)`.
Falls Qt nicht automatisch gefunden wird, den Qt-Pfad angeben, z. B.:

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.7.2/mingw_64"
```

### Über VS Code

1. Erweiterungspaket **Qt C++ Pack** (`theqtcompany.qt-cpp-pack`) sowie
   **CMake Tools** installieren.
2. Ordner öffnen, beim ersten Konfigurieren das Qt-Kit auswählen.
3. Mit *Build* bauen und mit *Run* starten.

## Bedienung

1. Zähler- und Nennerkoeffizienten eingeben.
2. Frequenzbereich (min/max in rad/s) und Punktzahl festlegen.
3. **„Bode-Diagramm berechnen“** klicken.
4. Amplituden-/Phasenrand werden unter den Feldern angezeigt.
5. Über **Datei → Export …** als PNG/JPG/SVG/PDF speichern.

---

## Tests

Die Kernmodule (Modell, Berechnung, Stabilität) sind unabhängig von der GUI
und werden mit einem Konsolen-Test gegen analytisch bekannte Werte geprüft.
Die Tests werden über die CMake-Option `BUILD_TESTS` gebaut:

```bash
cmake -S . -B build-test -G Ninja -DBUILD_TESTS=ON
cmake --build build-test
./build-test/BodeCoreTest        # Kern-Test (Rückgabewert 0 = alle bestanden)
```

Referenzsystem ist `G(s) = 1/(s(s+1)(s+2))` mit den erwarteten Werten
Phasenrand ≈ 53,4° (bei ≈ 0,446 rad/s) und Amplitudenrand ≈ 15,56 dB
(bei ω = √2 ≈ 1,4142 rad/s). Zusätzlich erzeugt `BodeExportDemo` das
Diagramm headless und exportiert es als PNG, JPG, SVG und PDF:

```bash
QT_QPA_PLATFORM=offscreen ./build-test/BodeExportDemo
```

---

## Git-Repository

Der vollständige, kommentierte Quellcode befindet sich unter:

`https://github.com/DEIN-BENUTZERNAME/bode-viewer`

> **Hinweis:** `DEIN-BENUTZERNAME` nach dem Anlegen des Repositorys durch
> den echten GitHub-Namen ersetzen – identisch auch im Abgabe-PDF (Abschnitt 7).
