#!/bin/sh

# Initialize the driver for the I/O expander to expose all of the
# pins in the filesystem.  The table of pins to initialize is at
# the end of the file.

report() {
    if [ "$?" -eq 0 ]; then
        printf "\x1b[32msucceeded\x1b[0m\n"
    else
        printf "\x1b[31mfailed\x1b[0m\n"
    fi
}

setup_gpio() {
    number="$1"
    direction="$2"
    purpose="$3"

    printf "%-22s" "Exporting $number "
    if [ ! -d "/sys/class/gpio/gpio$number" ]; then
        echo "$number" > "/sys/class/gpio/export"
        report
    else
        printf "\x1b[32malready exported\x1b[0m\n"
    fi

    printf "  %-20s" "Setting direction "
    echo "$direction" >> /sys/class/gpio/gpio$number/direction
    report

    if [ "$direction" = "out" ]; then
        printf "  %-20s" "Setting permissions "
        chmod og+rw /sys/class/gpio/gpio$number/value
        report
    fi
}

#          pin  dir   purpose
setup_gpio 138 'out' "blue LED"
setup_gpio 139 'out' "blue LED"
setup_gpio 214 'out' "red LED"
setup_gpio 215 'out' "green LED"
setup_gpio 216  'in' "switch"
setup_gpio 217  'in' "switch" 
setup_gpio 218  'in' "gpio 1" 
setup_gpio 219  'in' "gpio 2" 

# Sumo GPIO
#setup_gpio  137  'in' "Bump 0"
#setup_gpio  136  'in' "Bump 1"
#setup_gpio  143 'out' "Direction 0"
#setup_gpio  158 'out' "Direction 1"

