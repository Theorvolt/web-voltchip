
<br />
<p align="center">
  <a href="https://github.com/Theorvolt/web-voltchip">
    <img src="assets/logo.png" alt="Logo" width="120" height="120">
  </a>

  <h3 align="center">VoltChip</h3>

  <p align="center">
    A Chip-8 emulator written in C and transpiled to WebAssembly.
    <br />
    <a href="https://github.com/Theorvolt/web-voltchip/issues">Report Bug / Request Feature</a>
  </p>
</p>


# Table of Contents

1. [What is this project?](#what-is-it?)
    1. [Technical details](##technical-details)

2. [Getting started](#getting-started)
    1. [Building](##build-instructions)

3. [Contributing](#contributing)

4. [License](#license)

5. [Contact](#contact)

6. [Extra](#extra)
   1. [Why?](#why?)





# What is it?

Chip-8 is a simple interpreted programming language developed during the 1970s and was designed for various personal computer systems. It is widely considered an easy emulator to design due to the small number of opcodes required to implement. This project intends to create an emulator in C then use Emscripten to generate WASM files so that the emulator can be deployed as a JS script.


# Getting Started

To get a local copy up and running follow these steps.

## Build Instructions

1. Run `make all` if you do not wish to target WASM, and `make wasm` if you wish to target WASM.
2. If you wish to use the emulator from the browser, run the following:
   ```
   python http.server 8080
   ```
   Then head over to http://localhost:8080 which will load the site.


# Contributing

Simply fork the repository, modify it as you please then submit a pull request which will then be merged.

# License

Distributed under the MIT License. See `LICENSE` for more information.


# Contact

Sai kumar Murali krishnan - [LinkedIn](https://www.linkedin.com/in/sai-kumar-murali-krishnan/) - theorvoltbusiness@gmail.com

Project Link: [https://github.com/Theorvolt/web-voltchip](https://github.com/Theorvolt/web-voltchip)


# Extra

## Why?

I've always had an interest in emulation, and decided it would be an interesting idea to utilise WebAssembly to make an easily accessible emulator. The language chosen for the actual emulation wouldn't be too important, as the porting would be the more fascinating aspect of this project.

