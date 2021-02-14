# RESFET: Rice Eclipse Software for Engine Testing
[![Build Status](https://travis-ci.org/rice-eclipse/resfet.svg?branch=master)](https://travis-ci.org/rice-eclipse/resfet)

RESFET is a complete remodel of the old
[mk1-1-engine-controller](https://www.github.com/rice-eclipse/mk1-1-engine-controller)
code. It fulfills the same purpose: running persistently on the engine control
box, receiving commands from the operator over TCP and sending data from
sensors to the operator over UDP. However, it aims to be cleaner and more
robust (and better documented :-P) than its predecessor.

[RESFET Dashboard](https://www.github.com/rice-eclipse/resfet-dashboard) is the
frontend that is used to communicate with RESFET.

### Installation
Clone the repository into your desktop:
```bash
git clone https://github.com/rice-eclipse/resfet
```

CD into the directory:  
```bash
cd resfet
```

Install the required dependencies:
```bash
sudo ./install_dependencies.sh
```

Perform full build (runs both cmake and then make):
```bash
./make_force.sh
```

Run the software:
```bash
./build/resfet config.ini
```

If there's any change in the source code that doesn't require full cmake routine, then run:
```bash
./make_build.sh
```

### License
Copyright (c) Rice Eclipse. All rights reserved.

Licensed under [GNU General Public License v3.0](https://github.com/rice-eclipse/resfet/blob/master/LICENSE).
