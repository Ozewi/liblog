# liblog library changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

## [1.1] - 2026.09.26

### Changed
- CMakeLists.txt updated to allow fetching the library from other projects.
- Aesthetical changes.

## [1.0] - 2025.04.02

First public version in github.

### Changed
- Translated into english.
- Merged with changes in the SVN repo.
- Throw exception if the verbosity level is out of range.
- Throw exception if the output file can't be open.
- Add function name to trace context.

### Added
- The log file can be opened in append or truncate mode.
- Test application 'test_log'.
- Function GetLevelName to get the name of a verbosity level (as it appears in the context).

### Fixed
- Removed unused function Dump.
- Output state is now fully saved and restored when dumping MemDump objects.

## [0.13.2] - 2024.05.08

### Fixed
- Function Log High resolution now fill with 0 instead of blanks the miliseconds.

## [0.13.1] - 2024.02.22

### Added
- Feature: Writes to the log can be serialized.
- Function Log::SetSerialize to configure whether writes to the log are serialized or not.

## [0.13.0] - 2023.12.21

### Added
- First version of liblog, extracted from libUtility.
- New 'terminate' manipulator that flushes the output and terminates the application.

### Changed
- Texts translated into english.
- 'end' manipulator no longer terminates the application.
- Default date format is now TimeOfDay.

### Removed
- Removed the macro 'LogAbort'.


0.13.2 (2025.02.10)
Tracelog: El operador nullstream::<< ahora no hace nada.

0.13.1 (2024.05.08)
Tracelog: Fix - Al mostrar timestamps hi-res, la parte de milisegundos no se rellenaba con ceros sino con espacios.

0.13.0 (2024.02.22)
Tracelog: Nueva función SetSerialization para activar el log serializado.
  Ahora el log se puede serializar, para que no salga un churro mezclado si varios threads escriben a la vez.
