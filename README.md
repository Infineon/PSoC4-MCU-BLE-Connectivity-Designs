## Table of Contents

* [Introduction](#introduction)
* [Navigating the Repository](#navigating-the-repository)
* [Required Tools](#required-tools)
* [Code Examples List](#code-examples-list)
* [References](#references)

# Introduction

This repository contains examples and demos for [PSoC 4 MCU](http://www.cypress.com/products/32-bit-arm-cortex-m0-psoc-4) family of devices, world's Most Flexible 32-bit ARM Cortex-M0 One-Chip Solution. Cypress provides a wealth of data at [www.cypress.com](http://www.cypress.com/) to help you select the right PSoC device and effectively integrate it into your design. Visit our [PSoC 4 MCU](http://www.cypress.com/products/32-bit-arm-cortex-m0-psoc-4) webpage to explore more about PSoC 4 MCU family of device.

Feel free to explore through the code example source files and Let us innovate together!

# Navigating the Repository

The examples in this repository demonstrates PSoC 4 based Bluetooth Low Energy (BLE) connectivity feature. These code examples show how to integrate the complex BLE protocol into your design effortlessly.
If you are new to developing projects with PSoC 4 MCU, we recommend you to refer the [PSoC 4 Getting Started](https://github.com/cypresssemiconductorco/PSoC-4-MCU-Getting-Started) GitHub page which can help you familiarize with device features and guides you to create a simple PSoC 4 design with PSoC Creator IDE. For other block specific design please visit the following GitHub Pages:

#### 1. [Analog Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Analog-Designs)
#### 2. [Capacitive Touch Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Capacitive-Touch-Designs)
#### 3. [Digital Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Digital-Designs)
#### 4. [BLE Connectivity Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-BLE-Connectivity-Designs)
#### 5. [USB Connectivity Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-USB-Connectivity-Designs)
#### 6. [Device Related Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Device-Related-Designs)
#### 7. [PSoC 4 Pioneer Kit](https://github.com/cypresssemiconductorco/PSoC4-MCU-Pioneer-Kits)
#### 8. [System Design](https://github.com/cypresssemiconductorco/PSoC4-MCU-System-Designs)

You can use these block level examples to guide you through the development of a system-level design. All the code examples in this repository comes with well documented design guidelines to help you understand the design and how to develop it. The code examples and their associated documentation are in the Code Example folder in the repository.

# Required Tools

## Software
### Integrated Development Environment (IDE)
To use the code examples in this repository, please download and install
[PSoC Creator](http://www.cypress.com/products/psoc-creator)

## Hardware
### PSoC 4 MCU Development Kits
* [CY8CKIT-042-BLE-A Bluetooth® Low Energy 4.2 Compliant Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-042-ble-bluetooth-low-energy-42-compliant-pioneer-kit).

* [CY8CKIT-041-41XX PSoC 4100S CapSense Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-041-41xx-psoc-4100s-capsense-pioneer-kit). 

* [CY8CKIT-046 PSoC 4 L-Series Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-046-psoc-4-l-series-pioneer-kit)

* [CY8CKIT-044 PSoC Analog Coprocessor Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-048-psoc-analog-coprocessor-pioneer-kit)

* [CY8CKIT-049-4xxx PSoC 4 Prototyping Kit](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits)

* [CY8CKIT-042 PSoC 4 Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-042-psoc-4-pioneer-kit) 

**Note** Please refer to the code example documnetation for selecting the appropriate kit for testing the project

## Code Example List
#### 1. BLE_Battery_Level
This project demonstrates measurements of the battery voltage using PSoC 4 BLE/PRoC BLE's internal ADC and notifies the BLE central device of any change in the battery voltage.

## References
#### 1. PSoC 4 MCU
PSoC 4 is the world's Most Flexible 32-bit ARM Cortex-M0 One-Chip Solution. PSoC 4 has tackled some of the complex portions of embedded system design making it easier for you to get your product to market. Functions such as analog sensor integration, capacitive touch, and wireless connectivity have been integrated and optimized in PSoC 4 to “just work” so you don’t have to. To learn more on the device. Learn more: [PSoC 4 MCU](http://www.cypress.com/products/32-bit-arm-cortex-m0-psoc-4)

####  2. PSoC 4 MCU Learning resource list
##### 2.1 PSoC 4 MCU Datasheets
Device datasheets list the features and electrical specifications of PSoC 4 families of devices: [PSoC 4 MCU Datasheets](http://www.cypress.com/search/all?f%5b0%5d=meta_type%3Atechnical_documents&f%5b1%5d=field_related_products%3A1297&f%5b2%5d=resource_meta_type%3A575)
##### 2.2 PSoC 4 MCU Application Notes
Application notes are available on the Cypress website to assist you with designing your PSoC application: [A list of PSoC 4 MCU ANs](https://community.cypress.com/external-link.jspa?url=http%3A%2F%2Fwww.cypress.com%2Fsearch%2Fall%3Ff%255b0%255d%3Dmeta_type%253Atechnical_documents%26f%255b1%255d%3Dfield_related_products%253A1297%26f%255b2%255d%3Dresource_meta_type%253A574)
##### 2.3 PSoC 4 MCU Component Datasheets
PSoC Creator utilizes "components" as interfaces to functional Hardware (HW). Each component in PSoC Creator has an associated datasheet that describes the functionality, APIs, and electrical specifications for the HW. You can access component datasheets in PSoC Creator by right-clicking a component on the schematic page or by going through the component library listing. You can also access component datasheets from the Cypress website: [PSo 4 Component Datasheets](https://community.cypress.com/external-link.jspa?url=http%3A%2F%2Fwww.cypress.com%2Fsearch%2Fall%3Ff%255b0%255d%3Dmeta_type%253Asoftware_tools%26f%255b1%255d%3Dfield_related_products%253A1297%26f%255b2%255d%3Dsoftware_tools_meta_type%253A532)
##### 2.4 PSoC 4 MCU Technical Reference Manuals (TRM)
The TRM provides detailed descriptions of the internal architecture of PSoC 4 devices:[PSoC 4 MCU TRMs](https://community.cypress.com/external-link.jspa?url=http%3A%2F%2Fwww.cypress.com%2Fsearch%2Fall%3Ff%255b0%255d%3Dmeta_type%253Atechnical_documents%26f%255b1%255d%3Dfield_related_products%253A1297%26f%255b2%255d%3Dresource_meta_type%253A583)

## FAQ

### Technical Support
If you have any queries or questions, our technical support team would be happy to assist you. You can create a support request at [This link] (https://secure.cypress.com/myaccount/?id=25&techSupport=1), or if you are in the United States, you can talk to our technical support team by calling our toll-free number +1-800-541-4736 and then selecting option 8 at the IVR prompt. 

You can also use the following support resources if you need quick assistance: 
Self-help: [http://www.cypress.com/support] (http://www.cypress.com/support)
Local Sales office locations: [http://www.cypress.com/about-us/sales-offices] (http://www.cypress.com/about-us/sales-offices)
