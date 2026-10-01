This file describes changes in the ferret package.

## 1.0.17 (2026-09-28)

- Fix a pointer truncation where `long` is 32 bit, as on Windows, and the use
  of `random()`, which Windows lacks

## 1.0.16 (2026-01-20)

- Janitorial changes

## 1.0.15 (2025-09-11)

- Fix compilation with compilers that need `<cstdint>` for `uint32_t`
- Fix compiler warnings about unused variables
- Fix the return value of the availability test when the kernel module is
  missing

## 1.0.14 (2024-09-08)

- Janitorial changes

## 1.0.13 (2024-09-01)

- Use ferret for `Stabilizer` on sets of sets also in natural symmetric and
  alternating groups

## 1.0.12 (2024-08-27)

- Require GAP >= 4.12
- Load the kernel module via `LoadKernelExtension`, and warn when it is not
  compiled

## 1.0.11 (2024-04-24)

- Replace `std::random_shuffle`, removed in C++17, by `std::shuffle`

## 1.0.10 (2024-01-22)

- Fix ferret in restored GAP workspaces

## 1.0.9 (2022-10-18)

- Remove `SolveCoset`
- Derive the path to the GAP executable from `sysinfo.gap` when building
- Fix compiler warnings

## 1.0.8 (2022-07-01)

- Adapt the kernel module to stop assuming `Obj` is `UInt**`

## 1.0.7 (2022-03-30)

- Janitorial changes

## 1.0.6 (2021-10-25)

- Fix a compile error by using the standard `uint32_t` instead of `u_int32_t`
- Remove GAP 4.8-specific code

## 1.0.5 (2021-02-10)

- Avoid more `std::ostream` constructors that cause C++ ABI problems

## 1.0.4 (2021-02-09)

- Require GAP >= 4.11
- Work around C++ ABI problems with the `std::ostringstream` default
  constructor
- Compute method ranks dynamically, so ferret's `Stabilizer` methods keep their
  priority when new types are added
- Improve the readability of the manual

## 1.0.3 (2020-05-27)

- Leave stabilizers in natural symmetric and alternating groups to GAP's
  special methods
- Replace automake by a custom build system
- Stop printing in the availability test, which confused
  `TestPackageAvailability`
- Include `compiled.h` instead of `src/compiled.h`, for compatibility with
  future GAP versions
- Improve the manual and add a long example

## 1.0.2 (2019-01-17)

## 1.0.1 (2018-12-05)

## 1.0.0 (2018-03-29)

## 0.8.0 (2017-06-27)

## 0.7.1 (2016-10-19)

## 0.7.0 (2016-10-19)

## 0.6.0 (2016-07-09)

## 0.5.1 (2015-01-12)

## 0.5.0 (2014-11-21)

## 0.4.1 (2014-11-11)

## 0.4.0 (2014-11-03)

## 0.3.1 (2014-10-09)

## 0.1 (2014-08-29)

## 0.0.2 (2014-01-10)
