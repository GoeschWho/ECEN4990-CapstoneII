# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\megan\Google Drive\College\S11 - Spring 2018\ECEN 4990\Github\ECEN4990-CapstoneII\CapstoneII\CentralBLE.cydsn\CentralBLE.cyprj
# Date: Tue, 03 Apr 2018 23:46:39 GMT
#set_units -time ns
create_clock -name {LCDSPI_SCBCLK(FFB)} -period 62.5 -waveform {0 31.25} [list [get_pins {ClockBlock/ff_div_1}]]
create_clock -name {CyRouted1} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/dsi_in_0}]]
create_clock -name {CyWCO} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/wco}]]
create_clock -name {CyLFClk} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyECO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/eco}]]
create_clock -name {CyIMO} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyHFClk} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/hfclk}]]
create_clock -name {CySysClk} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/sysclk}]]
create_generated_clock -name {LCDSPI_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 3 7} [list]


# Component constraints for C:\Users\megan\Google Drive\College\S11 - Spring 2018\ECEN 4990\Github\ECEN4990-CapstoneII\CapstoneII\CentralBLE.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\megan\Google Drive\College\S11 - Spring 2018\ECEN 4990\Github\ECEN4990-CapstoneII\CapstoneII\CentralBLE.cydsn\CentralBLE.cyprj
# Date: Tue, 03 Apr 2018 23:46:37 GMT
