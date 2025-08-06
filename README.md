# RinsingMachine

> [!NOTE]
> Project indefinitely in-progress

## Main code in src
[src/main.cpp](src/main.cpp)

## Library code in lib
[lib/utils](lib/utils/)

## Hardware schematics is in hw
### REV. 4
[hw](hw)
![Schematics render](hw/TheRinsingMachine.png)

## Web path
`192.168.4.1` default IP
- `/` RinsingMachine main website
- `/stock` return current inventory
- `/delay` configure motor interval for each type
- `/set` add 1 more item to inventory
- `/man` control motor manually

inventory is controlled by https://github.com/stamp-cmd/RinsingMachine/blob/16c961063a23b14c88e6f8534ee2f54d6ff69be1/src/main.cpp#L9
