# Multiprocess Bitcoin Design Document

Guide to the design and architecture of the Bitcoin Core multiprocess feature

_This document describes the design of the multiprocess feature. For usage information, see the top-level [multiprocess.md](../multiprocess.md) file._

## Table of contents

- [Introduction](#introduction)
- [Current Architecture](#current-architecture)
- [Proposed Architecture](#proposed-architecture)
- [Component Overview: Navigating the IPC Framework](#component-overview-navigating-the-ipc-framework)
- [Design Considerations](#design-considerations)
  - [Selection of Cap’n Proto](#selection-of-capn-proto)
  - [Hiding IPC](#hiding-ipc)
  - [Interface Definition Maintenance](#interface-definition-maintenance)
  - [Interface Stability](#interface-stability)
- [Security Considerations](#security-considerations)
- [Example Use Cases and Flows](#example-use-cases-and-flows)
  - [Retrieving a Block Hash](#retrieving-a-block-hash)
- [Future Enhancements](#future-enhancements)
- [Conclusion](#conclusion)
- [Appendices](#appendices)
  - [Glossary of Terms](#glossary-of-terms)
  - [References](#references)
- [Acknowledgements](#acknowledgements)
