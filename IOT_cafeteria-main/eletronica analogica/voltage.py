from Adafruit_ADS1x15 import *

import time

import matplotlib.pyplot as plt

from drawnow import *

adc = ADS1115()

val = [ ]

cnt = 0

plt.ion()

lista = []

tensao = []

# Start continuous ADC conversions on channel 0 using the previous gain value.

adc.start_adc(0)

print('Reading ADS1x15 channel 0')

#create the figure function

def makeFig():

    plt.ylim(-6,6)

    plt.title('Osciloscope')

    plt.grid(True)

    plt.ylabel('ADC outputs')

    plt.plot(val, 'ro-', label='Channel 0')

    plt.legend(loc='lower right')

while (True):

    # Read the last ADC conversion value and print it out.

    value = adc.get_last_result()
    
    value_v=round(value,4) /10000
    
    V_real = 1.248 * value_v + 0.004
    
    current= 3.892919 * V_real - 9.555633
    
    lista.append(current)
  
    print('Channel 0 voltagem : {0}'.format(V_real))
    print('Corrente convertida : {0}'.format(current))
    
    # Sleep for half a second.
    

    #time.sleep(0.0001)

    val.append(current)

    drawnow(makeFig)

    plt.pause(.000001)

    cnt = cnt+1

    if(cnt>50):

        val.pop(0)