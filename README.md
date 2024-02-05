<a name="readme-top"></a>

<!-- ABOUT THE PROJECT -->
## About The Project

Hi! You are on the Audio Spectrum device repository. The device shows different frequencies of the signal (e.g. audio signal) and display them visually. Moreover, the device can just simply do an animation like rolling effect on the led strip - ws2812b. Additionally the led strip can be divided into sectors, which can independently display different animations. Below there is a gif which shows rolling animation based on different sine frequencies and dimming animation on some first diodes. In addition there is a possibility to change sectors and animations. Usb-c cable must be connected to the black pill. To change sectors or animation on that sector You need send proper stream of bytes using USB CDC. You can see what kind of messages are accepted by STM32 in *Test_Communication.c* file.

![App gif](https://github.com/MZdzw/AudioSpectrum/blob/main/img/frequenciesRolling.gif)

Below there is a clip of the audio spectrum animation

![App gif](https://github.com/MZdzw/AudioSpectrum/blob/main/img/audioRolling.gif)

The repository contains code which runs on stm32F401CCU6 - commonly known as "black pill". Moreover, the code which runs on the uC can be tested using unity and cmock framework. The tests can be executed on the host computer. The tests consists of test scenarios, which stubs HAL (Hardware Abstraction Layer) and mainly verify the logic of application and some critical boundary conditions.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

As mentioned above the repository contains code which require build and burn binaries into the uC to get device properly work. Moreover, the production code can be tested using unity and cmock

### Prerequisites

To build binaries on the uC You need:
* make
  ```sh
  make --version
  ```
* cmake
  ```sh
  cmake --version
  ```
* GNU Arm Embedded Toolchain
  ```sh
  arm-none-eabi-gcc --version
  ```
* STM32_Programmer_CLI
  ```sh
  STM32_Programmer_CLI --version
  ```

To build tests You need
* C compiler e.g. gcc   
   ```sh
   gcc --version
   ```
* cmock dependencies as ruby etc. (see cmock documentation)

### STM32 deploy

To run this app you need to follow these steps:

1. Clone the repo
   ```sh
   git clone https://github.com/MZdzw/AudioSpectrum.git
   ```
2. Change directory to the cloned repositry
   ```sh
   cd <cloned repository directory>
   ```
3. "Compile" and link all the files
   ```sh
   cmake -S"." -B"Build" && cd Build && make
   ```
4. Program the microcontroller. This step can be done in different ways (probably previous steps as well :)). I recommend using STM32_Programmer_CLI
   ```sh
   STM32_Programmer_CLI -c port=swd -w Core/AudioSpectrumHAL.bin 0x08000000
   ```
5. It's all done

### Tests
To test the production code You need

1. Compile files
   ```sh
   cmake -S"." -B"Build" -DBUILD_TEST=ON
   cd Build && make
   ```
2. Choose test
   ```sh
   cd Tests/Test_Activity
   ./Test_Activity
   ```
Exisiting tests:
* Test_Activity
* Test_Animations
* Test_Communication
* Test_SpiBuffer
* Test_Ws2812bDriver


<p align="right">(<a href="#readme-top">back to top</a>)</p>

