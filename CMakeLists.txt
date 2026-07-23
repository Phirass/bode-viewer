cmake_minimum_required(VERSION 3.16)

project(BodeViewer VERSION 1.0 LANGUAGES CXX)

# C++17 als Sprachstandard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt-Metaobjekt-Werkzeuge (moc/uic/rcc) automatisch ausfuehren.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Benoetigte Qt-6-Module:
#   Widgets       - Fenster, Layouts, Eingabefelder
#   Charts        - Amplituden-/Phasendiagramme
#   Svg           - SVG-Export (QSvgGenerator)
#   PrintSupport  - PDF-Export (QPrinter)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Svg PrintSupport)

qt_standard_project_setup()

qt_add_executable(BodeViewer
    src/main.cpp
    src/TransferFunction.h    src/TransferFunction.cpp
    src/BodeCalculator.h      src/BodeCalculator.cpp
    src/StabilityAnalyzer.h   src/StabilityAnalyzer.cpp
    src/BodePlotPanel.h       src/BodePlotPanel.cpp
    src/PlotExporter.h        src/PlotExporter.cpp
    src/MainWindow.h          src/MainWindow.cpp
)

target_link_libraries(BodeViewer PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    Qt6::PrintSupport
)

# Unter Windows als GUI-Anwendung (ohne Konsolenfenster) starten.
set_target_properties(BodeViewer PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# --- Optionaler Konsolen-Test der Kernmodule --------------------------------
# Aktivieren mit:  cmake -S . -B build-test -DBUILD_TESTS=ON ...
option(BUILD_TESTS "Konsolen-Test der Kernmodule bauen" OFF)
if(BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Core)
    qt_add_executable(BodeCoreTest
        test/test_core.cpp
        src/TransferFunction.cpp
        src/BodeCalculator.cpp
        src/StabilityAnalyzer.cpp
    )
    target_include_directories(BodeCoreTest PRIVATE src)
    target_link_libraries(BodeCoreTest PRIVATE Qt6::Core)

    # Headless-Demo fuer den Diagramm-Export (PNG/SVG/PDF).
    qt_add_executable(BodeExportDemo
        test/export_demo.cpp
        src/TransferFunction.cpp
        src/BodeCalculator.cpp
        src/StabilityAnalyzer.cpp
        src/BodePlotPanel.cpp
        src/PlotExporter.cpp
    )
    target_include_directories(BodeExportDemo PRIVATE src)
    target_link_libraries(BodeExportDemo PRIVATE
        Qt6::Widgets Qt6::Charts Qt6::Svg Qt6::PrintSupport)
endif()
