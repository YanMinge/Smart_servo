SET PATH=D:\Keil_v5\ARM\ARMCC\Bin;C:\Users\myan\AppData\Local\Programs\Python\Python35\;C:\Users\myan\AppData\Local\Programs\Python\Python35\Scripts;C:\PROGRAM FILES (X86)\AMD APP\BIN\X86_64;C:\PROGRAM FILES (X86)\AMD APP\BIN\X86;C:\Windows\SYSTEM32;C:\Windows;C:\Windows\SYSTEM32\WBEM;C:\Windows\SYSTEM32\WINDOWSPOWERSHELL\V1.0\;C:\PROGRAM FILES (X86)\ATI TECHNOLOGIES\ATI.ACE\CORE-STATIC;C:\PROGRAM FILES\VANDYKE SOFTWARE\CLIENTS\;D:\PROGRAM FILES\DOXYGEN\BIN;D:\CTEX\USERDATA\MIKTEX\BIN;D:\CTEX\MIKTEX\MIKTEX\BIN;D:\CTEX\CTEX\CTEX\BIN;D:\CTEX\CTEX\CCT\BIN;D:\CTEX\CTEX\TY\BIN;D:\CTEX\GHOSTSCRIPT\GS9.05\BIN;D:\CTEX\GSVIEW\GSVIEW;D:\CTEX\WINEDT;D:\PROGRAM FILES\NODEJS\;C:\Users\myan\AppData\Roaming\npm
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=M058ZDN
SET CPU_VENDOR=Nuvoton
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x00B71B00
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\system_m051series.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\objects\startup_m051series._ia"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\uart_printf.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\systime.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\mygpio.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\mepwm.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\protocol.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\interrupt.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sysinit.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\dataflash.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\smartservo.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\mp9960.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\retarget.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sys.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\uart.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\i2c.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\timer.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\clk.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\gpio.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\fmc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\pwm.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\spi.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\Neuron.lnp"
"D:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\Neuron.axf" --i32combined --output ".\Objects\Neuron.hex"
