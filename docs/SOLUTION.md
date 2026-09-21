# SPOILER: Root Cause and Fix

Do not read this until you have attempted the challenge.

## Root cause

`plan_add()` stores a raw pointer returned by `store_get()`:

```c
p->ops[p->len].cached_entry = entry;
```

That pointer points directly into `Store.items`, which is a dynamically allocated array.

Later, `loader_expand_catalog()` repeatedly calls `store_intern()`. When capacity is exhausted, `store_reserve()` calls:

```c
realloc(s->items, new_cap * sizeof(*new_items));
```

`realloc()` is allowed to move the allocation. If it moves, every pointer into the old allocation immediately becomes invalid.

The `Plan` still holds the old addresses. `plan_execute()` later dereferences them:

```c
op->cached_entry->hits += op->amount;
```

That is use-after-realloc: a form of use-after-free / dangling-pointer undefined behavior.

## Why the crash can be somewhere else

The old allocation has been returned to the allocator. `noise_churn()` intentionally performs many allocations and frees, increasing the chance that the freed bytes are reused.

The stale write can therefore modify memory now owned by something completely unrelated. The allocator or application may detect damage only later.

That is why the failure might appear inside `free()`, `malloc()`, program shutdown, or not appear at all.

## Correct fixes

### Preferred: cache stable identity, not unstable address

Change `PlanOp` to store an index:

```c
typedef struct {
    size_t entry_index;
    uint64_t amount;
} PlanOp;
```

Then resolve the pointer at execution time:

```c
Entry *entry = store_get(store, op->entry_index);
entry->hits += op->amount;
```

This requires passing the Store to `plan_execute`.

### Alternative architectural fixes

- Store individually allocated `Entry` objects and keep stable pointers.
- Use handles with generation counters.
- Prevent relocation after pointers escape by reserving all required capacity first.
- Use an arena/slab whose object addresses remain stable.

The important rule is: **never expose a pointer into a relocatable container unless the API also guarantees that no relocation can occur during that pointer's lifetime.**
