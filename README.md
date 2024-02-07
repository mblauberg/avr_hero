# AVR Hero

## Introduction

AVR Hero is a custom adaptation of the popular game "Guitar Hero," designed for the ATmega324A microcontroller as part of the CSSE2010 course at the University of Queensland. This project challenges students to apply their C programming skills to add features and functionalities to a basic game template. With inputs from multiple sources and output displayed on a 16x8 LED matrix, alongside additional information on a serial terminal and a seven-segment display, this project offers a hands-on experience with hardware programming and game development.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [Configuration](#configuration)
- [Documentation](#documentation)
- [Troubleshooting](#troubleshooting)
- [Contributors](#contributors)

## Features

- **LED Matrix Display**: Shows notes on a 16x8 LED matrix, providing a visual representation of the game board.
- **Seven Segment Display**: Displays the player's score, enhancing the game interaction.
- **Interactive Gameplay**: Players can 'play notes' using connected buttons, adding to the game's interactivity.
- **Serial Connection**: Offers additional features such as playing notes through keyboard input, changing game speed, and pausing the game, with status displayed on the terminal.
- **Countdown Feature**: Displays a countdown when starting a game, preparing players for the upcoming session.

## Installation

### Prerequisites

- ATmega324A microcontroller
- 16x8 LED matrix
- 4 buttons
- 7-segment display
- Pololu USB AVR Programmer v2.1
- MacBook
- VSCode with PlatformIO extension installed

### Hardware Setup

1. **LED Matrix**: Connect to pins B4-B7 on the ATmega324A.
2. **Buttons**: Connect to pins B0-B3.
3. **Seven-Segment Display**: Connect to pins C0-C7.
4. **Programmer**: Connect the Pololu USB AVR Programmer v2.1 to pins D0 and D1 for serial communication.

## Usage

To run AVR Hero, ensure all hardware is correctly connected and power is supplied to the AVR. Build and upload the software to the ATmega324A using PlatformIO in VSCode. The game can be controlled using the connected buttons or through keyboard input via the serial connection.

## Dependencies

- ATmega324A microcontroller
- Hardware components as listed in [Installation](#installation)
- VSCode and PlatformIO for building and uploading the software

## Configuration

Refer to the `platformio.ini` file for specific configuration details required by PlatformIO to build and upload the project.

## Documentation

The task sheet provided by UQ contains detailed instructions and expectations for the project's development. This document is available in the repository for reference.

## Troubleshooting

For common issues related to hardware connections or software configurations, please refer to the PlatformIO documentation or the AVR programming guide.

## Contributors

- Starter code and initial project idea provided by the University of Queensland.
- Custom features and project development by Michael Blauberg.
