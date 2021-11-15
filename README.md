## Presentation
https://docs.google.com/presentation/d/1mi4-n4dT8bwS4iG0mJ9_k5yZCrY3XL1ln9aOi6mdWBc/edit?usp=sharing

## Inspiration
As more and more people have been working from home, the problem of natural lighting has been growing. During the pandemic, we personally experienced this issue because we lived in rooms where sunlight was scarce. The constant intensity and hue ceiling light made motivation very dull.


## What it does
The kit fetches the time from an NTP server and drives an LED strip at the corresponding intensity and hue of light.

## How we built it
Using an ESP32, we fetched GMT time from a NTP Server, adjusted GMT time to local time zone, converted current time into an LED intensity value. The LED intensity was driven by adjusting the PWM duty cycle on the MOSFET driver. We built the MOSFET driver using basic components that were lying around (which explains the unnecessary complexity) and simulated it to make sure it can switch up to 10kHz which is the max PWM frequency of the ESP32. 

## Challenges we ran into
We started off using Micropython and ran into issues with its reliability and documentation. We ultimately made the decision to pivot to the Arduino IDE with the focus of producing a demo by the 6PM deadline. We were unfamiliar with the using Arduino libraries, so we didn't understand how all the functions worked; thus, we had a level of uncertainty as we implemented every feature. At first, splitting work between two people was difficult. Mapping the current time of the day to LED color and intensity to the PWM duty cycle was also a challenge without representative hardware from the start. 
On the hardware side, we didn't have all the proper components to create this project with minimal complexity and had to work with what was lying around. At first, the DC-DC buck coverters didn't work up to the 24V advertised voltage and our LEDs only worked at 24V so we weren't able to power our ESP32 without a workaround (we ended up using a dual supply). Our MOSFET wasn't the right fit for our project because the threshold voltage was too high so we had to build a preamplifier BJT circuit. During the demo, there was a stray piece of solder that shorted the BJT pins and caused us to have to edit the code to accomodate. After shaking it off, the circuit works as predicted.

## Accomplishments that we're proud of
Creating a working product within a short time frame
Creating a working product that is useful in our everyday lives
Learning to interface with hardware through an ESP32
Learning about components of our project on the go


## What we learned
How to connect to WIFI through an ESP32
How to fetch and process data retrieved from a server
How to convert the current time into an indexable LED intensity value
How think and plan a project


## What's next for Sun Following Light Therapy LED Kit
Being able to accurately grab location from a server
Determine time-zone from IP-address
Build a chassis to contain the entire module
Designing a custom PCB
Going to market on Amazon
