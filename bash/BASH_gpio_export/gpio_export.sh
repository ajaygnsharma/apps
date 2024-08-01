start() {
    printf "Creating GPIOs\n"

    # These are output for MUX(PA23,PA24,PA25)
    bank=0;
    for n in 23 24 25 
    do
      echo "$(((bank*16)+n)) > /sys/class/gpio/export"	
      echo "PA$(((bank*16)+n)) export OK" 
      echo "out > /sys/class/gpio/PA$(((bank*16)+n))/direction"
      echo "PA$(((bank*16)+n)) output OK"
    done

    # Various Port(Bank) B outputs.
    bank=1;
    for n in 11 12 13 14 17 18 21 23
    do
      echo "$(((bank*16)+n)) > /sys/class/gpio/export"	
      echo "PB$((n)) export OK"
      echo "out > /sys/class/gpio/PB$(((bank*16)+n))/direction"
      echo "PB$((n)) output OK"
    done
	
	# ALM_OUT, V_LEVEL_0 and V_LEVEL_1 need to be high
	echo "1 > /sys/class/gpio/PB11/value"
	echo "1 > /sys/class/gpio/PB17/value"
	echo "1 > /sys/class/gpio/PB18/value"
	
	# Wave guide switch needs to be low. Off position, 22KHz off, internal 10MHz off.
	echo "0 > /sys/class/gpio/PB12/value"
	echo "0 > /sys/class/gpio/PB13/value"
	echo "0 > /sys/class/gpio/PB14/value"
	echo "0 > /sys/class/gpio/PB21/value"
	echo "0 > /sys/class/gpio/PB23/value"
    
    # These are input for alarm
    bak=2;
    for n in 23 24 25 29 30 
    do
    	echo "$(((bank*16)+n)) > /sys/class/gpio/export"	
    	echo "PC$((n)) export OK" || echo "FAIL"
    done
}

start;