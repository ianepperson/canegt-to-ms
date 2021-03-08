# canegt-to-ms
A simple device to translate CAN-EGT signals to Megasquirt (MS) CLT

## Why?

The [CAN-EGT device](https://thedubshop.com/can-egt/) is fantastic in that it reads and stabilizes head and exhaust temperatures and provides the data via CAN-Bus and a voltage output. Unfortunately, Megasquirt ignores this data and requires a separate "coolant" (CLT) analog input so it can know when the engine is warm. When you have an air-cooled engine, you'll need to simulate the coolant signal. The device described here will do just that.

## The Problem

The CAN-EGT provides a 0-5 volt output signal for each of its inputs. 0 volts is 0 degrees C and 5 volts is either 1250 degrees C for an EGT setup, or 600 degrees C for a CHT setup. This is a nice, linear output.

Megasquirt needs a variable resistance to ground, where a high resistance denotes a low temperature and a low resistance denotes a high temperature. The actual values are adjustable in Tuner Studio, but the low temperature must have a higher resistance than the high temperature.

An analog circuit could be constructed to convert the 0-5 volts into the proper resistance to ground, but a tiny microprocessor is cheap and easier to implement.

## The Solution

### Hardware

The [Adafruit Trinket](https://www.adafruit.com/product/1501) is a $6 processor that is powered by a 5-16 volt input, has an input pin that reads precisely 0-5 volts on an input pin, can communicate with another simple chip via [SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface), and can provide that other chip with a stabilized 5 volts. Note that the [Trinket M0](https://www.adafruit.com/product/3500) might work and is radically easier to load code to, but it's only a 3 volt chip and other components would have to be used to translate the 0-5 volt into the 0-3 volt analog input.

[Trinket Pinouts](https://learn.adafruit.com/introducing-trinket/pinouts)

![pin outs](https://cdn-learn.adafruit.com/assets/assets/000/025/645/large1024/trinket5.png?1432753823)

The [MCP4151-103E/P](https://www.digikey.com/en/products/detail/microchip-technology/MCP4151-103E-P/1874217) is an SPI controlled 10k ohm potentiometer in a single chip. (I picked up a [5-pack on Amazon.com](https://www.amazon.com/gp/product/B00LVKK472) for $6, but it can be found at [other sources](https://www.mouser.com/ProductDetail/Microchip-Technology/MCP4151-103E-P?qs=hH%252BOa0VZEiCcBDYaXnd0Yg%3D%3D). Make sure you buy at least two - I melted my first one by reversing the polarity!)

[MCP4151 Pinouts]()

![pin outs](MCP41X1%20Pinout.png)

(Vdd is 5 volts, Vss is ground)

Since Megasquirt does not support a 0 ohm signal, add in a 470 ohm resister in series with the potentiometer. With the Arduino program in this folder, the CLT line will get a 470 ohm signal to represent 600 degrees C and a 10470 ohm signal will represent 0 degrees C.

### Wiring

If you haven't done any electronics tinkering and don't already have a handy set of boards, headers and terminals, it's not a bad idea to pick up [a kit](https://www.amazon.com/s?k=pcb+board+kit).

You'll need 4 connections for external wiring.

 - Switched 12v input. Wire directly to the Trinket's "battery" input.
 - Ground. Wire to the Trinket's ground, and the MCP4151's Vss (4) and P0A (5) pins.
 - 0-5 volt sensor input. Wire to the Trinket's pin "#4" (2nd analog input).
 - Variable resistance output. Wire to a 470 ohm resister, then through to the MCP4151's P0W pin (6).

You'll need just 4 more internal wiring connections.

 - Trinket's 5v output to MCP4151's Vdd pin (8) to power the MCP4151.
 - Trinket's #0 output to MCP4151's CS pin (1).
 - Trinket's #1 output to MCP4151's SDI/SDO pin (3).
 - Trinket's #2 output to MCP4151's SCK pin (2).

(todo: show a wiring diagram)

### Software

Use the [Arduino IDE](https://www.arduino.cc/en/Main.Software) software to connect to the Trinket via USB and load the program. View the file here: [canegt-2-ms.ino](canegt-2-ms.ino), and download it with the [raw link](https://raw.githubusercontent.com/ianepperson/canegt-to-ms/main/canegt-2-ms.ino).


## Finished product

