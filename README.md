<p align="center">
  <img src="krellinux-mascot.png" alt="Krellinux mascot — Tux, as a Krell" width="340">
</p>

# Krellinux

**The Linux kernel, converted from C to Krell.**

Krellinux is a fork of the Linux kernel. Its project goal is to convert the
kernel from C to [Krell](https://github.com/nightwarelabz/krell) — completely,
subsystem by subsystem, while keeping the tree building and booting at every
step.

## Status

Phase 0 is complete: the conversion has begun, and the tree still builds
and boots.

- **Census.** The Auto-Convergence Engine (`tools/converge/`) has
  classified all of `lib/` — 212 files, 3,418 function definitions:
  83 AUTO (convertible with Krell as it stands), 104 ASSIST (blocked by
  named Krell gaps), and 25 awaiting a config-faithful census. The
  conversion ledger is [`CONVERSION.md`](CONVERSION.md); per-file
  blockers are in `tools/converge/CENSUS-lib.md`.
- **First conversion.** `crc16` (`lib/crc/`) is rewritten in Krell and
  proven equal to its C original on a 3,563-case differential corpus,
  with zero divergence.
- **Integration spike.** Kbuild compiles `.krell` sources through
  krellc into vmlinux; the Krell crc16 runs inside the booted kernel
  and passes its boot-time self-check (verified in QEMU).

Next: Phase 1 — converting the AUTO class of `lib/`, function by
function, each one proven against its C original before it replaces
it.

## What is Krell?

Krell is a machine-facing portable assembly: a program representation between
assembly and C, designed for machines to produce and consume, not for human
readers.

- Typed SSA with block arguments, as a hash-consed graph
- Scalable vectors only; a backend-owned C ABI
- A curated operation set chosen to be efficient on every target, with a
  no-cliff verifier rule
- A per-target cost oracle, a reference interpreter, an optimizer, and a
  bounded fragment superoptimizer
- Backends: x86_64, AArch64, RISC-V 64, and an LLVM lane

On the twelve KRELLPROOF kernels, Krell v3.0 compiles code measured at
1.81× the speed of C -O2 (native x86_64) and 1.59× (LLVM lane) —
geometric mean, Fibonacci excluded. Full record:
[Krell v3.0 release](https://github.com/nightwarelabz/krell/releases/tag/v3.0).

Krell is licensed under Apache-2.0, with the Krell Encoding Exception:
Krell-encoded programs are not derivative works and carry no license
obligations.

## Conversion approach

- **Subsystem by subsystem.** Each part of the kernel is converted as a unit,
  with its behaviour preserved.
- **Semantics first.** A conversion is only done when the Krell form matches
  the C form's behaviour; the tree must keep building and booting throughout.
- **Upstream stays reachable.** The fork tracks upstream Linux so upstream
  fixes and improvements can keep flowing in while conversion proceeds.

## Not K0

Krellinux is distinct from [K0](https://github.com/nightwarelabz/k0), a
separate from-scratch kernel written in pure Krell. Krellinux converts the
existing Linux kernel; K0 starts from an empty tree.

## Upstream base

- Forked from: Linux **7.4.0-rc1** ("Baby Opossum Posse")
- Base commit: `05441d697f7f9a927264f4058cbbc6a03dfcac9d`

Everything below this fork point is the Linux kernel, and the upstream
documentation applies unchanged:

- The original upstream README is preserved as [`README`](README) in this tree
- Report a bug: `Documentation/admin-guide/reporting-issues.rst`
- Build the kernel: `Documentation/admin-guide/quickly-build-trimmed-linux.rst`
- Documentation online: https://www.kernel.org/doc/html/latest/
- Building requirements: `Documentation/process/changes.rst`
- Code of Conduct: `Documentation/process/code-of-conduct.rst`

## Licence

The kernel tree remains under its upstream licence — see
[`COPYING`](COPYING) (GPL-2.0). The Krellinux mascot image
(`krellinux-mascot.png`) is part of this project's documentation.
