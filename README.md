# The Hard C Bug Challenge

A deliberately hostile multi-file C debugging exercise built around one of the nastiest real-world bug families: **a pointer that is valid when cached, silently becomes invalid because an unrelated module grows a reallocating container, and is dereferenced much later**.

The challenge is intentionally structured so that the visible failure is far away from the root cause.

## Build

### GCC / Clang

```bash
make
./hardest_c_bug
```

### CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/hardest_c_bug
```

On Windows with MinGW, the executable may be under the generator-specific build directory.

## Stress knobs

```bash
./hardest_c_bug [generated_entries] [noise_rounds] [noise_width]
```

For example:

```bash
./hardest_c_bug 100000 200 256
```

Depending on allocator, optimization level, and platform, you may see:

- an invariant failure,
- heap corruption detected later,
- a crash in `free`, `malloc`, or unrelated-looking code,
- apparently correct execution,
- different behavior after adding logging,
- different behavior in Debug vs Release.

That variability is part of the exercise: the program contains undefined behavior.

## Rules of the challenge

1. Start with a normal optimized build.
2. Do not read `docs/SOLUTION.md` initially.
3. Locate the *first invalid assumption*, not merely the line where the process crashes.
4. Explain why adding a `printf` or changing optimization may change the symptom.
5. Produce a fix that preserves the general architecture.

## Sanitizer build

When you are ready to use instrumentation:

```bash
make asan
./hardest_c_bug-asan
```

or:

```bash
cmake -S . -B build-asan -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-asan
./build-asan/hardest_c_bug
```

## Why multiple files?

The lifetime violation crosses module boundaries:

```text
main -> plan -> store
          |
          | caches address
          v
       Entry *

main -> loader -> store -> realloc

main -> noise -> allocator churn

main -> plan_execute -> stale address is dereferenced
```

This is representative of real C failures where each function looks reasonable in isolation.
