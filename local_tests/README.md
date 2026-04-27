# Local Tests

These tests are for our own confidence while working on the assignment.
They are not required by the PDF and should be excluded from the final Moodle
submission unless we intentionally decide otherwise.

## Running The Stress Test

From the devcontainer:

```sh
make clean
EXTRA_TESTS=1 make qemu
```

Inside xv6:

```text
co_stress_test
```

Expected output:

```text
parent-first: passed 2000 handoffs
child-first: passed 2000 handoffs
co_stress_test: passed
```

Exit QEMU:

```text
Ctrl-a
x
```

Then clean generated files:

```sh
make clean
```

## Why This Is Optional

Normal builds do not include the stress test:

```sh
make qemu
```

Only this command includes it:

```sh
EXTRA_TESTS=1 make qemu
```

That means the assignment behavior stays focused on the required programs, and
we can still run the extra stress test when changing `co_yield`.

## Final Submission

Exclude this folder from Moodle:

```text
local_tests/
```

The optional Makefile support is harmless when `EXTRA_TESTS` is not set, but
for the cleanest final package we can also remove the small `EXTRA_TESTS`
Makefile block before packaging.
