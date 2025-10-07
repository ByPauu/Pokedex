@REM Compilación y Enlace con biblioteca gráfica.
@cls
@echo ---------------------------------------------------
@echo  ESAT Curso 2024-2024 Asignatura PRG Primero
@echo ---------------------------------------------------
@echo  Proceso por lotes iniciado.
@echo ---------------------------------------------------
@echo off

REM Compilar el código fuente
cl /nologo /Zi /GR- /EHs /MD grafpokedex.cc -I ./\ESAT_rev248\include ./\ESAT_rev248\bin\ESAT.lib opengl32.lib user32.lib gdi32.lib shell32.lib Ws2_32.lib

REM Verificar si la compilación fue exitosa
if %errorlevel%==0 (
    @echo ---------------------------------------------------
    @echo  Archivo Ejecutable finalizado
    @echo ---------------------------------------------------
    REM Obtener el nombre del ejecutable (el primer .exe encontrado en el directorio actual)
    for %%f in (*.exe) do (
        REM del grafpokedex.ilk
        REM del grafpokedex.obj
        REM del grafpokedex.pdb
        grafpokedex.exe
        goto end
    )
) else (
    @echo ---------------------------------------------------
    @echo  ERROR: La compilacion fallo
    @echo ---------------------------------------------------
)

:end