#!/usr/bin/env python3
"""Auto-Convergence Engine v0 -- construct census (libclang edition).

For every C file in a target directory (default: lib/), parse the file
with libclang using its real kernel compile flags (from
compile_commands.json) and count the constructs that decide the
file's conversion class against the Krell capability table:

  AUTO    -- only constructs Krell expresses directly
  ASSIST  -- convertible after a named gap is closed or a rewrite is
             applied (indirect calls, cmpxchg-family atomics,
             volatile, varargs, bitfields, i128, inline asm)
  RESIDUE -- stays C/asm for now (file-scope asm, clang parse failure)

Only AST nodes whose spelling location is the target file itself are
counted (headers are context, not the file's own constructs).

Output: census-<dir>.json (per-file detail) and CENSUS-<dir>.md
(summary with the blocker histogram = what the tree needs, in order).
"""
import ctypes, json, os, sys, collections

_lib = None
for _cand in ("/usr/lib/llvm-18/lib/libclang-18.so.1",
              "/usr/lib/x86_64-linux-gnu/libclang-18.so.1",
              "/usr/lib/llvm-18/lib/libclang.so.1"):
    if os.path.exists(_cand):
        _lib = ctypes.CDLL(_cand)
        break
if _lib is None:
    sys.exit("libclang not found")

class CXString(ctypes.Structure):
    _fields_ = [("data", ctypes.c_void_p), ("private_flags", ctypes.c_uint)]

class CXCursor(ctypes.Structure):
    _fields_ = [("kind", ctypes.c_int), ("xdata", ctypes.c_int),
                ("data", ctypes.c_void_p * 3)]

class CXType(ctypes.Structure):
    _fields_ = [("kind", ctypes.c_int), ("data", ctypes.c_void_p * 2)]

class CXSourceLocation(ctypes.Structure):
    _fields_ = [("ptr_data", ctypes.c_void_p * 2),
                ("int_data", ctypes.c_uint)]

_lib.clang_createIndex.restype = ctypes.c_void_p
_lib.clang_parseTranslationUnit.restype = ctypes.c_void_p
_lib.clang_parseTranslationUnit.argtypes = [ctypes.c_void_p,
    ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.c_int,
    ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint]
_lib.clang_getTranslationUnitCursor.restype = CXCursor
_lib.clang_getTranslationUnitCursor.argtypes = [ctypes.c_void_p]
_lib.clang_getCursorKind.restype = ctypes.c_int
_lib.clang_getCursorKind.argtypes = [CXCursor]
_lib.clang_getCursorKindSpelling.restype = CXString
_lib.clang_getCursorKindSpelling.argtypes = [ctypes.c_int]
_lib.clang_getCursorType.restype = CXType
_lib.clang_getCursorType.argtypes = [CXCursor]
_lib.clang_getTypeSpelling.restype = CXString
_lib.clang_getTypeSpelling.argtypes = [CXType]
_lib.clang_getCursorSpelling.restype = CXString
_lib.clang_getCursorSpelling.argtypes = [CXCursor]
_lib.clang_isCursorDefinition.restype = ctypes.c_uint
_lib.clang_isCursorDefinition.argtypes = [CXCursor]
_lib.clang_getFieldDeclBitWidth.restype = ctypes.c_int
_lib.clang_getFieldDeclBitWidth.argtypes = [CXCursor]
_lib.clang_getNumDiagnostics.restype = ctypes.c_uint
_lib.clang_getNumDiagnostics.argtypes = [ctypes.c_void_p]
_lib.clang_getDiagnostic.restype = ctypes.c_void_p
_lib.clang_getDiagnostic.argtypes = [ctypes.c_void_p, ctypes.c_uint]
_lib.clang_getDiagnosticSeverity.restype = ctypes.c_int
_lib.clang_getDiagnosticSeverity.argtypes = [ctypes.c_void_p]
_lib.clang_getDiagnosticSpelling.restype = CXString
_lib.clang_getDiagnosticSpelling.argtypes = [ctypes.c_void_p]
_lib.clang_createIndex.argtypes = [ctypes.c_int, ctypes.c_int]
_lib.clang_getFileName.argtypes = [ctypes.c_void_p]
_lib.clang_disposeString.argtypes = [CXString]
_lib.clang_getCursorLocation.restype = CXSourceLocation
_lib.clang_getCursorLocation.argtypes = [CXCursor]
_lib.clang_getSpellingLocation.argtypes = [CXSourceLocation,
    ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_uint)]
_lib.clang_getFileName.restype = CXString
_lib.clang_visitChildren.restype = ctypes.c_uint
_lib.clang_visitChildren.argtypes = [CXCursor, ctypes.c_void_p,
                                     ctypes.c_void_p]

CURSOR_CB = ctypes.CFUNCTYPE(ctypes.c_int, CXCursor, CXCursor,
                             ctypes.c_void_p)

def cxstr(s):
    if not s.data:
        return ""
    out = ctypes.cast(s.data, ctypes.c_char_p).value or b""
    _lib.clang_disposeString(s)
    return out.decode("utf-8", "replace")

def kind_of(cur):
    return cxstr(_lib.clang_getCursorKindSpelling(
        _lib.clang_getCursorKind(cur)))

def type_of(cur):
    return cxstr(_lib.clang_getTypeSpelling(_lib.clang_getCursorType(cur)))

def name_of(cur):
    return cxstr(_lib.clang_getCursorSpelling(cur))

def file_of(cur):
    f = ctypes.c_void_p()
    ln = ctypes.c_uint(); col = ctypes.c_uint(); off = ctypes.c_uint()
    _lib.clang_getSpellingLocation(_lib.clang_getCursorLocation(cur),
                                   ctypes.byref(f), ctypes.byref(ln),
                                   ctypes.byref(col), ctypes.byref(off))
    if not f:
        return ""
    return cxstr(_lib.clang_getFileName(f))

FEATURE_KEYS = ["functions", "indirect_calls", "fn_pointer_types",
                "varargs", "atomic_exprs", "atomic_api_calls",
                "volatile_uses", "bitfields", "i128_uses", "inline_asm",
                "file_scope_asm", "gotos"]
ATOMIC_NAME_HINTS = ("atomic_", "cmpxchg", "xchg", "try_cmpxchg",
                     "atomic64_", "this_cpu")

def first_child(cur):
    got = []
    def cb(c, p, d):
        got.append(c)
        return 0  # break after first
    _lib.clang_visitChildren(cur, CURSOR_CB(cb), None)
    return got[0] if got else None

def callee_kind(cur):
    """Peel a CallExpr's callee to its deciding cursor kind/name."""
    c = first_child(cur)
    depth = 0
    while c is not None and depth < 8:
        k = kind_of(c)
        if k in ("ImplicitCastExpr", "ParenExpr", "UnexposedExpr"):
            nxt = first_child(c)
            if nxt is None:
                return k, name_of(c)
            c = nxt; depth += 1; continue
        return k, name_of(c)
    return "?", ""

def census_file(path, args):
    st = {k: 0 for k in FEATURE_KEYS}
    idx = _lib.clang_createIndex(0, 0)
    cargs = (ctypes.c_char_p * len(args))(*[a.encode() for a in args])
    tu = _lib.clang_parseTranslationUnit(
        idx, path.encode(), cargs, len(args), None, 0, 0)
    if not tu:
        return st, "translation unit failed"
    err = None
    for i in range(_lib.clang_getNumDiagnostics(tu)):
        d = _lib.clang_getDiagnostic(tu, i)
        if _lib.clang_getDiagnosticSeverity(d) >= 4:  # fatal only
            err = cxstr(_lib.clang_getDiagnosticSpelling(d))[:300]
            break
    target = os.path.realpath(path)
    state = {"fn_depth": 0}
    def cb(cur, parent, data):
        if file_of(cur) == target:
            k = kind_of(cur)
            if k == "FunctionDecl" and _lib.clang_isCursorDefinition(cur):
                # definitions only: declaration-only FunctionDecls (the
                # __compiletime_assert_N externs and prototypes) are not
                # code the conversion must translate
                st["functions"] += 1
                state["fn_depth"] += 1
                _lib.clang_visitChildren(cur, CURSOR_CB(cb), None)
                state["fn_depth"] -= 1
                return 1  # continue with siblings; children done manually
            typ = type_of(cur)
            if "__int128" in typ:
                st["i128_uses"] += 1
            if "va_list" in typ or "__builtin_va" in typ:
                st["varargs"] += 1
            if k == "GCCAsmStmt":
                if state["fn_depth"]:
                    st["inline_asm"] += 1
                else:
                    st["file_scope_asm"] += 1
            elif k == "AtomicExpr":
                st["atomic_exprs"] += 1
            elif k == "VAArgExpr":
                st["varargs"] += 1
            elif k == "GotoStmt":
                st["gotos"] += 1
            elif k == "FieldDecl":
                if _lib.clang_getFieldDeclBitWidth(cur) >= 0:
                    st["bitfields"] += 1
                if "volatile" in typ:
                    st["volatile_uses"] += 1
                if "(*)" in typ:
                    st["fn_pointer_types"] += 1
            elif k in ("VarDecl", "ParmVarDecl", "ParmDecl"):
                if "volatile" in typ:
                    st["volatile_uses"] += 1
                if "(*)" in typ:
                    st["fn_pointer_types"] += 1
            elif k == "CallExpr":
                ck, nm = callee_kind(cur)
                if ck == "DeclRefExpr":
                    if any(h in nm for h in ATOMIC_NAME_HINTS):
                        st["atomic_api_calls"] += 1
                elif ck not in ("", "?"):
                    st["indirect_calls"] += 1
        return 2  # recurse
    root = _lib.clang_getTranslationUnitCursor(tu)
    _lib.clang_visitChildren(root, CURSOR_CB(cb), None)
    return st, err

GCC_ONLY = ("-mindirect-branch", "-fconserve-stack",
            "-fno-var-tracking-assignments", "-mskip-rax-setup",
            "-fno-jump-tables", "--param", "-flive-patching",
            "-fpatchable-function-entry", "-ftrivial-auto-var-init",
            "-mpreferred-stack-boundary", "-femit-struct-debug-baseonly",
            "-mtraceback")

def clean_args(cmd, target_file):
    """Flags for libclang: keep includes/defines/std; drop the source
    file (it is the parse target, passed separately), the output,
    every -W flag (warnings do not change the AST; gcc-only warning
    spellings are hard errors under clang), and gcc-only codegen
    flags clang rejects."""
    args = cmd["arguments"] if "arguments" in cmd else cmd["command"].split()
    out, skip = [], False
    for i, a in enumerate(args):
        if i == 0 or skip:
            skip = False
            continue
        if a == "-o":
            skip = True
            continue
        if a == "-c":
            continue
        if a.endswith(".c") and os.path.basename(a) == os.path.basename(target_file):
            continue  # the input file itself, in either path spelling
        if a.startswith("-W"):
            continue
        if any(a.startswith(g) for g in GCC_ONLY):
            if a == "--param":
                skip = True
            continue
        out.append(a)
    return out

def classify(st, err):
    if err:
        return "RESIDUE", "clang parse error: " + err
    if st["file_scope_asm"]:
        return "RESIDUE", "file-scope inline asm"
    b = []
    if st["indirect_calls"] or st["fn_pointer_types"]:
        b.append("indirect calls / function pointers")
    if st["atomic_exprs"] or st["atomic_api_calls"]:
        b.append("atomics beyond load/store/add")
    if st["volatile_uses"]:
        b.append("volatile")
    if st["varargs"]:
        b.append("varargs")
    if st["bitfields"]:
        b.append("bitfields")
    if st["i128_uses"]:
        b.append("i128")
    if st["inline_asm"]:
        b.append("inline asm")
    return ("ASSIST", "; ".join(b)) if b else ("AUTO", "")

def main():
    tree = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else ".")
    sub = sys.argv[2] if len(sys.argv) > 2 else "lib"
    cc = json.load(open(os.path.join(tree, "compile_commands.json")))
    want = {}
    for e in cc:
        f = e["file"]
        if os.path.dirname(os.path.relpath(f, tree)) == sub and f.endswith(".c"):
            want[os.path.basename(f)] = e
    # Every .c file in the directory is censused. Files the current
    # config does not compile have no compile command; they are parsed
    # with a sibling file's flag set (kernel flags are uniform across
    # the directory) and the row is marked flags=fallback for honesty.
    on_disk = sorted(fn for fn in os.listdir(os.path.join(tree, sub))
                     if fn.endswith(".c"))
    ref = next(iter(want.values()), None)
    rows = []
    for name in on_disk:
        e = want.get(name)
        flags_src = "build" if e else "fallback"
        if e is None:
            e = ref
        path = os.path.join(tree, sub, name)
        args = clean_args(e, e["file"])
        st, err = census_file(path, args)
        cls, why = classify(st, err)
        rows.append({"file": f"{sub}/{name}", "class": cls,
                     "blockers": why, "features": st, "flags": flags_src,
                     "error": (err or "")[:200]})
        print(f"{cls:8s} {sub}/{name}  {why}", flush=True)
    tot = collections.Counter(r["class"] for r in rows)
    blockers = collections.Counter()
    for r in rows:
        for b in r["blockers"].split("; "):
            if b and not b.startswith("clang parse"):
                blockers[b] += 1
    out_dir = os.path.join(tree, "tools/converge")
    os.makedirs(out_dir, exist_ok=True)
    with open(os.path.join(out_dir, f"census-{sub}.json"), "w") as fh:
        json.dump({"dir": sub, "totals": dict(tot), "files": rows}, fh, indent=1)
    with open(os.path.join(out_dir, f"CENSUS-{sub}.md"), "w") as fh:
        fh.write(f"# Convergence census -- {sub}/\n\n")
        fh.write(f"Files scanned: {len(rows)}. Classes: "
                 + ", ".join(f"{k} {v}" for k, v in sorted(tot.items())) + ".\n\n")
        fh.write("## What blocks conversion (files affected)\n\n")
        for b, n in blockers.most_common():
            fh.write(f"- {b}: {n}\n")
        fh.write("\n## Per file\n\n")
        for r in rows:
            fh.write(f"- `{r['file']}` -- {r['class']}"
                     + (f" -- {r['blockers']}" if r["blockers"] else "") + "\n")
    print("TOTALS", dict(tot))
    print("BLOCKERS", dict(blockers.most_common()))

if __name__ == "__main__":
    main()
