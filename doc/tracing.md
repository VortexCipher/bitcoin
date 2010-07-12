# User-space, Statically Defined Tracing (USDT) for Bitcoin Core

Bitcoin Core includes statically defined tracepoints to allow for more
observability during development, debugging, code review, and production usage.
These tracepoints make it possible to keep track of custom statistics and
enable detailed monitoring of otherwise hidden internals. They have
little to no performance impact when unused.

```
eBPF and USDT Overview
======================

                ┌──────────────────┐            ┌──────────────┐
                │ tracing script   │            │ bitcoind     │
                │==================│      2.    │==============│
                │  eBPF  │ tracing │      hooks │              │
                │  code  │ logic   │      into┌─┤►tracepoint 1─┼───┐ 3.
                └────┬───┴──▲──────┘          ├─┤►tracepoint 2 │   │ pass args
            1.       │      │ 4.              │ │ ...          │   │ to eBPF
    User    compiles │      │ pass data to    │ └──────────────┘   │ program
    Space    & loads │      │ tracing script  │                    │
    ─────────────────┼──────┼─────────────────┼────────────────────┼───
    Kernel           │      │                 │                    │
    Space       ┌──┬─▼──────┴─────────────────┴────────────┐       │
                │  │  eBPF program                         │◄──────┘
                │  └───────────────────────────────────────┤
                │ eBPF kernel Virtual Machine (sandboxed)  │
                └──────────────────────────────────────────┘

1. The tracing script compiles the eBPF code and loads the eBPF program into a kernel VM
2. The eBPF program hooks into one or more tracepoints
3. When the tracepoint is called, the arguments are passed to the eBPF program
4. The eBPF program processes the arguments and returns data to the tracing script
```

The Linux kernel can hook into the tracepoints during runtime and pass data to
sandboxed [eBPF] programs running in the kernel. These eBPF programs can, for
example, collect statistics or pass data back to user-space scripts for further
processing.

[eBPF]: https://ebpf.io/
